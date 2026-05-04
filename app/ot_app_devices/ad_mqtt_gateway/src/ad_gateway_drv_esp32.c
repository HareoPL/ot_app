
/**
 * @file ad_gateway_drv_esp32.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-05-2026
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2026 
 * 
 */
#include "ad_gateway_drv.h"
#include "ad_gateway_drv_esp32.h"

#include "ot_app.h"

#include "esp_modem_api.h"
#include "mqtt_client.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"

#include "hal/gpio_types.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "ad_gateway_drv_esp32 "



static ad_gw_drv_event_cb_t event_cb = NULL;

static esp_mqtt_client_handle_t mqtt_client = NULL;
static esp_modem_dce_t *dce = NULL;
static esp_netif_t *ppp_netif = NULL;

static TaskHandle_t modem_connect_task_handle = NULL;
static uint8_t modem_isShuttingDown = 0;

static int8_t init(void);

static int8_t modem_connect(void);
static int8_t modem_turnOffSequence(void);

static int8_t mqtt_publish(const char *topic, const uint8_t *payload, uint16_t len, uint8_t qos, uint8_t retain);
static int8_t mqtt_subscribeSingle(const char *topic, uint8_t qos);
static int8_t mqtt_unSubscribeSingle(const char *topic);

static ad_gw_drv_t gatewayEsp32 = {
    .init = init,
    
    .modem.connect = modem_connect,
    .modem.connectCheck = NULL,
    .modem.disconnect = modem_turnOffSequence,
    
    .mqtt.publish = mqtt_publish,
    .mqtt.subscribeSingle = mqtt_subscribeSingle,
    .mqtt.unSubscribeSingle = mqtt_unSubscribeSingle,
};

/////////////////////////////////////
////  MQTT SECTION
////

// MQTT EVENT HANDLER
static void mqtt_eventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) 
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) 
    {
        case MQTT_EVENT_CONNECTED:
        
            if(event_cb != NULL) event_cb(AD_GW_DRV_MQTT_CONNECTED, NULL);
            break;

        case MQTT_EVENT_DISCONNECTED:
            modem_connect();
            
            if(event_cb != NULL) event_cb(AD_GW_DRV_MQTT_DISCONNECTED, NULL);
            break;

        case MQTT_EVENT_ERROR:
            if(event_cb != NULL) event_cb(AD_GW_DRV_MQTT_ERROR, NULL);
            break;

        case MQTT_EVENT_DATA:
            ad_gw_drv_mqtt_data_t msg;
                        
            snprintf(msg.topic, sizeof(msg.topic), "%.*s", event->topic_len, event->topic);
            snprintf(msg.data,  sizeof(msg.data),  "%.*s", event->data_len,  event->data);

            if(event_cb != NULL) event_cb(AD_GW_DRV_MQTT_DATA, (void*)&msg);

            break;

        default:
            break;
    }
}

static void mqtt_start(void) 
{
    if (mqtt_client != NULL) return; // Double start prevention

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER, // broker.hivemq.com
        .session.keepalive = MQTT_KEEPALIVE,
        .network.timeout_ms = MQTT_TIMEOUT_MS,
        .buffer.size = MQTT_BUFFER_TX_RX_SIZE, 
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_eventHandler, NULL);
    esp_mqtt_client_start(mqtt_client);    
}

static int8_t mqtt_publish(const char *topic, const uint8_t *payload, uint16_t len, uint8_t qos, uint8_t retain)
{

    if (mqtt_client) 
    {        
        esp_mqtt_client_publish(mqtt_client, topic, (const char*)payload, len, qos, retain);  
        
        return 0;
    }

    return -1;
}

static int8_t mqtt_subscribeSingle(const char *topic, uint8_t qos)
{
    if (mqtt_client) 
    {        
        esp_mqtt_client_subscribe_single(mqtt_client, topic, qos);
        
        return 0;
    }

    return -1;
}

static int8_t mqtt_unSubscribeSingle(const char *topic)
{
    if (mqtt_client) 
    {   
        esp_mqtt_client_unsubscribe(mqtt_client, topic);
        
        return 0;
    }

    return -1;
}

/////////////////////////////////////
////  MODEM SECTION
////

// IP EVENT HANDLING 
static void modem_onIpEvent(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) 
{
    if (event_id == IP_EVENT_PPP_GOT_IP) 
    {        
        if(event_cb != NULL) event_cb(AD_GW_DRV_GOT_IP, NULL);
        
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        OTAPP_PRINTF(TAG, "IP: " IPSTR, IP2STR(&event->ip_info.ip));

        if(ppp_netif == NULL) return;

        esp_netif_set_default_netif(ppp_netif);
        mqtt_start();

    } else if (event_id == IP_EVENT_PPP_LOST_IP) 
    {
        // PPP connection lost
        modem_connect();
        if(event_cb != NULL) event_cb(AD_GW_DRV_LOST_IP, NULL);
    }
}
 
