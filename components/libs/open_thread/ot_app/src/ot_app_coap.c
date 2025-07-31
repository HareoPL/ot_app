/**
 * @file ot_app_coap.c
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
#include "main.h"
#include "ot_app_coap.h"
#include "ot_app.h"

#include <openthread/ip6.h>
#include <openthread/coap.h>

////////////////////////
// add uri here:
#include "ot_app_coap_uri_well-known-core.h"
#include "ot_app_coap_uri.h"

static otCoapResource otapp_coap_resource[] = {
    {".well-known/core", otapp_coap_uri_well_knownCoreHandle, NULL, NULL},
    {"test", otapp_coap_uri_testHandle, NULL, NULL},
    {"device/led", otapp_coap_uri_ledControlHandle, NULL, NULL}

};
#define OTAPP_COAP_RESOURCE_SIZE (sizeof(otapp_coap_resource) / sizeof(otapp_coap_resource[0]))

typedef struct {
    otapp_coap_messageId_t msgID;
    char *message;
}otapp_coap_message_t;

static const otapp_coap_message_t otapp_coap_messages[] = {
    {OTAPP_MESSAGE_OK, "OK"},
    {OTAPP_MESSAGE_ERROR, "ERROR"},
    {OTAPP_MESSAGE_TEST, "Hello coap !!"},
};
#define OTAPP_COAP_MESSAGE_SIZE (sizeof(otapp_coap_messages) / sizeof(otapp_coap_messages[0]))

//////////////////////////////
// do not edit below
//

static otMessageInfo mMessage;
static char charBuf[OTAPP_CHAR_BUFFER];

const char *otapp_coap_getMessage(otapp_coap_messageId_t msgID)
{
    for (uint16_t i = 0; i < OTAPP_COAP_MESSAGE_SIZE; i++)
    {
        if(otapp_coap_messages[i].msgID == msgID)
        {
            return otapp_coap_messages[i].message;
        }
    }
    
    return NULL;
} 

const char *otapp_coap_getUriName(otapp_coap_uriTableIndex_t uriIndex)
{
    return otapp_coap_resource[uriIndex].mUriPath;
} 

void otapp_coap_responseHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aResult)
{
    if (aResult == OT_ERROR_NONE && aMessage)
    {
        // Obsłuż odpowiedź (przeczytaj payload, kod odpowiedzi itd.) 
       
    }
}

void otapp_coap_requestHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    if (aMessage)
    {
        // Obsłuż odpowiedź (przeczytaj payload, kod odpowiedzi itd.)
    }
}

void otapp_coap_printSenderIP(const otMessageInfo *aMessageInfo)
{
    if (aMessageInfo != NULL)
    {
        // read sender IPv6 address
        const otIp6Address *sender_addr = &aMessageInfo->mPeerAddr;
        uint16_t sender_port = aMessageInfo->mPeerPort;

        // print it on uart
        otIp6AddressToString(sender_addr, charBuf, OTAPP_CHAR_BUFFER); // OT_IP6_ADDRESS_STRING_SIZE
        printf("Sender address: %s, port: %u\n", charBuf, sender_port);
    }
}

void otapp_coap_sendResponse(otMessage *requestMessage, const otMessageInfo *aMessageInfo, const char *responceContent)
{
    otError error = OT_ERROR_NONE;
    otMessage *responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_EMPTY;
   
    otCoapCode requestCode = otCoapMessageGetCode(requestMessage);

    if (requestCode == OT_COAP_CODE_GET)
    {
        responseCode = OT_COAP_CODE_CONTENT;

        responseMessage = otCoapNewMessage(otapp_getOpenThreadInstancePtr(), NULL);
        if (responseMessage == NULL)
        {
            error = OT_ERROR_NO_BUFS;
            goto exit;
        }

        // Create ACK for GET query
        error = otCoapMessageInitResponse(responseMessage, requestMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
        if (error != OT_ERROR_NONE) { goto exit; }

        // // Add marker payload's and payload
        error = otCoapMessageSetPayloadMarker(responseMessage);
        if (error != OT_ERROR_NONE) { goto exit; }

        if (NULL == responceContent) { goto exit; }
        error = otMessageAppend(responseMessage, (const uint8_t *)responceContent, (uint16_t)strlen(responceContent));
        if (error != OT_ERROR_NONE) { goto exit; }
 
    }
    else if(requestCode == OT_COAP_CODE_PUT)
    {
        responseCode = OT_COAP_CODE_CHANGED;

        responseMessage = otCoapNewMessage(otapp_getOpenThreadInstancePtr(), NULL);
        if (responseMessage == NULL)
        {
            error = OT_ERROR_NO_BUFS;
            goto exit;
        }

        // Create ACK for GET query
        error = otCoapMessageInitResponse(responseMessage, requestMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
        if (error != OT_ERROR_NONE)  { goto exit; }
    
    }
    else
    {
        printf("CoAP method not supported: %d\n", requestCode);
        error = OT_ERROR_NOT_IMPLEMENTED;
        return;
    }

    // send response with default parameters
    error = otCoapSendResponseWithParameters(otapp_getOpenThreadInstancePtr(), responseMessage, aMessageInfo, NULL);
    if (error != OT_ERROR_NONE)
    {
        goto exit;
    }else
    {
        printf("CoAP response sent.\n");  
    }

exit:
    if (error != OT_ERROR_NONE)
    {
        printf("CoAP error: %d (%s)\n", error, otThreadErrorToString(error));
        if (responseMessage != NULL)
        {
            otMessageFree(responseMessage);
        }
    }
}

void otapp_coap_client_send(const otIp6Address *peer_addr, const char *aUriPath, otCoapCode code, const char *payloadMsg)
{
    otError error;
    otMessage *message = NULL;

    mMessage.mPeerAddr = *peer_addr;
    mMessage.mPeerPort = OT_DEFAULT_COAP_PORT;
    mMessage.mHopLimit = 0; // standard limit
    mMessage.mIsHostInterface = false; // package comes out of openthread interface

    if(NULL == peer_addr || NULL == aUriPath)
    {
        return;
    }

    // create new message CoAP
    message = otCoapNewMessage(otapp_getOpenThreadInstancePtr(), NULL);
    if (message == NULL)
    {
        error = OT_ERROR_NO_BUFS;
        goto exit;
    }

    // message initialize as Confirmable GET
    otCoapMessageInit(message, OT_COAP_TYPE_CONFIRMABLE, code);

    // add URI
    error = otCoapMessageAppendUriPathOptions(message, aUriPath);
    if(error != OT_ERROR_NONE) { goto exit; }

    if(code == OT_COAP_CODE_PUT)
    {
        if(NULL == payloadMsg) { goto exit; }

        error = otCoapMessageSetPayloadMarker(message);
        if (error != OT_ERROR_NONE) { goto exit; }

        error = otMessageAppend(message, (const uint8_t *)payloadMsg, strlen(payloadMsg));
        if (error != OT_ERROR_NONE) { goto exit; }
    }

    // send request. otapp_coap_responseHandler 
    error = otCoapSendRequest(otapp_getOpenThreadInstancePtr(), message, &mMessage, otapp_coap_responseHandler, NULL);
    if (error != OT_ERROR_NONE) { goto exit; }

exit:
    if (error != OT_ERROR_NONE)
    {
        printf("CoAP send() error: %d (%s)\n", error, otThreadErrorToString(error));
        if (message != NULL)
        {
            otMessageFree(message);
        }
    }   
}

void otapp_coap_clientSendPut(const otIp6Address *peer_addr, const char *aUriPath, const char *payloadMsg)
{
   otapp_coap_client_send(peer_addr, aUriPath, OT_COAP_CODE_PUT, payloadMsg);
}

void otapp_coap_clientSendGet(const otIp6Address *peer_addr, const char *aUriPath)
{
   otapp_coap_client_send(peer_addr, aUriPath, OT_COAP_CODE_GET, NULL);
}

// char *charTest = {"test"};

void otapp_coapSendtoTestGet()
{
    printf("CoAP sent get to uri: test\n");
    otapp_coap_clientSendGet(otapp_getMulticastAddr(), otapp_coap_getUriName(OTAPP_URI_TEST));
}

// char *charLed = {"device/led"};
char *charLedPayload = {"LED_ON"};
void otapp_coapSendtoTestPut()
{
    printf("CoAP sent put to uri: device/led \n");
    otapp_coap_clientSendPut(otapp_getMulticastAddr(), otapp_coap_getUriName(OTAPP_URI_DEVICE_TEST), charLedPayload);
}

void otapp_coap_initCoapResource()
{
    for (uint32_t i = 0; i < OTAPP_COAP_RESOURCE_SIZE; i++)
    {
        otCoapAddResource(otapp_getOpenThreadInstancePtr(), &otapp_coap_resource[i]);
    }
}

void otapp_coap_init()
{
    otError error;

    error = otCoapStart(otapp_getOpenThreadInstancePtr(), OT_DEFAULT_COAP_PORT);
    if (error != OT_ERROR_NONE)
    {
        // Obsłużimożliwe błędy inicjacji
    }
    // init coap handler
    otCoapSetDefaultHandler(otapp_getOpenThreadInstancePtr(), otapp_coap_requestHandler, NULL);
    
    otapp_coap_initCoapResource();
}

