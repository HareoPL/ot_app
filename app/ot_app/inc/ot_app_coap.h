/**
 * @file ot_app_coap.h
 * @brief Core CoAP infrastructure for request handling, response generation, and resource management.
 * @details see more information in section: @ref ot_app_coap
 * 
 * @defgroup ot_app_coap CoAP Server Core
 * @ingroup ot_app
 * @brief Core CoAP infrastructure for request handling, response generation, and resource management.
 * @details
 * @{
 * This module wraps the OpenThread CoAP API to provide a simplified interface for application developers.
 * It handles the low-level details of message buffer management, header parsing, and response code generation.
 * **Key Functionalities:**
 * - **Initialization:** Starts the CoAP service on the default UDP port.
 * - **Resource Management:** Registers URI paths and their callback handlers.
 * - **Response Helpers:** Provides ready-to-use functions for sending ACK (`2.04 Changed`), ERROR (`4.04 Not Found`), or Data payloads.
 * - **Payload Extraction:** Safely copies data from OpenThread messages (Packet Buffers) to flat C buffers.
 * 
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @version 0.1
 * @date 21-07-2025
 * @copyright © 2025 MIT @ref prj_license 
 */
#ifndef OT_APP_COAP_H_
#define OT_APP_COAP_H_

#include "hro_utils.h"

#ifdef UNIT_TEST
    #include "mock_ip6.h"
#else
    #include "openthread/dataset.h"
    #include "openthread/instance.h"

    #include "openthread/ip6.h"
    #include <openthread/message.h>
    #include <openthread/coap.h>
#endif

#define OTAPP_COAP_PORT 5683 ///< Default CoAP port, as specified in RFC 7252

#define OTAPP_COAP_URI_OK       (-1)
#define OTAPP_COAP_URI_ERROR    (-2)

#define OTAPP_COAP_OK       OTAPP_COAP_URI_OK
#define OTAPP_COAP_ERROR    OTAPP_COAP_URI_ERROR
#define OTAPP_COAP_OK_OBSERVER_REQUEST (-3)

typedef struct ot_app_devDrv_t ot_app_devDrv_t; // forward declaration

#ifndef UNIT_TEST
    typedef enum {
        OTAPP_URI_NO_URI_INDEX = 0,
        
        OTAPP_URI_WELL_KNOWN_CORE,
        OTAPP_URI_PARING_SERVICES,
        OTAPP_URI_SUBSCRIBED_URIS,
        
        OTAPP_URI_TEST,
        OTAPP_URI_TEST_LED,

        OTAPP_URI_END_OF_INDEX,
    }otapp_coap_uriIndex_t;
#endif

typedef struct otapp_coap_uri_t{
    uint32_t            devType; // otapp_deviceType_t
    otCoapResource      resource;
}otapp_coap_uri_t;

typedef enum {
    OTAPP_MESSAGE_OK = 0,
    OTAPP_MESSAGE_ERROR,
    OTAPP_MESSAGE_TEST,
}otapp_coap_messageId_t;

/**
 * @brief Initializes the CoAP service.
 * @details Starts the CoAP engine on the default UDP port and sets up the internal driver reference.
 * @param devDriver Pointer to the main device driver instance.
 * @return int8_t     @ref OTAPP_COAP_OK on success.
 */
int8_t otapp_coap_init(ot_app_devDrv_t *devDriver);

/**
 * @brief Sends a generic CoAP response with a payload.
 * @param request           [in] The original request message (used to match Token/Message ID).
 * @param messageInfo       [in] Source address and port of the requester.
 * @param payload           [in] Pointer to the data buffer to send.
 * @param payload_len       [in] Length of the payload data.
 * @param coapResponseCode  [in] CoAP status code (e.g., OT_COAP_CODE_CONTENT).
 * @param coapType          [in] Message type (e.g., OT_COAP_TYPE_ACKNOWLEDGMENT).
 */
void otapp_coap_sendResponse(otMessage *requestMessage, const otMessageInfo *aMessageInfo, const uint8_t *responseContent, uint16_t responseLength);

/**
 * @brief Sends a simple "OK" response (2.04 Changed).
 * @details Typically used to acknowledge PUT/POST requests that don't require returning data.
 * @param request       [in] The original request message.
 * @param aMessageInfo  [in] Source address and port.
 */
void otapp_coap_sendResponseOK(otMessage *aMessage, const otMessageInfo *aMessageInfo);

/**
 * @brief Sends a "Not Found" error response (4.04).
 * @details Used when the requested URI or resource does not exist.
 * @param request       [in] The original request message.
 * @param aMessageInfo  [in] Source address and port.
 */
void otapp_coap_sendResponseERROR(otMessage *aMessage, const otMessageInfo *aMessageInfo);

/**
 * @brief Prints the sender's IP address from message metadata to the log.
 * @param aMessageInfo [in] Pointer to the message info structure.
 */
void otapp_coap_printSenderIP(const otMessageInfo *aMessageInfo);

/**
 * @brief Converts an internal message ID/Type to a string representation.
 * @param msgID [in] Internal message identifier.
 * @return const char* String literal (e.g., "GET", "PUT", "ACK").
 */
const char *otapp_coap_getMessage(otapp_coap_messageId_t msgID);

/**
 * @brief Sends a test PUT message (Debug function).
 * @details Hardcoded test routine for verifying transmission logic during development.
 */
void otapp_coapSendtoTestPut();

/**
 * @brief Sends a test GET message (Debug function).
 * @details Hardcoded test routine for verifying transmission logic during development.
 */
void otapp_coapSendtoTestGet();

/**
 * @brief Sends a request to update the device name on the network.
 * @details Constructs a CoAP PUT message containing the device's current name
 * and sends it to the default service locator or parent.
 * @return int8_t @ref OTAPP_COAP_OK on success.
 */
