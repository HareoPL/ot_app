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
#include "ot_app_coap_uri_test.h"

static otCoapResource otapp_coap_resource[] = {
    {".well-known/core", otapp_coap_uri_well_knownCoreHandle, NULL, NULL},
    {"test", otapp_coap_uri_testHandle, NULL, NULL}

};
#define OTAPP_COAP_RESOURCE_SIZE (sizeof(otapp_coap_resource)/sizeof(otapp_coap_resource[0]))



//////////////////////////////
// do not edit below
static otMessageInfo mMessage;
// static otCoapResource newResource;

static char charBuf[OTAPP_CHAR_BUFFER];


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
   
    //  uint16_t len = otMessageGetLength(requestMessage) - otMessageGetOffset(requestMessage);
    // uint8_t buffer[128]; 
    // otMessageRead(requestMessage, otMessageGetOffset(requestMessage), buffer, len);
    // // Przetwarzaj dane z buffer


    otCoapCode requestCode = otCoapMessageGetCode(requestMessage);
    // otCoapType messageType = otCoapMessageGetType(requestMessage);

    // Only GET support with confirmable
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
        if (error != OT_ERROR_NONE)
        {
            goto exit;
        }

        // // Add marker payload's and payload
        error = otCoapMessageSetPayloadMarker(responseMessage);
        if (error != OT_ERROR_NONE)
        {
            goto exit;
        }

        error = otMessageAppend(responseMessage, responceContent, (uint16_t)strlen(responceContent));
        if (error != OT_ERROR_NONE)
        {
            goto exit;
        }

        // send response with default parameters
        error = otCoapSendResponseWithParameters(otapp_getOpenThreadInstancePtr(), responseMessage, aMessageInfo, NULL);
        if (error != OT_ERROR_NONE)
        {
            goto exit;
        }

        printf("CoAP response sent.\n");       
    }
    else
    {
        printf("CoAP method not supported: %d\n", requestCode);
        error = OT_ERROR_NOT_IMPLEMENTED;
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

void otapp_coap_client_send_get(const otIp6Address *peer_addr, const char *aUriPath)
{
    otError error;
    otMessage *message = NULL;

    mMessage.mPeerAddr = *peer_addr;
    mMessage.mPeerPort = OT_DEFAULT_COAP_PORT;
    mMessage.mHopLimit = 0; // standard limitl
    mMessage.mIsHostInterface = false; // package comes out of openthread interface

    // create new message CoAP
    message = otCoapNewMessage(otapp_getOpenThreadInstancePtr(), NULL);
    if (!message)
        return;

    // message initialize as Confirmable GET
    otCoapMessageInit(message, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_GET);

    // add URI
    otCoapMessageAppendUriPathOptions(message, aUriPath);

    // send request. otapp_coap_responseHandler 
    error = otCoapSendRequest(otapp_getOpenThreadInstancePtr(), message, &mMessage, otapp_coap_responseHandler, NULL);
    if ((error != OT_ERROR_NONE) && (message != NULL)) //free only when is error. when succes CoAP stack do free message
    {
        otMessageFree(message);
    }

}

char *charTest = {"test"};
void otapp_coapSendtoTest()
{
    otapp_coap_client_send_get(otapp_getMulticastAddr(), charTest);
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

