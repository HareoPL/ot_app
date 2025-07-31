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
#include "ot_app_coap.h"

static const char *TAG = "ot_app";

otInstance *openThreadInstance;
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


void otapp_network_init() // this function will be initialize in ot_task_worker rtos task (esp_ot_cli.c)
{
    otapp_setDataset_tlv();
    // otapp_udpStart(); 
    otapp_coap_init();
}

void otapp_init(void) //app init
{
    openThreadInstance = esp_openthread_get_instance();
    otapp_cli_init();
}