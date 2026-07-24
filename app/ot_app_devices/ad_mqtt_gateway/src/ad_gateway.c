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
/**
 * 
 * @brief MQTT Topic Architecture based on Sparkplug B standard.
 *
 * @details This component manages bidirectional MQTT communication over GSM,
 * routing downstream commands to the OpenThread network and pushing upstream
 * telemetry/lifecycle states up to the cloud.
 * * To eliminate packet echo and optimize GSM bandwidth, both the Border Router
 * and the Controlling Devices use specific asymmetrical subscription matrices.
 *
 * ### Sparkplug B Topic Scheme:
 * @code
 * spBv1.0 / [Group_ID] / [Message_Type] / [Edge_Node_ID] / [Device_ID]
 * @endcode
 *
 * ### Mapping for OpenThread Network:
 * - [Group_ID]     : Variable location (e.g., device1, room1, room2), maks size OTAPP_DEVICENAME_SIZE
 * - [Edge_Node_ID] : Shortened Border Router ID (br_gsm), const size 
 * - [Device_ID]    : Dynamic thread node resource ([EUI]/[Type]/[Param], e.g., 588c81fffe3035a4/light/on_off), maks size OTAPP_EUI_STRING_SIZE + OTAPP_URI_MAX_NAME_LENGHT
 *
 * ### MQTT Traffic, Subscription & Action Matrix:
 * @code
 * +--------------+------+----------+----------------------------------------+-----------+-----------------------------------------+
 * | DEVICE TYPE  | I/O  | MSG TYPE | MQTT TOPIC PATTERN                     | ACTION    | PURPOSE / DESCRIPTION                   |
 * +==============+======+==========+========================================+===========+=========================================+
 * |              | IN   | NCMD     | spBv1.0/+/NCMD/br_gsm                  | SUBSCRIBE | Direct action on BR (Reboot, GSM cfg).  |
 * | Border       | IN   | DCMD     | spBv1.0/+/DCMD/br_gsm/#                | SUBSCRIBE | Forward payload to OpenThread node.     |
 * | Router       |------+----------+----------------------------------------+-----------+-----------------------------------------+
 * | (br_gsm)     | OUT  | NDATA    | spBv1.0/[Room_ID]/NDATA/br_gsm         | PUBLISH   | Pushes BR diagnostics (RSSI, RAM, etc). |
 * |              | OUT  | DDATA    | spBv1.0/[Room_ID]/DDATA/br_gsm/#       | PUBLISH   | Pushes actual state from Thread node.   |
 * +--------------+------+----------+----------------------------------------+-----------+-----------------------------------------+
 * |              | IN   | BIRTH   | spBv1.0/+/BIRTH/#                       | SUBSCRIBE | Detects BR online and new Thread nodes. |
 * | Controlling  | IN   | DEATH   | spBv1.0/+/DEATH/#                       | SUBSCRIBE | Detects BR offline (LWT) and lost nodes.|
 * | Device       | IN   | DATA    | spBv1.0/+/DATA/#                        | SUBSCRIBE | Receives all live telemetry and states. |
 * | (PC/App/LCD) |------+---------+-----------------------------------------+-----------+-----------------------------------------+
 * |              | OUT  |N/DCMD |spBv1.0/[Room_ID]/[NCMD_or_DCMD]/br_gsm/...| PUBLISH  | Dispatches control commands downstream. |
 * +--------------+------+----------+----------------------------------------+-----------+-----------------------------------------+
 *  controlling example topic "spBv1.0/device1/DCMD/br_gsm/588c81fffe3035a4/light/on_off"
 * 
 *  @endcode
 *
 * @note [Border Router] Subscribing only to CMD branches prevents receiving an echo of its own data payloads.
 * @note [Controlling Device] Subscribing to +BIRTH/DEATH/DATA text-matching wildcards filters out downstream 
 * command echoes, avoiding UI feedback loops without extra code overhead.
 */

