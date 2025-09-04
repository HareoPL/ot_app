/**
 * @file ot_app_deviceName.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 26-08-2025
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
#include "ot_app_deviceName.h"
#include <string.h>
#include <stdlib.h>


static char otapp_deviceName[OTAPP_DNS_SRV_LABEL_SIZE]; // = "device1_1_588c81fffe301ea4"
static const char *otapp_deviceName_domain = ".default.service.arpa.";

int8_t otapp_deviceNameSet(const char *deviceName, otapp_deviceType_t deviceType)
{
    if(deviceName == NULL || deviceType >= OTAPP_DEVICENAME_MAX_DEVICE_TYPE || deviceType == OTAPP_NO_DEVICE_TYPE)
    {
        return OTAPP_DEVICENAME_ERROR;
    }

    if(strlen(deviceName) > OTAPP_DEVICENAME_SIZE)
    {
        return OTAPP_DEVICENAME_TOO_LONG;
    }

    const otExtAddress *factoryEUI_64 = otapp_macAddrGet();

    snprintf(otapp_deviceName, OTAPP_DNS_SRV_LABEL_SIZE - 1, "%s_%d_%02x%02x%02x%02x%02x%02x%02x%02x", 
                        deviceName, (int)deviceType,
                        factoryEUI_64->m8[0], factoryEUI_64->m8[1], factoryEUI_64->m8[2], factoryEUI_64->m8[3],
                        factoryEUI_64->m8[4], factoryEUI_64->m8[5], factoryEUI_64->m8[6], factoryEUI_64->m8[7]);
    
    return OTAPP_DEVICENAME_OK;
}

const char *otapp_deviceNameFullGet()
{
    return otapp_deviceName;
}

void otapp_deviceNameDelete()
{
    memset(otapp_deviceName, 0, OTAPP_DNS_SRV_LABEL_SIZE);
}

int8_t otapp_deviceNameFullIsSame(const char *deviceNameFull)
{
    if(deviceNameFull == NULL )
    {
        return OTAPP_DEVICENAME_ERROR;
    }

    if(strlen(deviceNameFull) >= OTAPP_DEVICENAME_FULL_SIZE)
    {
        return OTAPP_DEVICENAME_TOO_LONG;
    }

    if(strcmp(deviceNameFull, otapp_deviceNameFullGet()) == 0)
    {
        return OTAPP_DEVICENAME_IS;
    }
    return OTAPP_DEVICENAME_IS_NOT;
}

int8_t otapp_deviceNameIsSame(const char *deviceNameFull, uint8_t stringLength)
{
    if(deviceNameFull == NULL )
    {
        return OTAPP_DEVICENAME_ERROR;
    }

    if(stringLength >= OTAPP_DEVICENAME_FULL_SIZE)
    {
        return OTAPP_DEVICENAME_TOO_LONG;
    }

    char inDeviceName[OTAPP_DEVICENAME_FULL_SIZE];
    char curDeviceName[OTAPP_DEVICENAME_FULL_SIZE];

    strncpy(inDeviceName, deviceNameFull, stringLength);
    strncpy(curDeviceName, otapp_deviceNameFullGet(), stringLength);
    
    strtok(inDeviceName, "_");
    strtok(curDeviceName, "_");

    if(strcmp(inDeviceName, curDeviceName) == 0)
    {
        return OTAPP_DEVICENAME_IS;
    }
    return OTAPP_DEVICENAME_IS_NOT;
}

otapp_deviceType_t otapp_deviceNameGetDevId(const char *deviceNameFull, uint8_t stringLength)
{
    if(deviceNameFull == NULL )
    {
        return OTAPP_DEVICENAME_ERROR;
    }

    if(stringLength >= OTAPP_DEVICENAME_FULL_SIZE)
    {
        return OTAPP_DEVICENAME_TOO_LONG;
    }

    char buf[OTAPP_DEVICENAME_FULL_SIZE];
    char *ptr;
    strncpy(buf, deviceNameFull, stringLength);
    
    strtok(buf, "_");
    ptr = strtok(NULL, "_");

    return atoi(ptr);
}

int8_t otapp_deviceNameFullAddDomain(char *deviceFullName, uint16_t bufLength)
{
    if(deviceFullName == NULL)
    {
        return OTAPP_DEVICENAME_ERROR;
    }

    if(strlen(deviceFullName) >= OTAPP_DEVICENAME_FULL_SIZE)
    {
        return OTAPP_DEVICENAME_TOO_LONG;
    }

    if(bufLength < (2 * OTAPP_DEVICENAME_FULL_SIZE))
    {
        return OTAPP_DEVICENAME_BUFFER_TOO_SMALL;
    }

    strcat(deviceFullName, otapp_deviceName_domain);
    
    return OTAPP_DEVICENAME_OK;
}

int8_t otapp_hostNameToDeviceNameFull(char *hostName)
{
    if(hostName == NULL )
    {
        return OTAPP_DEVICENAME_ERROR;
    }

    char *chrPtr;
    chrPtr = strtok(hostName, ".");

    if(chrPtr == NULL)
    {
        return OTAPP_DEVICENAME_ERROR;
    }

    return OTAPP_DEVICENAME_OK;
}

int8_t otapp_deviceNameIsMatching(char *deviceFullName)
{
     if(deviceFullName == NULL)
    {
        return OTAPP_DEVICENAME_ERROR;
    }

    if(otapp_deviceNameFullIsSame(deviceFullName) == OTAPP_DEVICENAME_IS_NOT) 
    {
        const char *thisDeviceNameFull = otapp_deviceNameFullGet();
        uint16_t thisSizeOfDevName = strlen(thisDeviceNameFull);

        if(otapp_deviceNameIsSame(deviceFullName, strlen(deviceFullName)) == OTAPP_DEVICENAME_IS || otapp_deviceNameGetDevId(thisDeviceNameFull, thisSizeOfDevName) == OTAPP_CONTROL_PANEL) // if This device is configured as control pannel, it should known all devices. it is center control device
        {
            // todo warunek parowania: jesli current device to switch a incomming device to light to parujemy. 
            // najlepiej takie warunki zrobic w osobnej funkcji gdzie latwo bedzie mozna dodac liste warunkow. 
           
            return OTAPP_DEVICENAME_IS;
        }
        else
        {
            return OTAPP_DEVICENAME_IS_NOT;
        }               
    }

    return OTAPP_DEVICENAME_IS_NOT;
}