/**
 * @file ad_mqtt_gateway.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-09-2025
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2025 
 * 
 * 
 */

#include "ot_app_drv.h"
#include "ad_gateway.h"

#include "freertos/queue.h"
#include "ws2812b_fx.h"

#define TAG "ad_mqtt_gateway "

static QueueHandle_t mqtt_queue;
static ad_gw_drv_t *gw = NULL;

typedef struct 
{
    ad_gw_drv_event_t event;
    char *name;
}ad_gw_event_name_t;

static ad_gw_event_name_t event_name[] = {
    {AD_GW_DRV_CONNECTED            , "modem connected"},
    {AD_GW_DRV_DISCONNECTED         , "modem disconnected"},
    {AD_GW_DRV_GOT_IP               , "got ip addr"},
    {AD_GW_DRV_LOST_IP              , "lost ip addr"},

    {AD_GW_DRV_MQTT_CONNECTED       , "MQTT connected with broker"},
    {AD_GW_DRV_MQTT_DISCONNECTED    , "MQTT disconnected"},
    {AD_GW_DRV_MQTT_SUBSCRIBED      , "MQTT subscribed"},
    {AD_GW_DRV_MQTT_UNSUBSCRIBED    , "MQTT unsubscribed"},
    {AD_GW_DRV_MQTT_PUBLISHED       , "MQTT published"},
    {AD_GW_DRV_MQTT_DATA            , "MQTT data"},
    {AD_GW_DRV_MQTT_ERROR           , "MQTT error"},
};
#define AD_GW_EVENT_NAME_SIZE (sizeof(event_name) / sizeof(event_name[0]))


/**
 * @brief Device name group buffer
 * 
 * @details
 * Internal buffer storing the device name group identifier used for network pairing.
 * 
 * **Buffer Properties:**
 * - Size: OTAPP_DEVICENAME_SIZE (10 bytes)
 * - Maximum string length: 9 characters + null terminator
 * - Initialized by Init(char *deviceNameGroup)
 * - Used by OpenThread framework for device discovery and pairing
 * 
 * **Purpose:**
 * - Identifies this button device on the OpenThread network
 * - Filters pairing: only devices with matching name group can pair
 * - Enables logical grouping of devices (e.g., by room or function)
 * 
 * @note This buffer persists for the lifetime of the application
 * @note Content is set once during initialization via strcpy()
 * @note Buffer is assigned to drv->deviceName during Init()
 * 
 * @see Init() for initialization
 * @see OTAPP_DEVICENAME_SIZE for buffer size definition
 */
// static char *deviceNameTab = otapp_noGroupDeviceName; // "no_group" drv->api.devName.getNoGroupNamePtr()

/**
 * @brief Device type identifier
 * @note Declared as OTAPP_GATEWAY 
 */
static const otapp_deviceType_t deviceType = OTAPP_GATEWAY;

/**
 * @brief Static pointer to device driver instance
 * @note Set during Init() and used throughout the module
 */
static ot_app_devDrv_t *drv;

//////////////////////
// PAIRING RULES
//////////////////////

/**
 * @brief Pairing rule set that allows all devices without restrictions
 * 
 * @details
 * This rule set permits pairing with any OpenThread device regardless of type.
 * Useful for development, testing, or flexible deployment scenarios.
 * 
 * Configuration:
 * - OTAPP_PAIR_NO_RULES: Special flag indicating no filtering
 * - OTAPP_PAIR_END_OF_RULES: Terminator for rule array
 * 
 * @note OTAPP_PAIR_RULES_ALLOWED_SIZE = 10 (maximum rules per set)
 * @see pairRulesGetList_all_allowed()
 */
static otapp_pair_rule_t deviceRules_all_allowed = {
    .allowed = {OTAPP_PAIR_NO_RULES, OTAPP_PAIR_END_OF_RULES} // NO RULES, pair every incoming device
};


/**
 * @brief Get pairing rules that allow all devices
 * 
 * @return Pointer to deviceRules_all_allowed rule set
 * 
 * @details
 * Use this callback when you want to pair with any device without filtering.
 * 
 * @see deviceRules_all_allowed
 */
