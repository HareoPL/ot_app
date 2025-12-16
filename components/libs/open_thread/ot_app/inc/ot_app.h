/**
 * @file ot_udp.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 14-07-2025
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
#ifndef THREAD_UDP_H_
#define THREAD_UDP_H_

// #include "esp_openthread.h"
#include "ot_app_coap.h"
#include "openthread/dns_client.h"
#include "openthread/dataset.h"

#define OTAPP_ERROR     (-1)


#define OTAPP_UDP_PORT 12345
#define OTAPP_CHAR_BUFFER_SIZE 1024 

#define OTAPP_PAIRED_DEVICES_MAX    10 // max number of devices to save them from DNS query
#define OTAPP_PAIRED_URI_MAX        OTAPP_COAP_URI_MAX 

#define OTAPP_DNS_LEASE_TASK_DELAY  300  // in secounds = 5m
#define OTAPP_DNS_LEASE_TIME        7200 // in secounds = 2h
#define OTAPP_DNS_LEASE_GUARD       (4 * OTAPP_DNS_LEASE_TASK_DELAY) // 20 min before end the time lease
#define OTAPP_DNS_M_KEY_LEASE_TIME  86400

#define OTAPP_DNS_SRV_NAME_SIZE     64 // OT_DNS_MAX_NAME_SIZE full service name: "_coap._udp.default.service.arpa." 
#define OTAPP_DNS_SRV_LABEL_SIZE    32 // OT_DNS_MAX_LABEL_SIZE host name: "device1"
#define OTAPP_DNS_SRV_TXT_SIZE      512
#define OTAPP_DEVICE_NAME_SIZE        OTAPP_DNS_SRV_LABEL_SIZE

typedef enum {
    OTAPP_CONTROL_PANEL = 1 ,
    OTAPP_SWITCH,
    OTAPP_LIGHTING,
    OTAPP_SENSOR,
    OTAPP_THERMOSTAT,
    OTAPP_DOOR_LOCK,
    OTAPP_MOTION_DETECTOR,
    OTAPP_REMOTE_CONTROL,
    OTAPP_ENERGY_METER,
    OTAPP_SMART_PLUG,
    OTAPP_ENVIRONMENT_SENSOR,
    OTAPP_DOOR_SENSOR,
    OTAPP_ALARM,
}otapp_deviceType_t;



void otapp_init(void);
void otapp_network_init();

otInstance *otapp_getOpenThreadInstancePtr(void);

const otIp6Address *otapp_getMulticastAddr(void);

const char *otapp_deviceNameFullGet(void);
void otapp_deviceNameSet(const char *deviceName, otapp_deviceType_t deviceType);
uint8_t otapp_deviceNameFullIsSame(const char *deviceNameFull);
uint8_t otapp_deviceNameIsSame(const char *deviceNameFull, uint16_t bufLength);

otapp_deviceType_t otapp_deviceNameConvertToDevId(const char *hostNameFull, uint16_t bufLength);

/**
 * @brief get PTR char buffer address and take MUTEX * 
 * @return char* PTR to char buffer * 
 * @note remember to release mutex by otapp_charBufRelease()
 * 
 */
char *otapp_charBufGet_withMutex(void);
void otapp_charBufRelease(void);

void otapp_printIp6Address(const otIp6Address *aAddress);

#endif  /* THREAD_UDP_H_ */