void otapp_coapSendDeviceNamePut();

/**
 * @brief Sends a GET request to the standard discovery URI `.well-known/core`.
 * @details This function is used for Service Discovery. It queries a remote device 
 * to list its available resources (URIs). The response is handled by the provided callback.
 * @param ipAddr          [in] IPv6 address of the target device.
 * @param responseHandler [in] Callback function to handle the list of resources returned.
 * @param aContext        [in] User context to pass to the handler.
 */
void otapp_coapSendGetUri_Well_known(const otIp6Address *ipAddr, otCoapResponseHandler responseHandler, void *aContext);

/**
 * @brief Sends a PUT request to update a resource on a remote device.
 * @details Sends a Confirmable PUT message with a payload. 
 * Use this to control remote devices (e.g., turn on a light).
 * @param ipAddr    [in] IPv6 address of the target device.
 * @param data        [in] Pointer to the payload data (e.g., new state).
 * @param dataSize    [in] Size of the payload.
 */
void otapp_coapSendPutUri_subscribed_uris(const otIp6Address *ipAddr, const uint8_t *data, uint16_t dataSize);

/**
 * @brief Initiates a CoAP Observe subscription (RFC 7641).
 * @details Sends a GET request with the Observe option set to 0 (Register). 
 * This tells the server to send notifications whenever the resource state changes.
 * @param ipAddr    [in] IPv6 address of the target device.
 * @param aUriPath    [in] URI path string to subscribe to (e.g., "light/on_off").
 * @param tokenOut    [out] Buffer (min 8 bytes) where the generated subscription Token will be saved.
 */
void otapp_coapSendSubscribeRequest(const otIp6Address *ipAddr, const char *aUriPath, uint8_t *tokenOut);

/**
 * @brief Refreshes or updates an existing subscription.
 * @details Similar to `otapp_coapSendSubscribeRequest`, but uses an existing Token 
 * instead of generating a new one. Useful for re-confirming interest without resetting state.
 * @param ipAddr    [in] IPv6 address of the target device.
 * @param aUriPath    [in] URI path string.
 * @param tokenIn     [in] Existing token to reuse for this request.
 */
void otapp_coapSendSubscribeRequestUpdate(const otIp6Address *ipAddr, const char *aUriPath, uint8_t *tokenIn);

/**
 * @brief send coap message bytes using the PUT method
 * 
 * @param peer_addr         [in] ptr to device IPv6 address
 * @param aUriPath          [in] string ptr to uri
 * @param payloadMsg        [in] ptr to payload data
 * @param payloadMsgSize    [in] payload size
 * @param responseHandler   [in] callback to response handler
 * @param aContext          [in] content will be provided with responseHandler
 */
void otapp_coap_clientSendPutByte(const otIp6Address *peer_addr, const char *aUriPath, const uint8_t *payloadMsg, const uint16_t payloadMsgSize, otCoapResponseHandler responseHandler, void *aContext);

/**
 * @brief send coap request using the GET method. Response will contain bytes
 * 
 * @param peer_addr         [in] ptr to device IPv6 address
 * @param aUriPath          [in] string ptr to uri
 * @param responseHandler   [in] callback to response handler
 * @param aContext          [in] content will be provided with responseHandler
 */
void otapp_coap_clientSendGetByte(const otIp6Address *peer_addr, const char *aUriPath, otCoapResponseHandler responseHandler, void *aContext);

/**
 * @brief Retrieves the string name of a default URI by its index.
 * @details Helper to map integer IDs (e.g., from config) to actual URI strings.
 * @param uriIndex [in] Index of the URI.
 * @return const char* URI path string (e.g., "light/dimm") or NULL if invalid.
 */
const char *otapp_coap_getUriNameFromDefault(otapp_coap_uriIndex_t uriIndex);

/**
 * @brief Extracts the payload from an OpenThread CoAP message.
 * @details Copies bytes from the opaque `otMessage` structure into a linear buffer.
 * @param[in]  aMessage       Pointer to the incoming message.
 * @param[out] bufferOut      Destination buffer.
 * @param[in]  bufferSize     Max size of the destination buffer.
 * @param[out] readBytesOut   Pointer to store the actual number of bytes read.
 * @return int8_t             @ref OTAPP_COAP_OK on success, or @ref OTAPP_COAP_ERROR if buffer is too small.
 */
int8_t otapp_coapReadPayload(otMessage *aMessage, uint8_t *bufferOut, uint16_t bufferSize, uint16_t *readBytesOut);

/**
 * @brief main function for HARDWARE DEVICE URIS. This function processing incoming request. 
 *        Request can be concern adding device to subscribe list or general uri request to next processing.
 *        If it is general uri request, then the given bufOut contains data to processing. 
 *          
 * @param aMessage      [in] ptr to incoming message
 * @param aMessageInfo  [in] ptr to message info
 * @param uriId         [in] specific HARDWARE DEVICE URI ID that was passed in uriGetListSize callback
 * @param bufOut        [out] ptr to buffer. Incomming data will be saved there.
 * @param bufSize       [in] size of bufOut
 * @return int8_t       [out]   OTAPP_COAP_OK when same data was saved in buffer,
 *                              OAC_URI_OBS_ADDED_NEW_DEVICE or OAC_URI_OBS_NO_NEED_UPDATE when observer function processed,
 *                              OTAPP_COAP_ERROR
 */
int8_t otapp_coap_processUriRequest(otMessage *aMessage, const otMessageInfo *aMessageInfo, uint8_t uriId, uint8_t *bufOut, uint16_t bufSize);

#endif  /* OT_APP_COAP_H_ */

/**
 * @}
 */