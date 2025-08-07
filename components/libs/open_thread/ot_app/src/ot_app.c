/**
 * @file thread_udp.c
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
#include <stdio.h>
#include <string.h>
#include "esp_ot_cli.h"

#include "esp_openthread.h"
#include "openthread/udp.h"
#include "openthread/dataset.h"
#include "openthread/srp_client.h"
#include "ot_app_coap.h"
#include "ot_app_dataset_tlv.h"

static const char *TAG = "ot_app";
static const char *otapp_hostName = "device1";
static const char *otapp_serviceName = "_coap._udp";
static otSrpClientService otapp_otSrpClientService;

otInstance *openThreadInstance;
const static otIp6Address *otapp_Ip6Address;

static otUdpSocket udp_socket;
otOperationalDatasetTlvs dataset;

static char otapp_charBuf[OTAPP_CHAR_BUFFER];

const otIp6Address ot_app_multicastAddr = {
    .mFields.m8 = {
        0xff, 0x03, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01
    }
};

const otIp6Address *otapp_getMulticastAddr()
{
    return &ot_app_multicastAddr;
}

otInstance *otapp_getOpenThreadInstancePtr()
{
    return openThreadInstance;
}

char *otapp_getCharBuf()
{
    return otapp_charBuf;
}

void otapp_cli_init(void)
{
    esp_ot_cli_init();
}

void otapp_setDataset_tlv(void)
{
    memcpy(&dataset.mTlvs, otapp_dataset_tlv, sizeof(otapp_dataset_tlv));
    dataset.mLength = sizeof(otapp_dataset_tlv);
    esp_openthread_auto_start(&dataset);
}

//
// UDP init
//
void otapp_receive_callback(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo) 
{
    int len = otMessageRead(aMessage, 0, otapp_charBuf, OTAPP_CHAR_BUFFER);
    ESP_LOGI(TAG, "Received UDP packet: %.*s", len, otapp_charBuf);
}

void otapp_udpStart(void) 
{
    otSockAddr bind_addr;
    memset(&bind_addr, 0, sizeof(bind_addr));
    otIp6AddressFromString("::", &bind_addr.mAddress); // nasłuch na wszystkich adresach
    bind_addr.mPort =OTAPP_UDP_PORT;

    otError err = otUdpOpen(openThreadInstance, &udp_socket, otapp_receive_callback, NULL);
    if (err != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "otUdpOpen failed: %d", err);
        return;
    }

    err = otUdpBind(openThreadInstance, &udp_socket, &bind_addr, OT_NETIF_THREAD);
    if (err != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "otUdpBind failed: %d", err);
        return;
    }

    ESP_LOGI(TAG, "UDP socket initialized and bound to port %d",OTAPP_UDP_PORT);
}

static void otapp_printIp6Address(const otIp6Address *aAddress)
{
    if (aAddress != NULL)
    {
        otIp6AddressToString(aAddress, otapp_charBuf, OTAPP_CHAR_BUFFER); 
        printf("%s\n", otapp_charBuf);
    }
}
                                 
static void otapp_deviceStateChangedCallback(otChangedFlags flags, void *context) 
{
    if (flags & OT_CHANGED_THREAD_RLOC_ADDED) 
    {
        // otapp_netifAddress = otIp6GetUnicastAddresses(otapp_getOpenThreadInstancePtr());
        
        otapp_Ip6Address = otThreadGetMeshLocalEid(otapp_getOpenThreadInstancePtr());

        printf(">>>>>>> device address has been updated: ");
        otapp_printIp6Address(otapp_Ip6Address);
    }
    if (flags & OT_CHANGED_THREAD_RLOC_REMOVED) 
    {
        printf(">>>>>>> device address has been deleted");
    }
}

static void otapp_srpClientSetHostName(otInstance *instance, const char *hostName)
{
    otError error;

    error = otSrpClientSetHostName(instance, hostName);
    if (error != OT_ERROR_NONE)
    {
        printf("Error: hostname SRP NOT set: %d\n", error);
        return;
    }
}

static void otapp_srpClientEnableAutoHostAddress(otInstance *instance)
{
    otError error;
    error = otSrpClientEnableAutoHostAddress(instance);
    if (error != OT_ERROR_NONE)
    {
        printf("Błąd ustawienia adresów IPv6 hosta SRP: %d\n", error);
        return;
    }
}

static void otapp_srpClientAddService(otInstance *instance)
{
    otError error;

    otapp_otSrpClientService.mName = otapp_serviceName;               
    otapp_otSrpClientService.mInstanceName = otapp_hostName;    
    otapp_otSrpClientService.mSubTypeLabels = NULL;              
    otapp_otSrpClientService.mTxtEntries = NULL;                 
    otapp_otSrpClientService.mPort = OTAPP_COAP_PORT;               
    otapp_otSrpClientService.mPriority = 0;                      
    otapp_otSrpClientService.mWeight = 0;                       
    otapp_otSrpClientService.mNumTxtEntries = 0;                
    otapp_otSrpClientService.mState = OT_SRP_CLIENT_ITEM_STATE_TO_ADD; 
    otapp_otSrpClientService.mData = 0;                         
    otapp_otSrpClientService.mNext = NULL;                     
    otapp_otSrpClientService.mLease = 7200;                   
    otapp_otSrpClientService.mKeyLease = 86400;                
    
    error = otSrpClientAddService(instance, &otapp_otSrpClientService);
    if (error != OT_ERROR_NONE)
    {
        printf("Error: SRP service add: %d\n", error);
        return;
    }
}
void otapp_srpClientAutoStartCallback(const otSockAddr *aServerSockAddr, void *aContext)
{
    if(NULL != aServerSockAddr)
    {
        printf("SRP SERVER detected on IP: ");
        otapp_printIp6Address(&aServerSockAddr->mAddress);
    }else
    {
         printf("SRP SERVER lost\n");
    }
}

static void otapp_srpClientInit(otInstance *instance)
{
    otSrpClientStop(instance);
    otapp_srpClientSetHostName(instance, otapp_hostName);
    otapp_srpClientEnableAutoHostAddress(instance);
    otapp_srpClientAddService(instance);
     
    if(otSrpClientIsAutoStartModeEnabled(instance))
    {
        printf("SRP client has already ran\n");
        return;
    }else
    {
       otSrpClientEnableAutoStartMode(instance, otapp_srpClientAutoStartCallback, NULL); 
    }
    
    printf("SRP client Auto start Enabled \n");
}


//
// init functions
//
void otapp_network_init() // this function will be initialize in ot_task_worker rtos task (esp_ot_cli.c)
{
    otapp_setDataset_tlv();
    // otapp_udpStart(); 
    otapp_coap_init();    
    otapp_srpClientInit(otapp_getOpenThreadInstancePtr());
}

void otapp_init(void) //app init
{
    openThreadInstance = esp_openthread_get_instance();
    otapp_cli_init();    
    otSetStateChangedCallback(otapp_getOpenThreadInstancePtr(),otapp_deviceStateChangedCallback, NULL);
    
}