static void modem_connectTaskDelete(void)
{
    modem_connect_task_handle = NULL;
    vTaskDelete(NULL); 
}


static void modem_connectTask(void *arg) 
{       
    if(dce == NULL) 
    {
        OTAPP_PRINTF(TAG, "ERROR DCE is NULL \n");
        modem_connectTaskDelete();
        return;
    }
    
    esp_err_t sync_err = esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND); // Soft Reset - return to COMMAND mode
    
    if (sync_err != ESP_OK) 
    {
        OTAPP_PRINTF(TAG, "ERROR Soft reset failed! The modem does not respond to +++. \n");
        // NOTE: If we have reached here, the modem is physically frozen. 
        // At this point in the future, you should trigger a hardware restart of the pin (Hard Reset).
        // FEATURE
        modem_connectTaskDelete(); 
        return;
    }
  
    esp_modem_at(dce, "ATH", NULL, 1000); // forcing old session, disconnect ATH

    int state = 0;
    int retries = 0;
    const int MAX_RETRIES = MODEM_CONNECT_TASK_RETRIES_MAX; 

    OTAPP_PRINTF(TAG, "Whait when SIM CARD will connect to network ...\n");

    while (retries < MAX_RETRIES) 
    {
        // download the GPRS/LTE registration status
        esp_err_t err = esp_modem_get_network_attachment_state(dce, &state);
        
        if (err == ESP_OK && (state == 1 || state == 5)) {
            OTAPP_PRINTF(TAG, "Logged in to the network! (Status: %d)\n", state);
            break; // aut of the loop, whem modem is online
        }

        OTAPP_PRINTF(TAG, "Not online yet, Attempt: %d/%d\n", retries, MAX_RETRIES);
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        retries++;
    }

    if (retries >= MAX_RETRIES) 
    {
        OTAPP_PRINTF(TAG, "ERROR SIM card can not login to network\n");
        // feature MODEM HW RESET here
        modem_connectTaskDelete();
        return;
    }

    OTAPP_PRINTF(TAG, "Logged in. Wait 2 seconds for the PDP context to stabilize... \n");
    vTaskDelay(pdMS_TO_TICKS(2000));

    OTAPP_PRINTF(TAG, "switch to DATA mode(PPP)... \n");
    esp_err_t mode_err = esp_modem_set_mode(dce, ESP_MODEM_MODE_DATA); // Open tunnel PPP...
    
    if (mode_err != ESP_OK) 
    {
        OTAPP_PRINTF(TAG, "ERROR failed to enable data mode \n");
    } else {
        OTAPP_PRINTF(TAG, "Modem in to DATA mode. LwIP negotiates IP in the background... \n");
    }

    modem_connectTaskDelete(); // The task can end and the rest will be taken care of by the on_ip_event callback
}


static void modem_hardwareInitTask(void *arg) 
{  
    uint32_t delay_ms = 20000; 
    const uint32_t MAX_DELAY_MS = 3600000;

    // Hardware Configuration (UART)
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    dte_config.uart_config.tx_io_num = GPIO_NUM_6;
    dte_config.uart_config.rx_io_num = GPIO_NUM_7;
    dte_config.uart_config.port_num = UART_NUM_1;

    // Modem DCE config
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(MODEM_APN);

    while (1) 
    {
        OTAPP_PRINTF(TAG, "Enable modem power...\n");
        // gpio_set_level(MODEM_PWRKEY_PIN, 0); // FEATURE modem power control using mosfet
        
        dce = esp_modem_new_dev(MODEM_MODEL, &dte_config, &dce_config, ppp_netif);
        
        if (dce != NULL) 
        {
            int retries = 0;
            const int MAX_BOOT_RETRIES = MODEM_HW_INIT_TASK_RETRIES_MAX;
            esp_err_t sync_err = ESP_FAIL;

            // Polling AT
            while (sync_err != ESP_OK && retries < MAX_BOOT_RETRIES) 
            {
                sync_err = esp_modem_sync(dce); // send AT and wait for "OK"
                
                if (sync_err != ESP_OK) 
                {
                    OTAPP_PRINTF(TAG, "modem is not responding... Attempt: %d/%d\n", retries + 1, MAX_BOOT_RETRIES);
                    vTaskDelay(pdMS_TO_TICKS(1000)); 
                    retries++;
                }
            }

            // SUKCES
            if (sync_err == ESP_OK) 
            {
                OTAPP_PRINTF(TAG, "hardware woken up. Lanch modem_conn_task (PPP) \n");        
                modem_connect(); // hardware works, start connect task
            
                vTaskDelete(NULL); 
                return; //protects against exiting vTaskDelete
            }
            
            OTAPP_PRINTF(TAG, "ERROR modem did not get up. Hardware ERROR\n");
            esp_modem_destroy(dce); 
            dce = NULL;
        }
        else
        {
            // Memory / system failure
            OTAPP_PRINTF(TAG, "ERROR DCE did not create! Lack of RAM or UART busy.\n");
        }
        
        OTAPP_PRINTF(TAG, "Disable modem power...\n");
        // gpio_set_level(MODEM_PWRKEY_PIN, 1); // FEATURE modem power control using mosfet

        // EXPONENTIAL BACKOFF 
        OTAPP_PRINTF(TAG, "Waiting %lu ms before next boot attempt...\n", delay_ms);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));

        // extending the waiting time twice before the next loop!
        delay_ms *= 2; 
        
        // make sure not to exceed the limit (e.g. 1 hour)
        if (delay_ms > MAX_DELAY_MS) 
        {
            delay_ms = MAX_DELAY_MS; 
        }
    }
}


