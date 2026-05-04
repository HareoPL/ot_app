
#include "ad_gateway_drv.h"
#include "ad_gateway_drv_esp32.h"

#include "esp_modem_api.h"
#include "mqtt_client.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"

#include "hal/gpio_types.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #include "nvs_flash.h"
// #include "nvs.h"

#define MODEM_TXD_PIN           (GPIO_NUM_6)
#define MODEM_RXD_PIN           (GPIO_NUM_7)
#define MODEM_UART_PORT         (UART_NUM_1)
#define MODEM_APN               ("internet")
#define MQTT_BROKER             ("mqtt://35.157.202.98") // broker.hivemq.com
#define MQTT_KEEPALIVE          60
#define MQTT_TIMEOUT_MS         10000


static ad_gw_drv_event_cb_t event_cb = NULL;

static int8_t init(void);
// static int8_t connect(void);

static int8_t publish(const char *topic, const uint8_t *payload, uint16_t len, uint8_t qos, uint8_t retain);

// --- ZMIENNE GLOBALNE ---
static ad_gw_drv_t gatewayEsp32 = {
    .init = init,
    
    .modem.connect = NULL,
    .modem.connectCheck = NULL,
    .modem.disconnect = NULL,
    
    .mqtt.publish = publish,
    .mqtt.subscribeSingle = NULL,
    .mqtt.unSubscribeSingle = NULL,
};

static esp_mqtt_client_handle_t mqtt_client = NULL;
static esp_modem_dce_t *dce = NULL;
static esp_netif_t *ppp_netif = NULL;
// --- OBSŁUGA MQTT ---
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    // esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            // ESP_LOGI("APP", "MQTT: Połączono z brokerem!");
            if(event_cb != NULL) event_cb(AD_GW_DRV_MQTT_CONNECTED, NULL);
            break;

        case MQTT_EVENT_DISCONNECTED:
            // ESP_LOGW("APP", "MQTT: Rozłączono!");
            if(event_cb != NULL) event_cb(AD_GW_DRV_MQTT_DISCONNECTED, NULL);
            break;

        case MQTT_EVENT_ERROR:
            // ESP_LOGE("APP", "MQTT: Błąd!");
            if(event_cb != NULL) event_cb(AD_GW_DRV_MQTT_ERROR, NULL);
            break;

        default:
            break;
    }
}

static void mqtt_start(void) 
{
    if (mqtt_client != NULL) return; // Zapobieganie podwójnemu startowi

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER, // broker.hivemq.com
        .session.keepalive = MQTT_KEEPALIVE,
        .network.timeout_ms = MQTT_TIMEOUT_MS,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);    
}


// OBSŁUGA ZDARZEŃ IP 
static void on_ip_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) 
{
    if (event_id == IP_EVENT_PPP_GOT_IP) {
        
        if(event_cb != NULL) event_cb(AD_GW_DRV_GOT_IP, NULL);
        
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI("APP", "IP: " IPSTR, IP2STR(&event->ip_info.ip));
        
        mqtt_start();
    } else if (event_id == IP_EVENT_PPP_LOST_IP) {

        // ESP_LOGW("APP", "Utracono połączenie PPP");
        if(event_cb != NULL) event_cb(AD_GW_DRV_LOST_IP, NULL);
    }
}

static int8_t publish(const char *topic, const uint8_t *payload, uint16_t len, uint8_t qos, uint8_t retain)
{
    if (mqtt_client) {
        
        esp_mqtt_client_publish(mqtt_client, topic, (const char*)payload, len, qos, retain);   
        
        return 0;
    }

    return -1;
}
 

// // --- ZADANIE W TLE (Asynchroniczna pętla) ---
// static void modem_connection_task(void *pvParameters) {
    
//     esp_modem_dce_t *dce = (esp_modem_dce_t *)self->hw_context;
    
//     int retry_count = 0;
//     const int max_retries = 15;
//     bool connected = false;

