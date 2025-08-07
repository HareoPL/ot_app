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
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_openthread.h"
#include "openthread/udp.h"
#include "openthread/dataset.h"
#include "openthread/srp_client.h"
#include "ot_app_coap.h"
#include "ot_app_dataset_tlv.h"

static const char *TAG = "ot_app";

static const char *otapp_hostName = "device1";
static const char *otapp_serviceName = "_coap._udp";
static const char *otapp_browseDefaultServiceName = "_coap._udp.default.service.arpa.";
static otapp_DNS_services_t otapp_DNS_services[OTAPP_DNS_SERVICES_MAX];

otInstance *openThreadInstance;
const static otIp6Address *otapp_Ip6Address;

static otUdpSocket udp_socket;
otOperationalDatasetTlvs dataset;

static char otapp_charBuf[OTAPP_CHAR_BUFFER_SIZE];
SemaphoreHandle_t otapp_mutexBuf;

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
    memcpy(&dataset.mTlvs, otapp_dataset_tlv, sizeof(otapp_dataset_tlv));
    dataset.mLength = sizeof(otapp_dataset_tlv);
    esp_openthread_auto_start(&dataset);
}

/////////////////////////
// UDP init
//
void otapp_receive_callback(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo) 
{
    char *buf = otapp_charBufGet_withMutex();

    if (buf != NULL)
    {
        int len = otMessageRead(aMessage, 0, buf, OTAPP_CHAR_BUFFER_SIZE);
        ESP_LOGI(TAG, "Received UDP packet: %.*s", len, buf);
    }else
    {
        printf("NULL PTR from otapp_charBufGet_withMutex");
    }

    otapp_charBufRelease();
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

void otapp_printIp6Address(const otIp6Address *aAddress)
{
    if (aAddress != NULL)
    {
        char *buf = otapp_charBufGet_withMutex();

        if (buf != NULL)
        {
            otIp6AddressToString(aAddress, buf, OTAPP_CHAR_BUFFER_SIZE); 
            printf("%s\n", buf);
            otapp_charBufRelease();
        }else
        {
            printf("NULL PTR from otapp_charBufGet_withMutex");
        }
        
        otapp_charBufRelease();        
    }
}
                                 
static void otapp_deviceStateChangedCallback(otChangedFlags flags, void *context) 
{
    if (flags & OT_CHANGED_THREAD_RLOC_ADDED) 
    {
        otapp_Ip6Address = otThreadGetMeshLocalEid(otapp_getOpenThreadInstancePtr());

        printf(">>>>>>> device address has been updated: ");
        otapp_printIp6Address(otapp_Ip6Address);
    }
    if (flags & OT_CHANGED_THREAD_RLOC_REMOVED) 
    {
        printf(">>>>>>> device address has been deleted");
    }
}

void otapp_macAddrPrint(const otExtAddress *macAddr)
{
    if(macAddr != NULL)
    {
        printf("MAC Address: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
               macAddr->m8[0], macAddr->m8[1], macAddr->m8[2], macAddr->m8[3],
               macAddr->m8[4], macAddr->m8[5], macAddr->m8[6], macAddr->m8[7]);
    }else
    {
        printf("ERROR: MAC Address - null ptr \n");
    }
}

const otExtAddress *otapp_macAddrGet(otInstance *instance)
{
   return otLinkGetExtendedAddress(instance);
}

///////////////////////
// dnsClient functions
//

void otapp_dnsClientBrowsePrintServiceInfo(otDnsServiceInfo *aServiceInfo)
{
    printf(" Port: %d, Priority:%d, Weight:%d, TTL:%lu \n", aServiceInfo->mPort, aServiceInfo->mPriority, aServiceInfo->mWeight, aServiceInfo->mTtl);
    printf(" Host: %s \n", aServiceInfo->mHostNameBuffer);
    printf(" HostAddress: ");
    otapp_printIp6Address(&aServiceInfo->mHostAddress);
    printf(" TTL:%lu \n", aServiceInfo->mHostAddressTtl);
    printf(" TXT: ");

    if (!aServiceInfo->mTxtDataTruncated)
    {
        printf("mTxtDataTruncated:%d \n", aServiceInfo->mTxtDataTruncated);
    }
    else
    {
        printf("[");
        for(uint16_t byte = 0; byte < aServiceInfo->mTxtDataSize; byte++)
        {
            printf("%02X", aServiceInfo->mTxtData[byte]);
        }        
        printf("...] \n");
    }
}

uint8_t txtBuffer[OTAPP_DNS_SRV_TXT_SIZE]; 

void otapp_dnsClientBrowseResponseCallback(otError aError, const otDnsBrowseResponse *aResponse, void *aContext)
{
    static otDnsServiceInfo otapp_serviceInfo;
    otDnsBrowseResponseGetServiceName(aResponse, otapp_DNS_services[0].nameBuffer, OTAPP_DNS_SRV_NAME_SIZE);

    printf("DNS browse response for %s \n", otapp_DNS_services[0].nameBuffer);

    if (aError == OT_ERROR_NONE)
    {
        uint16_t index = 0;

        while (otDnsBrowseResponseGetServiceInstance(aResponse, index, otapp_DNS_services[0].labelBuffer, OTAPP_DNS_SRV_LABEL_SIZE) == OT_ERROR_NONE)
        {
            printf("label: %s \n", otapp_DNS_services[0].labelBuffer);
            index++;

            otapp_serviceInfo.mHostNameBuffer     = otapp_DNS_services[0].nameBuffer;
            otapp_serviceInfo.mHostNameBufferSize = OTAPP_DNS_SRV_NAME_SIZE;
            otapp_serviceInfo.mTxtData            = txtBuffer;
            otapp_serviceInfo.mTxtDataSize        = sizeof(txtBuffer);

            if (otDnsBrowseResponseGetServiceInfo(aResponse, otapp_DNS_services[0].labelBuffer, &otapp_serviceInfo) == OT_ERROR_NONE)
            {
               otapp_dnsClientBrowsePrintServiceInfo(&otapp_serviceInfo);
            }

            printf("\n");
        }
    }
    otapp_charBufRelease();
}

void otapp_dnsClientBrowse(otInstance *instance, const char *serviceName)
{
    otError error = OT_ERROR_NONE; 
    static const otDnsQueryConfig *config;
    config = otDnsClientGetDefaultConfig(instance);

    error = otDnsClientBrowse(instance, serviceName, otapp_dnsClientBrowseResponseCallback, NULL, config);
    if (error != OT_ERROR_NONE)
    {
        printf("Error: DNS client browse: %d\n", error);
        return;
    }
}

///////////////////////
// srpClient functions
//
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
        printf("Error: SRP set IPv6 host addresses: %d\n", error);
        return;
    }
}