#define MQTT_TOPIC_SUBSCRIBE_NCMD       "spBv1.0/+/NCMD/br_gsm"
#define MQTT_TOPIC_SUBSCRIBE_SIZE_NCMD  (sizeof(MQTT_TOPIC_SUBSCRIBE_NCMD) - 1)

#define MQTT_TOPIC_SUBSCRIBE_DCMD       "spBv1.0/+/DCMD/br_gsm/#"
#define MQTT_TOPIC_SUBSCRIBE_SIZE_DCMD  (sizeof(MQTT_TOPIC_SUBSCRIBE_DCMD) - 1)


#define MQTT_TOPIC_PREFIX                     "spBv1.0/"
#define MQTT_TOPIC_PREFIX_SIZE                (sizeof(MQTT_TOPIC_PREFIX) - 1)

#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_NCMD   "/NCMD/"  // cmd to br_gsm
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_DCMD   "/DCMD/"  // cmd to devices in openthread
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_NDATA  "/NDATA/" // NODE eg br_gsm
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_DDATA  "/DDATA/" // devices eg light
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_BIRTH  "/BIRTH/" // devices eg light when it has been turned on 
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_DEATH  "/DEATH/" // devices eg light when it has been turned on 

#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_CMD     (sizeof(MQTT_TOPIC_MESSAGE_TYPE_PREFIX_DCMD) - 1)
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_MAX_SIZE     (sizeof(MQTT_TOPIC_MESSAGE_TYPE_PREFIX_DDATA) - 1)
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_DATA    MQTT_TOPIC_MESSAGE_TYPE_PREFIX_MAX_SIZE
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_BIRTH   MQTT_TOPIC_MESSAGE_TYPE_PREFIX_MAX_SIZE
#define MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_DEATH   MQTT_TOPIC_MESSAGE_TYPE_PREFIX_MAX_SIZE

#define MQTT_TOPIC_EDGE_NODE_ID_PREFIX        "br_gsm/"
#define MQTT_TOPIC_EDGE_NODE_ID_PREFIX_SIZE   (sizeof(MQTT_TOPIC_EDGE_NODE_ID_PREFIX) - 1)
#define MQTT_TOPIC_URI_SEPARATOR_SIZE         1
#define MQTT_TOPIC_TERMINATOR_SIZE            1
#define MQTT_TOTAL_BUFFER_SIZE ( \
    MQTT_TOPIC_PREFIX_SIZE            + \
    OTAPP_DEVICENAME_SIZE             + \
    MQTT_TOPIC_MESSAGE_TYPE_PREFIX_MAX_SIZE + \
    MQTT_TOPIC_EDGE_NODE_ID_PREFIX_SIZE + \
    OTAPP_EUI_STRING_SIZE             + \
    MQTT_TOPIC_URI_SEPARATOR_SIZE     + \
    OTAPP_URI_MAX_NAME_LENGHT         + \
    MQTT_TOPIC_TERMINATOR_SIZE          \
)

typedef enum{
    // for MQTT received topic
    NCMD = 0,   ///> command to NODE rg br_gsm
    DCMD,       ///> command to devices in openthread

    // for MQTT publish topic
    NDATA, ///> NODE eg br_gsm
    DDATA, ///> devices eg light
    BIRTH, ///> devices and NODE eg light, br_gsm when it has been turned on
    DEATH  ///> devices and NODE eg light, br_gsm when it has been turned off
}mqttMessageType_t;

/**
 * @brief MQTT topic buffer for Sparkplug B format
 * 
 * @details Buffer size calculation for topic:
 * spBv1.0 / [Group_ID] / DDATA / br_gsm / [EUI] / [URI]
 */
static char mqttTopicBuffer[MQTT_TOTAL_BUFFER_SIZE]; 

