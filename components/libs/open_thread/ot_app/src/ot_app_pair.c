/**
 * @file ot_app_pair.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 19-08-2025
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

 #include "ot_app_pair.h"

typedef struct {
    char devName[OTAPP_PAIR_NAME_SIZE]; // deviceNameFull
    otIp6Address ipAddr;
    uint8_t uriIndex[OTAPP_PAIR_URI_MAX];    
}otapp_pair_Device_t;

struct otapp_pair_DeviceList_t{
    otapp_pair_Device_t list[OTAPP_PAIR_DEVICES_MAX];
    uint8_t takenPosition[OTAPP_PAIR_DEVICES_MAX];
};

PRIVATE otapp_pair_DeviceList_t otapp_pair_DeviceList;

PRIVATE int8_t otapp_pair_DeviceIsFreeSpace(otapp_pair_DeviceList_t *pairDeviceList)
{
    if(pairDeviceList == NULL)
    {
        return OTAPP_PAIR_ERROR;
    } 

     for (uint8_t i = 0; i < OTAPP_PAIR_DEVICES_MAX; i++)
     {
        if(pairDeviceList->takenPosition[i] == 0)
        {
           return i;
        }
     }
     return OTAPP_PAIR_ERROR; 
}

PRIVATE void otapp_pair_spaceTake(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice)
{
    if(pairDeviceList == NULL || indexDevice >= OTAPP_PAIR_DEVICES_MAX)
    {
        return;
    } 

    pairDeviceList->takenPosition[indexDevice] = 1;
}

PRIVATE int8_t otapp_pair_spaceIsTaken(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice)
{ 
    if(pairDeviceList == NULL || indexDevice >= OTAPP_PAIR_DEVICES_MAX)
    {
        return OTAPP_PAIR_ERROR;
    }

    return (pairDeviceList->takenPosition[indexDevice]);
}

PRIVATE int8_t otapp_pair_deviceNameIsSame(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull, uint8_t indexDevice)
{
    if(pairDeviceList == NULL || deviceNameFull == NULL || indexDevice >= OTAPP_PAIR_DEVICES_MAX)
    {
        return OTAPP_PAIR_ERROR;
    }

    if(strcmp(deviceNameFull, pairDeviceList->list[indexDevice].devName) == 0)
    {
        return 1;
    }
    return 0;
}

PRIVATE int8_t otapp_pair_DeviceIsExist(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull)
{
    if(pairDeviceList == NULL || deviceNameFull == NULL)
    {
        return OTAPP_PAIR_ERROR;
    }

    for (int8_t i = 0; i < OTAPP_PAIR_DEVICES_MAX; i++)
    {
        if(otapp_pair_spaceIsTaken(pairDeviceList, i))
        {
            if(otapp_pair_deviceNameIsSame(pairDeviceList, deviceNameFull, i))
            {
                return i;
            }
        }
    }        

    return OTAPP_PAIR_NO_EXIST;
}

char *otapp_pair_DeviceNameGet(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice)
{
    if(pairDeviceList == NULL || indexDevice >= OTAPP_PAIR_DEVICES_MAX)
    {
        return NULL;
    }
    
    if(otapp_pair_spaceIsTaken(pairDeviceList, indexDevice) == 0)
    {
        return NULL;
    }

    return pairDeviceList->list[indexDevice].devName;
}

otapp_pair_DeviceList_t *otapp_pair_getHandle(void)
{
    return &otapp_pair_DeviceList;
}

int8_t otapp_pair_DeviceDelete(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull)
{
    if(pairDeviceList == NULL || deviceNameFull == NULL)
    {
        return OTAPP_PAIR_ERROR;
    }

    int8_t tableIndex = otapp_pair_DeviceIsExist(pairDeviceList, deviceNameFull);
    if(tableIndex >= 0)
    {
        memset(pairDeviceList->list[tableIndex].devName, 0, OTAPP_PAIR_NAME_SIZE);
        memset(&pairDeviceList->list[tableIndex].ipAddr, 0, sizeof(otIp6Address));
        memset(pairDeviceList->list[tableIndex].uriIndex, 0, OTAPP_PAIR_URI_MAX);
        pairDeviceList->takenPosition[tableIndex] = 0;
        
        return tableIndex;
    }
    return OTAPP_PAIR_NO_EXIST;
}

int8_t otapp_pair_DeviceDeleteAll(otapp_pair_DeviceList_t *pairDeviceList)
{
    if(pairDeviceList == NULL)
    {
        return OTAPP_PAIR_ERROR;
    }

    for (uint8_t i = 0; i < OTAPP_PAIR_DEVICES_MAX; i++)
    {
        memset(pairDeviceList->list[i].devName, 0, OTAPP_PAIR_NAME_SIZE);
        memset(&pairDeviceList->list[i].ipAddr, 0, sizeof(otIp6Address));
        pairDeviceList->takenPosition[i] = 0;
                
        memset(pairDeviceList->list[i].uriIndex, 0, OTAPP_PAIR_URI_MAX);  
    }

    return OTAPP_PAIR_OK;
}

int8_t otapp_pair_DeviceIndexGet(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull)
{
    if(pairDeviceList == NULL || deviceNameFull == NULL)
    {
        return OTAPP_PAIR_ERROR;
    }

    int8_t deviceIndex;
    
    deviceIndex = otapp_pair_DeviceIsExist(pairDeviceList, deviceNameFull);
    if(deviceIndex == OTAPP_PAIR_NO_EXIST)
    {
        return OTAPP_PAIR_NO_EXIST;
    }

    return deviceIndex;
}

int8_t otapp_pair_DeviceAdd(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull, otIp6Address *ipAddr)
{
    if(pairDeviceList == NULL || deviceNameFull == NULL || ipAddr == NULL)
    {
        return OTAPP_PAIR_ERROR;
    }

    int8_t tableIndex;
    uint16_t strLen = 0;

    if(otapp_pair_DeviceIsExist(pairDeviceList, deviceNameFull) == OTAPP_PAIR_NO_EXIST)
    {
       tableIndex = otapp_pair_DeviceIsFreeSpace(pairDeviceList);
       
       if(tableIndex != OTAPP_PAIR_ERROR)
       { 
            strLen = strlen(deviceNameFull); 
            if(strLen >= OTAPP_DNS_SRV_LABEL_SIZE)
            {
                return OTAPP_PAIR_DEVICE_NAME_TO_LONG;
            }
            strncpy(pairDeviceList->list[tableIndex].devName, deviceNameFull, strLen);
            memcpy(&pairDeviceList->list[tableIndex].ipAddr, ipAddr, sizeof(otIp6Address)); 

            otapp_pair_spaceTake(pairDeviceList, tableIndex);

            return tableIndex;    
       }
       else
       {
            return OTAPP_PAIR_DEVICE_NO_SPACE;
       }
    }
   
    return OTAPP_PAIR_DEVICE_NAME_EXIST;
}

int16_t otapp_pair_DeviceUriIndexAdd(otapp_pair_DeviceList_t *pairDeviceList, const char *deviceNameFull, otapp_coap_uriTableIndex_t uriIndex)
{
    if(pairDeviceList == NULL || deviceNameFull == NULL || uriIndex >= (OTAPP_PAIR_URI_MAX_VAL) || uriIndex == OTAPP_PAIR_NO_URI)
    {
        return OTAPP_PAIR_ERROR;
    }

    int8_t tableIndex = otapp_pair_DeviceIsExist(pairDeviceList, deviceNameFull);
    if(tableIndex != OTAPP_PAIR_ERROR)
    {
        for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX; i++)
        {
            if(pairDeviceList->list[tableIndex].uriIndex[i] == OTAPP_PAIR_NO_URI)
            {
                pairDeviceList->list[tableIndex].uriIndex[i] = uriIndex;
                return ((tableIndex << 8) | (i));
            }
        }
    }

    return OTAPP_PAIR_ERROR;
}

otapp_coap_uriTableIndex_t otapp_pair_deviceUriIndexGet(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice, uint8_t indexUri)
{
    otapp_coap_uriTableIndex_t uriIndex;

    if(pairDeviceList == NULL || indexDevice >= OTAPP_PAIR_DEVICES_MAX || indexUri >= OTAPP_PAIR_URI_MAX)
    {
        return OTAPP_PAIR_ERROR;
    }

    if(otapp_pair_spaceIsTaken(pairDeviceList, indexDevice) == 0)
    {
        return OTAPP_PAIR_NO_EXIST;
    }

    uriIndex = pairDeviceList->list[indexDevice].uriIndex[indexUri];

    if(uriIndex == OTAPP_PAIR_NO_URI)
    {
        return OTAPP_PAIR_NO_URI;
    }
    return uriIndex;
}


otIp6Address *otapp_pair_ipAddressGet(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice)
{
    if(pairDeviceList == NULL || indexDevice >= OTAPP_PAIR_DEVICES_MAX)
    {
        return NULL;
    }

    return &pairDeviceList->list[indexDevice].ipAddr;
}
// otapp_pair_ipAddressCompare
// otapp_pair_ipAddressUpdate


void otapp_pair_devicePrintData(otapp_pair_DeviceList_t *pairDeviceList, uint8_t indexDevice)
{
    if(pairDeviceList == NULL || indexDevice >= OTAPP_PAIR_DEVICES_MAX)
    {
        return  ;
    }
    
    char *deviceName;
    otIp6Address *ipAddr;
    const char *uriName;
    otapp_coap_uriTableIndex_t uriIndex = OTAPP_PAIR_URI_INIT;

    deviceName = otapp_pair_DeviceNameGet(pairDeviceList, indexDevice);
    ipAddr = otapp_pair_ipAddressGet(pairDeviceList, indexDevice);

    printf("Paired device info: \n");
    printf("  name: %s \n", deviceName);
    printf("  IP: ");
    otapp_printIp6Address(ipAddr);
    printf("  URI: \n");

    for (uint8_t i = 0; i < OTAPP_PAIR_URI_MAX; i++)
    {
        uriIndex = otapp_pair_deviceUriIndexGet(pairDeviceList, indexDevice, i);

        if(uriIndex == OTAPP_PAIR_NO_URI)
        {
            if(i == 0)
            {
                printf("    no URI \n");
            }
            return;
        }

        uriName = otapp_coap_getUriName(uriIndex);
        printf("        %s\n", uriName);
    }
}

