/** 
 * @file ot_app_pair.h
 * @brief Manages the list of discovered devices, pairing logic, and URI resource parsing.
 * @details see more information in section: @ref ot_app_pair 
 * 
 * @defgroup ot_app_pair Device Pairing
 * @ingroup ot_app
 * @brief Manages the list of discovered devices, pairing logic, and URI resource parsing.
 * @details
 * @{
 * This module handles the core logic of the "Pairing" phase. It maintains a list of known devices
 * (@ref otapp_pair_DeviceList_t), handles the logic of adding/removing devices based on rules,
 * and parses CoAP responses containing device capabilities (URIs).
 * **Key Features:**
 * - **Device List Management:** Add, remove, and search for devices by name or IP.
 * - **Rule-based Pairing:** Filters incoming devices based on allowed types (defined in driver).
 * - **TLV Parsing:** Decodes Type-Length-Value payloads to extract supported URIs.
 * - **Observer Pattern:** Notifies the application when a new device is successfully paired.
 * 
 * @version 0.1
 * @date 19-08-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */

#ifndef OT_APP_PAIR_H_
#define OT_APP_PAIR_H_

#include "hro_utils.h"
#include "ot_app_coap_uri_obs.h"
#include "ot_app_coap.h"
#include "string.h"

#ifndef UNIT_TEST
    #include "ot_app.h"
#else
    #include "mock_ot_app.h"
#endif

/** @name Comparison Results */
///@{
#define OTAPP_PAIR_IS                       (1)  ///< Objects are identical
#define OTAPP_PAIR_IS_NOT                   (2)  ///< Objects are different
///@}

/** @name Return Codes */
///@{
#define OTAPP_PAIR_OK                       (-1) ///< Operation successful
#define OTAPP_PAIR_UPDATED                  (-2) ///< Device data was updated (e.g. new IP)
#define OTAPP_PAIR_NO_NEED_UPDATE           (-3) ///< Device exists and data is current
#define OTAPP_PAIR_ERROR                    (-4) ///< Generic error
#define OTAPP_PAIR_NO_EXIST                 (-5) ///< Device not found in list
#define OTAPP_PAIR_DEVICE_NAME_EXIST        (-6) ///< Device name collision
#define OTAPP_PAIR_DEVICE_NAME_TO_LONG      (-7) ///< Name exceeds buffer size
#define OTAPP_PAIR_DEVICE_NO_SPACE          (-8) ///< Device list is full
///@}

/** @name Buffer & Limits Configuration */
///@{
#define OTAPP_PAIR_DEVICES_MAX                      OTAPP_PAIRED_DEVICES_MAX ///< Max number of paired devices (from ot_app.h)
#define OTAPP_PAIR_URI_MAX                          OTAPP_PAIRED_URI_MAX     ///< Max number of URIs per device (from ot_app.h)
#define OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE         (OTAPP_URI_MAX_NAME_LENGHT + sizeof(otapp_deviceType_t) + sizeof(uint8_t))
#define OTAPP_PAIR_URI_RESOURCE_BUFFER_MAX_SIZE     (OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE * OTAPP_PAIR_URI_MAX)

#define OTAPP_PAIR_URI_MAX_VAL      OTAPP_URI_END_OF_INDEX
#define OTAPP_PAIR_NAME_FULL_SIZE   OTAPP_DEVICE_NAME_FULL_SIZE 
#define OTAPP_PAIR_NO_URI           OTAPP_URI_NO_URI_INDEX
#define OTAPP_PAIR_URI_INIT         OTAPP_URI_END_OF_INDEX
///@}

/** @name Task & Queue Configuration */
///@{
#define OTAPP_PAIR_QUEUE_LENGTH         10          ///< Depth of the pairing event queue
#define OTAPP_PAIR_TASK_STACK_DEPTH     (128 * 17)  ///< Stack size for the pairing RTOS task
#define OTAPP_PAIR_TASK_PRIORITY        9           ///< Priority of the pairing RTOS task
///@}

/** @name Pairing Rules Configuration */
///@{
#define OTAPP_PAIR_RULES_ALLOWED_SIZE           10
#define OTAPP_PAIR_RULES_ALLOWED_ITEM_MAX_SIZE  OTAPP_END_OF_DEVICE_TYPE

#define OTAPP_PAIR_NO_RULES     (OTAPP_END_OF_DEVICE_TYPE + 1) 
#define OTAPP_PAIR_NO_ALLOWED   OTAPP_NO_DEVICE_TYPE 
#define OTAPP_PAIR_END_OF_RULES   OTAPP_END_OF_DEVICE_TYPE 

