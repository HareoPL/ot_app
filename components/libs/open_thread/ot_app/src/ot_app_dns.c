/**
 * @file ot_app_dns.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 05-09-2025
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

#include "ot_app_dns.h"
#include "ot_app.h"
#include "ot_app_pair.h"
#include "ot_app_deviceName.h"

PRIVATE otapp_pair_queueItem_t queueItem;

PRIVATE int8_t otapp_dnsPairDevice(const otDnsAddressResponse *aResponse)
{
    char *charBuff = otapp_charBufGet_withMutex();

    if(charBuff == NULL)
    {
        return OTAPP_DNS_ERROR;
    }
    
    if(otDnsAddressResponseGetAddress(aResponse, 0, &queueItem.ipAddress, NULL) != OT_ERROR_NONE)
    {
        otapp_charBufRelease();
        return OTAPP_DNS_ERROR;
    }
    
    otDnsAddressResponseGetHostName(aResponse, charBuff, OTAPP_CHAR_BUFFER_SIZE);
    
    if(otapp_hostNameToDeviceNameFull(charBuff) != OTAPP_DEVICENAME_OK)
    {
        otapp_charBufRelease();
        return OTAPP_DNS_ERROR;
    }
    
    queueItem.type = OTAPP_PAIR_CHECK_AND_ADD_TO_DEV_LIST;
    strcpy(queueItem.deviceNameFull, charBuff);
    memcpy(&queueItem.ipAddress, otapp_ip6AddressGet(), sizeof(otIp6Address));

    printf("otapp_pair_addToQueue \n");
    otapp_pair_addToQueue(&queueItem);
   
    otapp_charBufRelease();

    return OTAPP_DNS_OK;
}

void otapp_otDnsAddressCallback(otError aError, const otDnsAddressResponse *aResponse, void *aContext)
{
    if(aError == OT_ERROR_NONE)
    {
        otapp_dnsPairDevice(aResponse);
    }    
}

/**
 * @brief get DNS resolve using full host name
 * 
 * @param instance [in] ptr to openthread instance
 * @param hostName [in] full host name with domain. Example: "device1_1_588c81fffe301ea4.default.service.arpa."
 */
PRIVATE int8_t otapp_dnsClientResolve(otInstance *instance, const char *hostName)
{   
    if(NULL == instance || NULL == hostName)
    {
        return OTAPP_DNS_ERROR;
    }

    otError                  error = OT_ERROR_NONE;
    const otDnsQueryConfig *config = otDnsClientGetDefaultConfig(instance);

    error = otDnsClientResolveAddress(instance, hostName, otapp_otDnsAddressCallback, NULL, config);
    if(error != OT_ERROR_NONE)
    {
        return OTAPP_DNS_ERROR;
        printf("Error: otDnsClientResolveAddress\n");
    }
    return OTAPP_DNS_OK;
}

void otapp_dnsClientBrowseResponseCallback(otError aError, const otDnsBrowseResponse *aResponse, void *aContext)
{
    if (aError == OT_ERROR_NONE)
    {        
        uint16_t index = 0;
        char *buffer = otapp_charBufGet_withMutex();

        if(buffer == NULL)
        {
            printf("ERROR NULL PTR FROM otapp_charBufGet_withMutex()");
            return;
        }
        
        otDnsBrowseResponseGetServiceName(aResponse, buffer, OTAPP_DNS_SRV_NAME_SIZE);    
        printf("DNS browse response for %s \n", buffer);        

        while (otDnsBrowseResponseGetServiceInstance(aResponse, index, buffer, OTAPP_DNS_SRV_LABEL_SIZE) == OT_ERROR_NONE)
        {
            if(index == OTAPP_PAIRED_DEVICES_MAX)
            {
                printf("OTAPP_PAIRED_DEVICES_MAX has been reached");
                otapp_charBufRelease();
                return;
            }

            printf("Device full name (label): %s \n", buffer);

            if(otapp_deviceNameFullAddDomain(buffer, OTAPP_CHAR_BUFFER_SIZE) != OTAPP_DEVICENAME_OK)
            {
                otapp_charBufRelease();
                return ;
            }
            otapp_dnsClientResolve(otapp_getOpenThreadInstancePtr(), buffer);
            
            printf("\n");
            index++;
        }
        otapp_charBufRelease();
    }
}

int8_t otapp_dnsClientBrowse(otInstance *instance, const char *serviceName)
{
    if(NULL == instance || NULL == serviceName)
    {
        return OTAPP_DNS_ERROR;
    }

    otError error = OT_ERROR_NONE; 
    static const otDnsQueryConfig *config;
    config = otDnsClientGetDefaultConfig(instance);

    error = otDnsClientBrowse(instance, serviceName, otapp_dnsClientBrowseResponseCallback, NULL, config);
    if (error != OT_ERROR_NONE)
    {
        printf("Error: DNS client browse: %d\n", error);
        return OTAPP_DNS_ERROR;
    }else
    {
        printf("DNS client browse. Checked reported services: \n");
    }

    return OTAPP_DNS_OK; 
}
