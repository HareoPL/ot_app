/**
 * @file ot_app_coap_uri.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 24-07-2025
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

// #include "main.h"
#include "ot_app_coap_uri.h"
#include "string.h"
#include "ot_app.h"
#include "ot_app_deviceName.h"
#include "ot_app_pair.h"
#include "ot_app_drv.h"
#include "ot_app_buffer.h"

#include <openthread/coap.h>
#include <openthread/instance.h>
#include <openthread/message.h>
#include <string.h>

#define TAG "ot_app_coap_uri "

#define OTAPP_COA_URI_BUFFER 264
void otapp_coap_uri_testHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{
    otapp_coap_printSenderIP(aMessageInfo);

    if (request)
    {

        otapp_coap_sendResponse(request, aMessageInfo, (uint8_t*) otapp_coap_getMessage(OTAPP_MESSAGE_TEST), strlen(otapp_coap_getMessage(OTAPP_MESSAGE_TEST)));
    }
}

static char charBuffer[1024];

void otapp_coap_uri_ledControlHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{
    uint16_t readBytes = 0;
    otapp_coap_printSenderIP(aMessageInfo);

    if (request)
    {
        if(otapp_coapReadPayload(request, (uint8_t*)charBuffer, 1024, &readBytes) == OTAPP_COAP_ERROR) return; 
        
        otapp_coap_sendResponseOK(request, aMessageInfo);      
        OTAPP_PRINTF(TAG, "Sender data: %s bytes: %d \n", charBuffer, readBytes);
    }
}

void otapp_coap_uri_paringServicesHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{    
    otapp_pair_queueItem_t queueItem; 
    uint16_t readBytes = 0;

    if (request)
    {
        OTAPP_PRINTF(TAG, "from uri: paring_services \n");
        if(otapp_coapReadPayload(request, (uint8_t*)queueItem.deviceNameFull, OTAPP_PAIR_NAME_FULL_SIZE, &readBytes) == OTAPP_COAP_ERROR) return; 
        
        otapp_coap_sendResponseOK(request, aMessageInfo);        

        queueItem.type = OTAPP_PAIR_CHECK_AND_ADD_TO_DEV_LIST;
        memcpy(&queueItem.ipAddress, &aMessageInfo->mPeerAddr, sizeof(otIp6Address));

        OTAPP_PRINTF(TAG, "Sender data: %s \n", queueItem.deviceNameFull );
        otapp_coap_printSenderIP(aMessageInfo);

        OTAPP_PRINTF(TAG, "URI: Add item to queue\n");
        otapp_pair_addToQueue(&queueItem);    
    }
}

void ad_temp_uri_well_knownCoreHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{
    int8_t result = 0;
    ot_app_devDrv_t *devDrv_ = otapp_getDevDrvInstance();

    otapp_coap_uri_t *urisList = NULL;    
    ot_app_size_t uriListSize = 0;

    uint8_t *buffer = NULL;
    uint16_t bufferSize = 0;
    
    if (request && devDrv_)
    {
        // Retrieve the URI list defined in the device driver
        urisList = devDrv_->uriGetList_clb();
        uriListSize = devDrv_->uriGetListSize;
        if(urisList == NULL || uriListSize == 0) return;

        // Acquire access to the thread-safe global buffer
        bufferSize = otapp_pair_uriResourcesCalculateBufSize(urisList, uriListSize);
        buffer = otapp_buf_getWriteOnly_ptr(OTAPP_BUF_KEY_1, bufferSize);
        if(buffer == NULL || bufferSize == 0) 
        {
            otapp_buf_writeUnlock(OTAPP_BUF_KEY_1);
            OTAPP_PRINTF(TAG, "ERROR well-known/core: buffer = NULL"); 
            return;
        }         

        // Serialize URI data into TLV format
        result = otapp_pair_uriResourcesCreate(urisList, uriListSize, buffer, &bufferSize);
        if(result == OTAPP_PAIR_OK)
        {
            // Send the populated TLV buffer as a CoAP response
            otapp_coap_sendResponse(request, aMessageInfo, buffer, bufferSize);
            OTAPP_PRINTF(TAG, "well-known/core: sent resources size: %d\n", bufferSize);
        }else
        {
            OTAPP_PRINTF(TAG, "ERROR well-known/core: uriResourcesCreate \n");
        }

        // unlock the buffer
        otapp_buf_writeUnlock(OTAPP_BUF_KEY_1);
    }
}

void otapp_coap_uri_subscribedHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{
    int8_t result;

    ot_app_devDrv_t *drv;
    oac_uri_dataPacket_t *dataPacket;
    uint16_t readBytes = 0;
   
    uint8_t *buffer = NULL;
    uint16_t bufferSize = 0;


    if (request)
    {
        bufferSize = otMessageGetLength(request) - otMessageGetOffset(request);
        buffer = otapp_buf_getWriteOnly_ptr(OTAPP_BUF_KEY_1, bufferSize);
        if(buffer == NULL || bufferSize == 0) 
        {
            otapp_buf_writeUnlock(OTAPP_BUF_KEY_1);
            OTAPP_PRINTF(TAG, "ERROR ubscribedHandle: buffer = NULL\n"); 
            return;
        }   
        result = otapp_coapReadPayload(request, buffer, bufferSize, &readBytes);
        if(result != OTAPP_COAP_OK || bufferSize != readBytes)
        {
            otapp_buf_writeUnlock(OTAPP_BUF_KEY_1);
            OTAPP_PRINTF(TAG, "ERROR ubscribedHandle: readPayload\n");
            return;
        } 
        
        otapp_coap_sendResponseOK(request, aMessageInfo);

        drv = otapp_getDevDrvInstance();

        dataPacket = oac_uri_obs_getdataPacketHandle();
        result = oac_uri_obs_parseMessageFromNotify(buffer, readBytes, dataPacket); 
        if(result == OAC_URI_OBS_ERROR)
        {
            otapp_buf_writeUnlock(OTAPP_BUF_KEY_1);
            OTAPP_PRINTF(TAG, "ERROR: ubscribedHandle\n");
            return;
        }

        otapp_buf_writeUnlock(OTAPP_BUF_KEY_1);
        drv->obs_subscribedUri_clb(dataPacket); // inform app device about new subscribed event.         
    }
}