#define OTAPP_PAIR_OBSERVER_PAIRE_DDEVICE_CALLBACK_SIZE 10
///@}

/**
 * @brief Structure holding temporary parsed data from a URI resource.
 */
typedef struct {
    char uri[OTAPP_URI_MAX_NAME_LENGHT];    ///< URI path string (e.g. "light/on_off")
    uint32_t devTypeUriFn;                  ///< Functional type of this URI (see @ref otapp_deviceType_t)
    uint8_t obs;                            ///< Flag: 1 if this URI supports Observe, 0 otherwise
} otapp_pair_resUrisParseData_t;

typedef uint8_t otapp_pair_resUrisBuffer_t[OTAPP_PAIR_URI_RESOURCE_BUFFER_SIZE];

/**
 * @brief Represents a single URI endpoint belonging to a paired device.
 */
typedef struct {
    char uri[OTAPP_URI_MAX_NAME_LENGHT];            ///< URI path
    uint32_t uriState;                              ///< Current cached state/value of the URI
    otapp_deviceType_t devTypeUriFn;                ///< Functional type
    oacu_token_t token[OAC_URI_OBS_TOKEN_LENGTH];   ///< Token used for CoAP Observe subscription
} otapp_pair_uris_t;

/**
 * @brief Represents a fully paired remote device.
 */
typedef struct {
    char devNameFull[OTAPP_PAIR_NAME_FULL_SIZE];    ///< Full hostname (e.g. "device1_1_588c...service.arpa")
    otIp6Address ipAddr;                            ///< Mesh-Local EID IPv6 address
    otapp_pair_uris_t urisList[OTAPP_PAIR_URI_MAX]; ///< List of discovered services/URIs on this device
} otapp_pair_Device_t;

/**
 * @brief Opaque handle for the device list.
 * @details The actual definition is hidden in the source file to encapsulate the list storage.
 */
typedef struct otapp_pair_DeviceList_t otapp_pair_DeviceList_t;

/**
 * @brief Rules structure defining which device types are allowed to pair.
 */
typedef struct {
    otapp_deviceType_t allowed[OTAPP_PAIR_RULES_ALLOWED_SIZE]; 
} otapp_pair_rule_t;

/**
 * @brief Queue event types.
 */
typedef enum {
    OTAPP_PAIR_CHECK_AND_ADD_TO_DEV_LIST     ///< Event: Check rules and potentially add device
} otapp_pair_QueueDataType_t;

/**
 * @brief Structure for the pairing event queue.
 */
typedef struct {
    otapp_pair_QueueDataType_t type;
    char deviceNameFull[OTAPP_PAIR_NAME_FULL_SIZE];
    otIp6Address ipAddress;
} otapp_pair_queueItem_t;

/**
 * @brief Initializes the pairing module.
 * @details Creates the RTOS task and queue responsible for processing discovery events
 * and managing the device list. Registers the initial observer callback.
 * @param driver Pointer to the main application driver instance.
 * @return int8_t @ref OTAPP_PAIR_OK on success.
 */
int8_t otapp_pair_init(ot_app_devDrv_t *driver);

/**
 * @brief Prototype for observer callback function.
 * @param[out] newDevice Pointer to the structure of the newly paired device.
 */
typedef void (*otapp_pair_observerCallback_t)(otapp_pair_Device_t *newDevice);

/**
 * @brief Registers a callback to be notified when a new device is paired.
 * @param callback Function pointer to the handler.
 * @return int8_t @ref OTAPP_PAIR_OK on success.
 */
int8_t otapp_pair_observerPairedDeviceRegisterCallback(otapp_pair_observerCallback_t callback);

/**
 * @brief Adds a new device to the pairing list.
 * @param pairDeviceList  [in] Handle to the device list (use @ref otapp_pair_getHandle).
 * @param deviceNameFull  [in] Full device name string.
 * @param ipAddr          [in] Pointer to the device's IPv6 address.
 * @return int8_t         Index of the added device, or error code (@ref OTAPP_PAIR_ERROR, etc.).
 */
int8_t otapp_pair_DeviceAdd(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull, otIp6Address *ipAddr);

/**
 * @brief Retrieves the index of a device in the list by its name.
 * @param pairDeviceList  [in] Handle to the device list.
 * @param deviceNameFull  [in] Full device name string.
 * @return int8_t         Device index (0 to Max-1) or @ref OTAPP_PAIR_NO_EXIST.
 */