//     ESP_LOGI("MODEM_ESP32", "Zadanie łączenia z siecią wystartowało w tle...");

//     while (retry_count < max_retries) {
//         if (esp_modem_set_mode(dce, ESP_MODEM_MODE_DATA) == ESP_OK) {
//             connected = true;
//             break; // Mamy sieć!
//         }
//         ESP_LOGW("MODEM_ESP32", "Szukam sieci (próba %d/%d). Czekam 2s...", retry_count + 1, max_retries);
//         vTaskDelay(pdMS_TO_TICKS(2000));
//         retry_count++;
//     }

//     // --- NOTYFIKACJA (Callback do warstwy wyższej) ---
//     if (connected) {
//         ESP_LOGI("MODEM_ESP32", "Sukces! Powiadamiam bramkę...");
//         // Zdarzenie: Połączono! (Spowoduje odpalenie MQTT w ad_gateway.c)
//         if (self->event_cb) {
//             self->event_cb(AD_MODEM_EVENT_CONNECTED, self->event_context);
//         }
//     } else {
//         ESP_LOGE("MODEM_ESP32", "Timeout! Powiadamiam bramkę o błędzie...");
//         // Zdarzenie: Błąd (Bramka może np. wymusić Hard Reset)
//         if (self->event_cb) {
//             self->event_cb(AD_MODEM_EVENT_ERROR, self->event_context);
//         }
//     }

//     // Zabijamy to zadanie, bo spełniło swój cel
//     vTaskDelete(NULL); 
// }


// static int8_t connect(void)
// {
//     ESP_LOGI("MODEM_ESP32", "Zlecam połączenie w tle...");

//     BaseType_t xReturned = xTaskCreate( modem_connection_task, "modem_conn_task", 4096, NULL, 5, NULL);

//     if (xReturned == pdPASS) {
//         return 0; // Sukces: Zadanie wystartowało pomyślnie
//     } else {
//         ESP_LOGE("MODEM_ESP32", "Błąd: Nie udało się utworzyć zadania RTOS!");
//         return -1; // Błąd krytyczny braku RAM-u
//     }
// }


//  static void init(char *apn, char *mqttBrokerIp)
 static int8_t init(void)
 {
    // nvs_flash_init(); // it was initialized in open_thread

    ESP_ERROR_CHECK(esp_netif_init());

    // PPP interface configure
    esp_netif_config_t netif_ppp_config = {
    .base = ESP_NETIF_BASE_DEFAULT_PPP,
    .driver = NULL,
    .stack = ESP_NETIF_NETSTACK_DEFAULT_PPP
    };
    ppp_netif = esp_netif_new(&netif_ppp_config);

    // Konfiguracja sprzętu (UART)
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    dte_config.uart_config.tx_io_num = GPIO_NUM_6;
    dte_config.uart_config.rx_io_num = GPIO_NUM_7;
    dte_config.uart_config.port_num = UART_NUM_1;

    // Konfiguracja modemu (DCE)
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(MODEM_APN);

    dce = esp_modem_new_dev(ESP_MODEM_DCE_SIM800, &dte_config, &dce_config, ppp_netif);
    if (dce == NULL) 
    {
        ESP_LOGE("APP", "Błąd: Nie udało się stworzyć urządzenia modem!");
    }

    // Rejestracja zdarzeń
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_PPP_GOT_IP, &on_ip_event, NULL));
    
    ESP_LOGI("APP", "START MODEMU !!!! ");
    // START: Przełączenie w tryb danych
    esp_modem_set_mode(dce, ESP_MODEM_MODE_DATA);

    return 0;
 }

 ad_gw_drv_t *ad_gateway_drv_getEsp32(void)
 {
    return &gatewayEsp32;
 }

 void ad_gw_drv_register_event_cb(ad_gw_drv_event_cb_t callback)
 {
    event_cb = callback;
 }