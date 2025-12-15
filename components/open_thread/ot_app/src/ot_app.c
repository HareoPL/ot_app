/**
 * @file ot_app.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 11-07-2025
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
#include "ot_app.h"
#include "ot_app_coap.h"
#include "ot_app_pair.h"
#include "ot_app_dataset_tlv.h"
#include "ot_app_deviceName.h"
#include "ot_app_srp_client.h"
#include "ot_app_drv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_ot_cli.h"

#include "esp_openthread.h"
#include "openthread/dataset.h"


#include <inttypes.h>

// static const char *TAG = "ot_app";

static ot_app_devDrv_t *otapp_devDrv;

static otInstance *openThreadInstance;
const static otIp6Address *otapp_Ip6Address;

static otExtAddress otapp_factoryEUI_64;

static char otapp_charBuf[OTAPP_CHAR_BUFFER_SIZE];
static SemaphoreHandle_t otapp_mutexBuf;



static const otIp6Address ot_app_multicastAddr = {
    .mFields.m8 = {
        0xff, 0x03, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01
    }
};

const otIp6Address *otapp_multicastAddressGet()
{
    return &ot_app_multicastAddr;
}

const otIp6Address *otapp_ip6AddressGet()
{
    return otapp_Ip6Address;
}

const otIp6Address *otapp_ip6AddressRefresh()
{
    otapp_Ip6Address = otThreadGetMeshLocalEid(otapp_getOpenThreadInstancePtr());
    return otapp_Ip6Address;
}

void otapp_ip6AddressPrint(const otIp6Address *aAddress)
{
    if (aAddress != NULL)
    {
        char buf[OT_IP6_ADDRESS_STRING_SIZE];

        otIp6AddressToString(aAddress, buf, OTAPP_CHAR_BUFFER_SIZE); 
        printf("%s\n", buf);
    }
}

void otapp_macAddrPrint(const otExtAddress *macAddr)
{
    if(macAddr != NULL)
    {
        printf("Factory EUI-64: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
               macAddr->m8[0], macAddr->m8[1], macAddr->m8[2], macAddr->m8[3],
               macAddr->m8[4], macAddr->m8[5], macAddr->m8[6], macAddr->m8[7]);
    }else
    {
        printf("ERROR: Factory EUI-64: - null ptr \n");
    }
}

const otExtAddress *otapp_macAddrGet(void)
{
    return &otapp_factoryEUI_64;
}

static void otapp_macAddrInit(void)
{
    otLinkGetFactoryAssignedIeeeEui64(otapp_getOpenThreadInstancePtr(), &otapp_factoryEUI_64);
    otapp_macAddrPrint(&otapp_factoryEUI_64);
}

otInstance *otapp_getOpenThreadInstancePtr()
{
    return openThreadInstance;
}

ot_app_devDrv_t *otapp_getDevDrvInstance(void)
{
    return otapp_devDrv;
}

/////////////////////////
// char buffer
//
char *otapp_charBufGet_withMutex()
{
    if(xSemaphoreTake(otapp_mutexBuf, portMAX_DELAY) == pdTRUE)
    {
        return otapp_charBuf;
    }
    return NULL; // it should never come here
}

void otapp_charBufRelease()
{
    xSemaphoreGive(otapp_mutexBuf);
}

void otapp_cli_init(void)
{
    esp_ot_cli_init();
}

void otapp_setDataset_tlv(void)
{
    esp_openthread_auto_start(&otapp_dataset_tlv);
}

static void otapp_deviceStateChangedCallback(otChangedFlags flags, void *context) 
{
    int8_t result = 0;

    if (flags & OT_CHANGED_THREAD_RLOC_ADDED) 
    {
        otapp_coapSendDeviceNamePut();
        otapp_srpClientUpdateHostAddress(otapp_getOpenThreadInstancePtr());
        
        result = otapp_pair_subSendUpdateIP(otapp_pair_getHandle());
        if(result != OTAPP_PAIR_ERROR)
        {
            printf(">>>>>>> Num of updated sub: %d \n", result);
        }
        
        printf(">>>>>>> device address has been updated: ");
        otapp_ip6AddressPrint(otapp_Ip6Address);
    }
    if (flags & OT_CHANGED_THREAD_RLOC_REMOVED) 
    {
        printf(">>>>>>> device address has been deleted");
    }
}


///////////////////////
//
// init functions
//
void otapp_network_init() // this function will be initialize in ot_task_worker rtos task (esp_ot_cli.c)
{
    otapp_setDataset_tlv();
    
    otapp_macAddrInit();
    otapp_deviceNameSet(otapp_devDrv->deviceName, *otapp_devDrv->deviceType);
    otapp_coap_init(otapp_devDrv);    
    otapp_srpInit();
}

int8_t otapp_init() //app init
{    
    otapp_devDrv = ot_app_drv_getInstance();

    openThreadInstance = esp_openthread_get_instance();
    
        
    otapp_cli_init();    
    otSetStateChangedCallback(otapp_getOpenThreadInstancePtr(),otapp_deviceStateChangedCallback, NULL);
    otapp_mutexBuf = xSemaphoreCreateMutex();
    otapp_pair_init(otapp_devDrv);
    
    return OTAPP_OK;
}