int8_t otapp_pair_DeviceIndexGet(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull);

/**
 * @brief Retrieves a pointer to the device structure by name.
 * @param pairDeviceList Handle to the device list.
 * @param deviceNameFull Full device name string.
 * @return otapp_pair_Device_t* Pointer to the device, or NULL if not found.
 */
otapp_pair_Device_t *otapp_pair_DeviceGet(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull);

/**
 * @brief Retrieves the full name of a device at a specific index.
 * @param pairDeviceList Handle to the device list.
 * @param indexDevice    Index of the device.
 * @return char* Pointer to the name string, or NULL on error.
 */
char *otapp_pair_DeviceNameGet(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief Removes a device from the pairing list.
 * @param pairDeviceList Handle to the device list.
 * @param deviceNameFull Full device name string.
 * @return int8_t        Index of the deleted device, or error code.
 */
int8_t otapp_pair_DeviceDelete(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull);

/**
 * @brief Clears all devices from the pairing list.
 * @param pairDeviceList Handle to the device list.
 * @return int8_t @ref OTAPP_PAIR_OK.
 */
int8_t otapp_pair_DeviceDeleteAll(otapp_pair_DeviceList_t *pairDeviceList);

/**
 * @brief Gets the singleton handle to the global device list.
 * @return otapp_pair_DeviceList_t* Pointer to the list structure.
 */
otapp_pair_DeviceList_t *otapp_pair_getHandle(void);

/**
 * @brief Retrieves the IPv6 address of a device at a specific index.
 * @param pairDeviceList Handle to the device list.
 * @param indexDevice    Index of the device.
 * @return otIp6Address* Pointer to the IP address, or NULL on error.
 */
otIp6Address *otapp_pair_ipAddressGet(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief Checks if the stored IP address matches the provided one.
 * @param pairDeviceList Handle to the device list.
 * @param indexDevice    Index of the device.
 * @param ipAddr         Pointer to the IPv6 address to compare.
 * @return int8_t        @ref OTAPP_PAIR_IS (match), @ref OTAPP_PAIR_IS_NOT (different).
 */
int8_t otapp_pair_ipAddressIsSame(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice, otIp6Address *ipAddr);

/**
 * @brief Updates the IP address of a paired device if it has changed.
 * @param pairDeviceList Handle to the device list.
 * @param indexDevice    Index of the device.
 * @param ipAddrNew      Pointer to the new IPv6 address.
 * @return int8_t        @ref OTAPP_PAIR_UPDATED if changed, @ref OTAPP_PAIR_NO_NEED_UPDATE if same.
 */
int8_t otapp_pair_ipAddressUpdate(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice, otIp6Address *ipAddrNew);

/**
 * @brief Prints debug information about a paired device to the log.
 * @param pairDeviceList Handle to the device list.
 * @param indexDevice    Index of the device to print.
 */
void otapp_pair_devicePrintData(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief Adds an item to the pairing processing queue.
 * @param queueItem Pointer to the item to add.
 * @return int8_t @ref OTAPP_PAIR_OK on success.
 */
int8_t otapp_pair_addToQueue(otapp_pair_queueItem_t *queueItem);

/**
 * @brief Parses the payload of a CoAP response containing URI resources.
 * @details Extracts TLV data from the buffer and populates an array of @ref otapp_pair_resUrisParseData_t.
 * @param[in]  inBuffer         Pointer to the raw TLV buffer.
 * @param[in]  inBufferSize     Size of the buffer.
 * @param[out] result           Operation result (@ref OTAPP_PAIR_OK / ERROR).
 * @param[out] outParsedDataSize Number of URI items found.
 * @return otapp_pair_resUrisParseData_t* Pointer to the start of the parsed structures (inside the buffer).
 */
otapp_pair_resUrisParseData_t *otapp_pair_uriParseMessage(uint8_t *inBuffer, uint16_t inBufferSize, int8_t *result, uint16_t *outParsedDataSize);

/**
 * @brief Calculates the required buffer size to hold a message with URI resources.
 * @param aMessagePayloadSize Size of the raw payload.
 * @return uint16_t Total bytes needed.
 */
uint16_t otapp_pair_uriParseMessageCalculateBufSize(uint16_t aMessagePayloadSize);

/**
 * @brief Adds a parsed URI to a specific slot in a device's URI list.
 * @param deviceUrisList Pointer to the destination slot in the device structure.
 * @param uriData        Pointer to the parsed source data.
 * @param token          Optional CoAP token if subscription is active.
 * @return int8_t        @ref OTAPP_PAIR_OK on success.
 */
int8_t otapp_pair_uriAdd(otapp_pair_uris_t *deviceUrisList, const otapp_pair_resUrisParseData_t *uriData, const oacu_token_t *token);

/**
 * @brief Serializes a list of device URI resources into a TLV-encoded byte buffer.
 * @details This function takes an array of CoAP resources and packs them into a 
 * user-provided buffer using the @ref ot_app_msg_tlv module. It is primarily used 
 * during the device discovery and pairing phase to inform other nodes about 
 * available endpoints.
 * **TLV Memory Layout:**
 * ```text
 * ┌───────────────────────────────────────────────────────────────────────────────┐
 * │                             TLV MESSAGE BUFFER                                │
 * ├─────────────┬──────┬─────┬─────┬──────┬─────┬───────┬──────┬──────┬───────┬───│
 * │ 2B          │Key0  │Len0 │Val0 │Key1  │Len1 │Val1   │Key2  │Len2  │Val2   │   │
 * │writtenBytes │0xAA00│1B   │URIs │0xAA01│ 4B  │devType│0xAA02│strlen│UriPath│...│
 * └─────────────┴──────┴─────┴─────┴──────┴─────┴───────┴──────┴──────┴───────┴───┘
 * Key 0xAA00: uriSize (uint8_t)
 * Key 0xAA01: uri_1.devType (uint32_t, 4B)  
 * Key 0xAA02: uri_1.mUriPath (char[], variable)
 * Key 0xAA03: uri_2.devType (uint32_t, 4B)[1]
 * Key 0xAA04: uri_2.mUriPath (char[], variable)[1]
 * Key 0xAA05: uri_3.devType (uint32_t, 4B)[2]
 * Key 0xAA06: uri_3.mUriPath (char[], variable)[2]
 * ...
 * ```
 * **Encoding Logic:**
 * - Base key: `0xAA00` 
 * - Key `0xAA00`: Total number of URIs (`uriSize`)
 * - For each URI `i` (0 to `uriSize-1`):
 * - Key `0xAA00 + 2*i + 1`: `uri[i].devType` (4 bytes)
 * - Key `0xAA00 + 2*i + 2`: `uri[i].resource.mUriPath` (string length)
 * @param[in] uri               Pointer to the array of URI resource structures.
 * @param[in] uriSize           Number of URIs to serialize (max @ref OTAPP_PAIR_URI_MAX).
 * @param[out] bufferOut        Pointer to the destination buffer (usually acquired via @ref otapp_buf_getWriteOnly_ptr).
 * @param[in,out] bufferSizeInOut [in] Total capacity of the buffer; [out] Actual number of bytes written.
 * @return int8_t 
 * - @ref OTAPP_PAIR_OK on success.
 * - @ref OTAPP_PAIR_ERROR if parameters are invalid or buffer space is insufficient.
 * @note This function automatically updates @p bufferSizeInOut by calling 
 * @ref otapp_msg_tlv_getBufferTotalUsedSpace at the end of the process.
 */
int8_t otapp_pair_uriResourcesCreate(otapp_coap_uri_t *uri, uint8_t uriSize, uint8_t *bufferOut, uint16_t *bufferSizeInOut);

/**
 * @brief Calculates the buffer size needed to serialize a list of URIs.
 * @param uri     Pointer to array of URIs.
 * @param uriSize Number of URIs.
 * @return uint16_t Size in bytes.
 */
uint16_t otapp_pair_uriResourcesCalculateBufSize(otapp_coap_uri_t *uri, uint8_t uriSize);

/**
 * @brief Sets the state (value) of a URI based on its token.
 * @param pairDeviceList    [in] Handle to Pair devices list.
 * @param token             [in] Token of the URI (from Observe notification).
 * @param uriState          [in] New state value (max uint32_t).
 * @return int8_t           @ref OTAPP_PAIR_OK or @ref OTAPP_PAIR_ERROR.
 */ 
int8_t otapp_pair_uriStateSet(otapp_pair_DeviceList_t *pairDeviceList, const oacu_token_t *token, const uint32_t *uriState);

/**
 * @brief Resends subscription updates to all paired devices (e.g. after IP change).
 * @param pairDeviceList Handle to the device list.
 * @return int8_t Count of updated devices or error code.
 */
int8_t otapp_pair_subSendUpdateIP(otapp_pair_DeviceList_t *pairDeviceList);

/**
 * @brief Finds a URI item in the list by its token.
 * @param pairDeviceList Handle to the device list.
 * @param token          Token to search for.
 * @return otapp_pair_uris_t* Pointer to the URI item or NULL.
 */
otapp_pair_uris_t *otapp_pair_tokenGetUriIteams(otapp_pair_DeviceList_t *pairDeviceList, const oacu_token_t *token);

/**
 * @brief Searches for a specific device function type within a device's URI list.
 * @param deviceHandle  [in] Pointer to the device structure.
 * @param uriDevType    [in] Function type to look for (e.g. LIGHTING_ON_OFF).
 * @return int8_t       Index in urisList or @ref OTAPP_PAIR_NO_EXIST.
 */
int8_t otapp_pair_uriGetIdList(otapp_pair_Device_t *deviceHandle, otapp_deviceType_t uriDevType);

#ifdef UNIT_TEST

/**
 * @brief check free space of pairDeviceList->list[]
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @return int8_t         [out] free table index of pairDeviceList->list[tableIndex] 
 * or  OTAPP_PAIR_ERROR (-1)
 */
PRIVATE int8_t otapp_pair_DeviceIsFreeSpace(otapp_pair_DeviceList_t *pairDeviceList);

/**
 * @brief reserve space in pairing device list 
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param indexDevice     [in] index of free position of pairDeviceList->list[tableIndex] 
 * @note to get free position of device list you should use otapp_pair_DeviceIsFreeSpace()
 */
PRIVATE void otapp_pair_spaceTake(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief check if space in paring device list is taken
 * @param pairDeviceList [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param indexDevice    [in] index of device name to check
 * @return int8_t        [out] TRUE = 1, FALSE = 0
 * or OTAPP_PAIR_ERROR
 */
PRIVATE int8_t otapp_pair_spaceIsTaken(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice);

/**
 * @brief check if deviceNameFull is existing on pairing device list
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle()
 * @param deviceNameFull  [in] char ptr of full device name ("device1_1_588c81fffe301ea4")
 * @return int8_t         [out] index of device name found in table list
 * or  OTAPP_PAIR_ERROR (-1)
 */
PRIVATE int8_t otapp_pair_DeviceIsExist(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull);

/**
 * @brief compare two deviceNameFull
 * @param pairDeviceList  [in] handle ptr of otapp_pair_DeviceList_t. Use: otapp_pair_getHandle() 
 * @param deviceNameFull  [in] char ptr of full device name ("device1_1_588c81fffe301ea4") 
 * @param indexDevice     [in] device index to comparison 
 * @return int8_t         [out] TRUE = OTAPP_PAIR_IS, FALSE = OTAPP_PAIR_IS_NOT, 
 * or OTAPP_PAIR_ERROR (-1) if error
 */
PRIVATE int8_t otapp_pair_deviceNameIsSame(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull, uint8_t indexDevice);

/**
 * @brief todo
 * @param pairDeviceList 
 * @param queueIteam 
 * @return PRIVATE 
 */
PRIVATE int8_t otapp_pair_deviceIsMatchingFromQueue(otapp_pair_queueItem_t *queueIteam);

/**
 * @brief todo
 * @return PRIVATE 
 */
PRIVATE int8_t otapp_pair_initQueue(void);

/**
 * @brief todo
 * @return PRIVATE 
 */
PRIVATE int8_t otapp_pair_initTask(void);

/**
 * @brief todo
 * @param errorState 
 * @param deviceNameFull 
 * @return PRIVATE 
 */
PRIVATE int8_t otapp_pair_observerPairedDeviceNotify(otapp_pair_Device_t *newDevice);

/**
 * @brief todo
 * @param deviceDrv 
 * @param mainDeviceID 
 * @param incommingDeviceID 
 * @return int8_t 
 */
PRIVATE int8_t otapp_pair_deviceIsAllowed(ot_app_devDrv_t *deviceDrv, otapp_deviceType_t incommingDeviceID);

PRIVATE int8_t otapp_pair_tokenIsSame(otapp_pair_DeviceList_t *pairDeviceList, int8_t devListId, int8_t uriListId, const oacu_token_t *tokenToCheck);


#endif  /* UNIT_TEST */

#endif  /* OT_APP_PAIR_H_ */

/**
 * @}
 */