/**
 * @brief Build MQTT topic following Sparkplug B standard
 * 
 * @details
 * Constructs topic in format: spBv1.0/[Group_ID]/DDATA/br_gsm/[EUI]/[URI]
 * 
 * This function is used for publishing device state and subscribing to device updates.
 * The DDATA (Device Data) message type is used for both operations.
 * 
 * @param[in] devNameFull Full device name to extract Group_ID and EUI from
 * @param[in] uri Resource URI path (e.g., "light/on_off")
 * @param[out] buffer Output buffer where topic string will be written
 * @param[in] bufferSize Maximum size of output buffer
 * 
 * @return 0 on success, -1 on failure (invalid input or insufficient buffer size)
 * 
 * @example
 * Topic built: "spBv1.0/device1/DDATA/br_gsm/588c81fffe3035a4/light/on_off"
 * where device1 = Group_ID, 588c81fffe3035a4 = EUI, light/on_off = URI
 */

 
// todo przemyslec bo DEATH BIRTH nie maja uri wiec uri moze byc null !!
static int8_t mqttMakeTopicPublish(char *devNameFull, char *uri, char *buffer, uint16_t bufferSize, mqttMessageType_t messageType)
{
    if(devNameFull == NULL || uri == NULL || buffer == NULL) return -1;

    int8_t result = 0;
    int8_t devGroupLen = 0;
    uint16_t bufferIndex = 0;
    char *eui = NULL;
    uint16_t uriLen = 0;
    
    memset(buffer, 0, bufferSize); // clear buffer

    // Validate buffer size is sufficient
    uriLen = strlen(uri);
    if(bufferSize < (
            MQTT_TOPIC_PREFIX_SIZE +
            OTAPP_DEVICENAME_SIZE +
            MQTT_TOPIC_MESSAGE_TYPE_PREFIX_MAX_SIZE +
            MQTT_TOPIC_EDGE_NODE_ID_PREFIX_SIZE +
            OTAPP_EUI_STRING_SIZE +
            MQTT_TOPIC_URI_SEPARATOR_SIZE +
            uriLen +
            MQTT_TOPIC_TERMINATOR_SIZE)) {
        OTAPP_PRINTF(TAG, "Error: Buffer too small for topic\n");
        return -1;
    }

    // Start with "spBv1.0/"
    memcpy(buffer, MQTT_TOPIC_PREFIX, MQTT_TOPIC_PREFIX_SIZE);
    bufferIndex = MQTT_TOPIC_PREFIX_SIZE;

    // Extract and add device group name
    devGroupLen = drv->api.devName.getDeviceGroupName(devNameFull, buffer + bufferIndex, bufferSize - bufferIndex);
    if(devGroupLen > 0) // Check if group name is not empty
    {
        OTAPP_PRINTF(TAG, "Group Name: %s, length: %d\n", buffer + bufferIndex, devGroupLen);
        bufferIndex += devGroupLen;
    }
    else
    {
        OTAPP_PRINTF(TAG, "Error: Failed to get group name from device name full\n");
        return -1;
    }

    switch (messageType)
    {
        case NDATA:
            memcpy(buffer + bufferIndex, MQTT_TOPIC_MESSAGE_TYPE_PREFIX_NDATA, MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_DATA);
            break;

        case DDATA:
            memcpy(buffer + bufferIndex, MQTT_TOPIC_MESSAGE_TYPE_PREFIX_DDATA, MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_DATA);
            break;

        case BIRTH:
            memcpy(buffer + bufferIndex, MQTT_TOPIC_MESSAGE_TYPE_PREFIX_BIRTH, MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_BIRTH);
            break;

        case DEATH:
            memcpy(buffer + bufferIndex, MQTT_TOPIC_MESSAGE_TYPE_PREFIX_DEATH, MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_DEATH);
            break;

        default:
            return -1;
    }

    bufferIndex += MQTT_TOPIC_MESSAGE_TYPE_PREFIX_SIZE_DATA;
    
    // Add "br_gsm/"
    memcpy(buffer + bufferIndex, MQTT_TOPIC_EDGE_NODE_ID_PREFIX, MQTT_TOPIC_EDGE_NODE_ID_PREFIX_SIZE);
    bufferIndex += MQTT_TOPIC_EDGE_NODE_ID_PREFIX_SIZE;

    // Extract and add EUI
    result = drv->api.devName.devNameFullToEUI(devNameFull, strlen(devNameFull), &eui);
    if(result == OTAPP_DEVICENAME_OK && eui != NULL)
    {
        OTAPP_PRINTF(TAG, "EUI: %s\n", eui);
        memcpy(buffer + bufferIndex, eui, OTAPP_EUI_STRING_SIZE);
        bufferIndex += OTAPP_EUI_STRING_SIZE;
    }
    else
    {
        OTAPP_PRINTF(TAG, "Error: Failed to decode EUI from device name full\n");
        return -1;
    }

    // Add "/"
    buffer[bufferIndex] = '/';
    bufferIndex++;

    // Add URI path (e.g., "light/on_off")
    memcpy(buffer + bufferIndex, uri, uriLen);
    bufferIndex += uriLen;

    // Null terminate
    buffer[bufferIndex] = '\0';
    
    OTAPP_PRINTF(TAG, "Topic: %s\n", buffer);
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
   
    // send update state by MQTT
   
    char *devNameFull = NULL;
    devNameFull = drv->api.pair.tokenGetDevNameFull(pairHandle, data->token);                   // get devNameFull ptr using token

    otapp_pair_uris_t *uriItems = drv->api.pair.tokenGetUriIteams(pairHandle, data->token);     // get URI items for this token
    if(uriItems == NULL) 
    {
        OTAPP_PRINTF(TAG, "ERROR NULL uriItems \n");
        return; 
    }
        
    if(mqttMakeTopicPublish(devNameFull, uriItems->uri, mqttTopicBuffer, sizeof(mqttTopicBuffer), DDATA) != 0)  // build topic. it should looks like: device1/588c81fffe3035a4/light/on_off
    {
        return;
    }

    if(gw->mqtt.publish(mqttTopicBuffer, (uint8_t*)&uriItems->uriState, sizeof(uriItems->uriState), 1, 1) == 0) // publish to MQTT broker // trzeba to zmienic dane w buforze powinny byc wysylane TLV, a nie surowy stan, TREBA TO ZMIENIC w framework. todo
    {
        OTAPP_PRINTF(TAG, "MQTT: Published to URI: %s, DATA: %ld\n", mqttTopicBuffer, uriItems->uriState);
    }
    else
    {
        OTAPP_PRINTF(TAG, "MQTT: Failed to publish to URI: %s\n", mqttTopicBuffer);
    }

    
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
            // WYSYLAMY DANE DO SIECI OPENTHREAD. napisac parser 
            // spBv1.0 / [Group_ID] / [Message_Type] / [Edge_Node_ID] / [Device_ID]     / URI
            // spBv1.0 / device1    / DCMD           / br_gsm         /588c81fffe3035a4 / light/on_off   // dla urzadzen w openthread
            // spBv1.0 / device1    / DCMD           / br_gsm         /588c81fffe3035a4 /                // dla border routhera gsm tego urzadzenia
            
            OTAPP_PRINTF(TAG,"Topic: %s\n", rx_msg.topic);
            OTAPP_PRINTF(TAG,"Data:  %s\n", rx_msg.data);
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
        if(gw->mqtt.subscribeSingle != NULL)
        {
            if(gw->mqtt.subscribeSingle(MQTT_TOPIC_SUBSCRIBE_NCMD, 1) == 0)
            {
                OTAPP_PRINTF(TAG, "MQTT: Subscribed: %s\n", MQTT_TOPIC_SUBSCRIBE_NCMD);
            }
            else
            {
                OTAPP_PRINTF(TAG, "MQTT: ERROR NOT subscribed: %s\n", MQTT_TOPIC_SUBSCRIBE_NCMD);
            }
    
            if(gw->mqtt.subscribeSingle(MQTT_TOPIC_SUBSCRIBE_DCMD, 1) == 0)
            {
                OTAPP_PRINTF(TAG, "MQTT: Subscribed: %s\n", MQTT_TOPIC_SUBSCRIBE_DCMD);
            }
            else
            {
                OTAPP_PRINTF(TAG, "MQTT: ERROR NOT subscribed: %s\n", MQTT_TOPIC_SUBSCRIBE_DCMD);
            }
        } 
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