static otapp_pair_rule_t *pairRulesGetList_all_allowed()
{
    return &deviceRules_all_allowed;
}


//////////////////////
// OBSERVER CALLBACKS
//////////////////////
char mqttTopicBuffer[OTAPP_DEVICENAME_SIZE + OTAPP_EUI_STRING_SIZE + OTAPP_URI_MAX_NAME_LENGHT + 1]; 

static int8_t mqttMakeTopic(char *devNameFull, char *uri, char *buffer, uint16_t bufferSize)
{
    if(devNameFull == NULL || uri == NULL || buffer == NULL) return -1;

    int8_t result = 0;
    int8_t devGroupLen = 0;
    uint8_t bufferIndex = 0;
    char *eui = NULL;
    
    memset(buffer, 0, bufferSize); // clear buffer

    devGroupLen = drv->api.devName.getDeviceGroupName(devNameFull, buffer, bufferSize); // extract and add device group name to start of buffer
    if(devGroupLen > 0) // Check if group name is not empty
    {
        OTAPP_PRINTF(TAG, "Group Name: %s, length: %d \n", buffer, devGroupLen);
    }
    else
    {
        OTAPP_PRINTF(TAG, "Failed to get group name from device name full\n");
    }  

    result = drv->api.devName.devNameFullToEUI(devNameFull, strlen(devNameFull), &eui); // extracta and EUI ptr frome device name
    if(result == OTAPP_DEVICENAME_OK && eui != NULL)
    {
        OTAPP_PRINTF(TAG, "EUI: %s \n", eui);
    }
    else
    {
        OTAPP_PRINTF(TAG, "Failed to decode EUI from device name full\n");
    }

    bufferIndex = devGroupLen;                                  // start after group name

    buffer[bufferIndex] = '/';                                  // add separator between group name and EUI, 
    bufferIndex ++;                                             // move pointer after separator

    memcpy(buffer + bufferIndex, eui, OTAPP_EUI_STRING_SIZE);   // copy EUI to topic buffer after group name, it should looks like gropu_name/eui
    bufferIndex += OTAPP_EUI_STRING_SIZE;                       // move pointer after EUI

    buffer[bufferIndex] = '/';                                  // add separator between group name and EUI, we will add URI path later in the loop
    bufferIndex ++;                                             // move pointer after separator

    memcpy(buffer + bufferIndex, uri, strlen(uri));             // Build MQTT topic by appending URI path to the base topic (group/EUI/URI)
    
    return 0;
}
/**
 * @brief Callback invoked when a device is successfully paired
 * 
 * @param[in] device Pointer to paired device information structure
 */
static void pairedCallback(otapp_pair_Device_t *device)
{        
    
    OTAPP_PRINTF(TAG, "Detect DEVICE! %s \n", device->devNameFull);
            
    for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX; i++)
    {
        if(device->urisList[i].uri[0] != '\0') // Check if URI is not empty
        {
            if(mqttMakeTopic(device->devNameFull, device->urisList[i].uri, mqttTopicBuffer, sizeof(mqttTopicBuffer)) != 0)
            {
                return;
            }
         
            if(gw->mqtt.subscribeSingle(mqttTopicBuffer, 1) == 0)
            {
                OTAPP_PRINTF(TAG, "MQTT: Subscribed to URI: %s\n", mqttTopicBuffer);
            }
            else
            {
                OTAPP_PRINTF(TAG, "MQTT: Failed to subscribe to URI: %s\n", mqttTopicBuffer);
            }
        }
    }
    
}

/**
 * @brief Callback invoked when a subscribed URI status is updated
 * 
 * @param[in] data Pointer to URI status update data packet
 * 
 */
static void subscribedUrisCallback(oac_uri_dataPacket_t *data)  
{    
    // Parse 32-bit state value from buffer (little-endian)
    uint32_t uriState_ = 0;
    uriState_ |= (uint32_t)data->buffer[0];
    uriState_ |= ((uint32_t)data->buffer[1] << 8);
    uriState_ |= ((uint32_t)data->buffer[2] << 16);
    uriState_ |= ((uint32_t)data->buffer[3] << 24);

    // Update internal state tracking
    otapp_pair_DeviceList_t *pairHandle = drv->api.pair.getHandle();
    drv->api.pair.uriStateSet(pairHandle, data->token, &uriState_);
    
    // Log status update for debugging
    OTAPP_PRINTF(TAG, " @ message from subs: \n");
    OTAPP_PRINTF(TAG, " @--> token: 0x%x 0x%x 0x%x 0x%x\n", data->token[0], data->token[1], data->token[2], data->token[3]);
    OTAPP_PRINTF(TAG, " @--> data: %ld\n", uriState_);
}

