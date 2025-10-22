/**
 * @file ot_app_coap.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 21-07-2025
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2025 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * 
 */
#ifndef OT_APP_COAP_H_
#define OT_APP_COAP_H_

#include "hro_utils.h"

#ifdef UNIT_TEST
    #include "mock_ip6.h"
#else
    #include "esp_openthread.h"
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


int8_t otapp_coap_init(ot_app_devDrv_t *devDriver);
void otapp_coap_sendResponse(otMessage *requestMessage, const otMessageInfo *aMessageInfo, const uint8_t *responceContent, uint16_t responceLength);
void otapp_coap_printSenderIP(const otMessageInfo *aMessageInfo);

// void otapp_coap_clientSendPut(const otIp6Address *peer_addr, const char *aUriPath, const char *payloadMsg);
// void otapp_coap_clientSendGet(const otIp6Address *peer_addr, const char *aUriPath);

const char *otapp_coap_getMessage(otapp_coap_messageId_t msgID);

void otapp_coapSendtoTestPut();
void otapp_coapSendtoTestGet();
void otapp_coapSendDeviceNamePut();
void otapp_coapSendGetUri_Well_known(const otIp6Address *ipAddr, otCoapResponseHandler responseHandler, void *aContext);
void otapp_coapSendPutUri_subscribed_uris(const otIp6Address *ipAddr, const uint8_t *data, uint16_t dataSize);
void otapp_coapSendSubscribeRequest(const otIp6Address *ipAddr, const char *aUriPath, uint8_t *tokenOut);
void otapp_coapSendSubscribeRequestUpdate(const otIp6Address *ipAddr, const char *aUriPath, uint8_t *tokenIn);

void otapp_coap_clientSendPutByte(const otIp6Address *peer_addr, const char *aUriPath, const uint8_t *payloadMsg, const uint16_t payloadMsgSize, otCoapResponseHandler responseHandler, void *aContext);
void otapp_coap_clientSendGetByte(const otIp6Address *peer_addr, const char *aUriPath, otCoapResponseHandler responseHandler, void *aContext);

const char *otapp_coap_getUriNameFromDefault(otapp_coap_uriIndex_t uriIndex);

int8_t otapp_coapReadPayload(otMessage *aMessage, uint8_t *bufferOut, uint16_t bufferSize, uint16_t *readBytesOut);

int8_t otapp_coap_processUriRequest(otMessage *aMessage, const otMessageInfo *aMessageInfo, uint8_t uriId, uint8_t *bufOut, uint16_t bufSize);

#endif  /* OT_APP_COAP_H_ */
