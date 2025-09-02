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

#include "main.h"
#include "ot_app_coap_uri.h"
#include "string.h"
#include "ot_app.h"
#include "ot_app_deviceName.h"

void otapp_coap_uri_testHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{
    otapp_coap_printSenderIP(aMessageInfo);

    if (request)
    {

        otapp_coap_sendResponse(request, aMessageInfo, otapp_coap_getMessage(OTAPP_MESSAGE_TEST));
    }
}

static char charBuffer[1024];

void otapp_coap_uri_ledControlHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{
    otapp_coap_printSenderIP(aMessageInfo);

    if (request)
    {
        uint16_t len = otMessageGetLength(request) - otMessageGetOffset(request);

        uint16_t lenOfReadedBytes = otMessageRead(request, otMessageGetOffset(request), charBuffer, len);

        printf("Sender data: %s bytes: %d\n ", charBuffer, lenOfReadedBytes);
        otapp_coap_sendResponse(request, aMessageInfo, NULL);
    }
}

void otapp_coap_uri_paringServicesHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{    
    if (request)
    {
        printf("from uri: paring_services \n");
        char incommingHostName[OTAPP_DEVICENAME_FULL_SIZE];

        uint8_t strCompare = 0;


            uint16_t len = otMessageGetLength(request) - otMessageGetOffset(request);    
            uint16_t lenOfReadedBytes = otMessageRead(request, otMessageGetOffset(request), incommingHostName, len);
            incommingHostName[lenOfReadedBytes] = '\0';

            printf("Sender data: %s bytes: %d\n ", incommingHostName, lenOfReadedBytes);
            otapp_coap_sendResponse(request, aMessageInfo, NULL);


            strCompare = otapp_deviceNameIsSame(incommingHostName, lenOfReadedBytes);

            if(strCompare)
            {
                printf("new devica has been successfully paired. Dev ID: %d \n", otapp_deviceNameGetDevId(incommingHostName, lenOfReadedBytes));
                // todo check otapp pair.... 
            }else
            {
                printf("new device has NOT been paried ! \n ");
            }
       
    }
}