static void otapp_srpClientAddService(otInstance *instance)
{
    otError error;
    static otSrpClientService otapp_otSrpClientService;

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

void otapp_otSrpClientCallback(otError aError, const otSrpClientHostInfo *aHostInfo, const otSrpClientService *aServices, const otSrpClientService *aRemovedServices, void *aContext)
{
    if(aError == OT_ERROR_NONE)
    {
        if(aHostInfo->mState == OT_SRP_CLIENT_ITEM_STATE_REGISTERED)
        {
            printf("CHECK DNS BROWSE: \n");
            otapp_dnsClientBrowse(otapp_getOpenThreadInstancePtr(), otapp_browseDefaultServiceName);
        }       
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


///////////////////////
//
// init functions
//
void otapp_network_init() // this function will be initialize in ot_task_worker rtos task (esp_ot_cli.c)
{
    otapp_setDataset_tlv();
    // otapp_udpStart(); 
    otapp_coap_init();    
    otapp_srpClientInit(otapp_getOpenThreadInstancePtr());
    otSrpClientSetCallback(otapp_getOpenThreadInstancePtr(), otapp_otSrpClientCallback, NULL); 
    otapp_macAddrPrint(otapp_macAddrGet(otapp_getOpenThreadInstancePtr()));
}

void otapp_init(void) //app init
{
    openThreadInstance = esp_openthread_get_instance();
    otapp_cli_init();    
    otSetStateChangedCallback(otapp_getOpenThreadInstancePtr(),otapp_deviceStateChangedCallback, NULL);
    otapp_mutexBuf = xSemaphoreCreateMutex();
}