/**
 * @brief Main task function for button device
 * 
 * @details
 * This function should be called periodically from the main loop or RTOS task.
 * It processes button events and triggers appropriate actions.
 * 
 * @note Non-blocking function - returns immediately after processing
 * 
 * @see ad_btn_task() for button processing implementation
 */
static void task()
{
   
    WS2812BFX_Callback(); // FX effects calllback
}

static void tick()
{
    WS2812BFX_SysTickCallback();
}

static void ad_gw_eventPrint(ad_gw_drv_event_t event)
{
    for (uint16_t i = 0; i < AD_GW_EVENT_NAME_SIZE; i++)
    {
        if(event == event_name[i].event)
        {
            OTAPP_PRINTF(TAG, "%s\n", event_name[i].name);
        }
    } 
}

static void ad_gw_mqttParseTask(void *pvParameters) 
{   
    ad_gw_drv_mqtt_data_t rx_msg;
    
    for (;;) 
    {
        if (xQueueReceive(mqtt_queue, &rx_msg, portMAX_DELAY) == pdPASS) 
        {
            OTAPP_PRINTF(TAG,"Topic: %s\n", rx_msg.topic);
            OTAPP_PRINTF(TAG,"Datad:  %s\n", rx_msg.data);            
        }
    }
}

static void ad_gateway_event(ad_gw_drv_event_t event, void *context)
{
    ad_gw_eventPrint(event);

    if (event == AD_GW_DRV_MQTT_DATA)
    {       
        if (xQueueSend(mqtt_queue, context, 0) != pdPASS) 
        {
            OTAPP_PRINTF(TAG,"ERROR: MQTT queue is full \n");
        }
    }

    if(event == AD_GW_DRV_MQTT_CONNECTED)
    {
        if(gw->mqtt.subscribeSingle != NULL) gw->mqtt.subscribeSingle("hro/t1/test", 1);
    }       
}

static void ad_gw_init(ad_gw_drv_t *gateway)
{
    if (gateway == NULL) return;

    gw = gateway;
    gw->init();
    ad_gw_drv_register_event_cb(ad_gateway_event);
    
    mqtt_queue = xQueueCreate(5, sizeof(ad_gw_drv_mqtt_data_t));
   
    xTaskCreate(ad_gw_mqttParseTask, "MQTT_PROC", 4096, NULL, 2, NULL);
}


//////////////////////
// INITIALIZATION
//////////////////////

void ad_gatewayInit(ad_gw_drv_t *gateway)
{    
    // if(deviceNameGroup == NULL) return;
    // strcpy(deviceNameTab, "mqtt_gw");

    // Get singleton driver instance
    drv = ot_app_drv_getInstance();

    // Initialize NVS for persistent storage
    if(drv->api.nvs.init == NULL) return;
    drv->api.nvs.init();

    // Register pairing rules callback (choose one option):
    drv->pairRuleGetList_clb = pairRulesGetList_all_allowed;     // if you want to pair all devices
    // raczej bedziemy chcieli subskrybowac urzadzania koncowe, swiatlo, rolety..

    // Register URI callbacks
    drv->uriGetList_clb = NULL;
    drv->uriGetListSize = 0;

    // Register observer callbacks
    drv->obs_pairedDevice_clb = pairedCallback;
    drv->obs_subscribedUri_clb = subscribedUrisCallback;

    // Set device name gropu. length of it should be max OTAPP_DEVICENAME_SIZE - 1 (9 bytes)
    drv->deviceName = drv->api.devName.getNoGroupNamePtr();
    drv->deviceType = &deviceType;
    drv->task = task;
    drv->tick = tick;
    
    otapp_init();

    ad_gw_init(gateway);

}