static int8_t modem_connect(void)
{
    if (modem_isShuttingDown) 
    {
        OTAPP_PRINTF(TAG, "Modem is shuting down \n");
        return -2; // poprawic to nie jest dokonca blad.  
    }

    if(modem_connect_task_handle == NULL)
    {
        BaseType_t xReturned = xTaskCreate( modem_connectTask, "modem_conn_task", 4096, NULL, 5, &modem_connect_task_handle);
        
        if (xReturned != pdPASS) 
        {
            OTAPP_PRINTF(TAG, "ERROR: failed to create RTOS modem_conn_task \n");
            return -1; // fatal error out of RAM
        }
    }

    OTAPP_PRINTF(TAG, "modem connect task is running \n");
    return 0;    
}

static int8_t modem_turnOffSequence() 
{
    OTAPP_PRINTF(TAG, "begin procedure for safely disabling modem... \n");

    // enable shuting down flag
    modem_isShuttingDown = 1;

    // disconnect the MQTT client at the application level (so that LwIP does not hold sockets)
    esp_mqtt_client_disconnect(mqtt_client);
    vTaskDelay(pdMS_TO_TICKS(500));                   // give a moment to send the DISCONNECT package

    esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND); // We exit the DATA mode to the COMMAND mode (Close PPP)
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    esp_modem_at(dce, "AT+COPS=2", NULL, 2000);     // Deregister from network
    
    // Command to safely shut down the system inside the modem 
    // AT+CPOWD=1 is a command for SIM800 to safely shut down the file system 
    // and save the data to the SIM card before cutting off the power    
    esp_modem_at(dce, "AT+CPOWD=1", NULL, 2000);    // power off command
    vTaskDelay(pdMS_TO_TICKS(2000));                // modem needs around 2-3 sec for turn off

    // disable modem power  (feature mosfet control)
    // gpio_set_level(MODEM_POWER_PIN, 0);
    
    OTAPP_PRINTF(TAG, "modem is safely turned off \n");
    
    // disable shuting down flag
    modem_isShuttingDown = 0;

    return 0;
}

//  static void init(char *apn, char *mqttBrokerIp)
static int8_t init(void)
{
    // nvs_flash_init(); // it was initialized in open_thread

    ESP_ERROR_CHECK(esp_netif_init());

    // create wirtual PPP interface 
    esp_netif_config_t netif_ppp_config = {
        .base = ESP_NETIF_BASE_DEFAULT_PPP,
        .driver = NULL,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_PPP
    };

    ppp_netif = esp_netif_new(&netif_ppp_config);
    if (ppp_netif == NULL) 
    {
        OTAPP_PRINTF(TAG, "ERROR filed to create NETIF \n");
        return -1;
    }
    // register modem ip event 
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_PPP_GOT_IP, &modem_onIpEvent, NULL));

    // start modem hw init task
    xTaskCreate(modem_hardwareInitTask, "modem_hw_init", 4096, NULL, 5, NULL);

    return 0; 
}


 void ad_gw_drv_register_event_cb(ad_gw_drv_event_cb_t callback)
 {
    event_cb = callback;
 }

 ad_gw_drv_t *ad_gateway_drv_getEsp32(void)
 {
    return &gatewayEsp32;
 }