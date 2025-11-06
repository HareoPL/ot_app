/**
 * @file ad_button.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-09-2025
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


#include "ad_button.h"
#include "ad_btn_assign.h"
#include "ad_btn_uri.h"

#include "string.h"
#include "ot_app_drv.h"

#define AD_BUTTON_DEVICE_NAME       ("device1")

static const otapp_deviceType_t ad_button_deviceType = OTAPP_SWITCH;
static ot_app_devDrv_t *drv;

//////////////////////
// RULES
// OTAPP_PAIR_RULES_ALLOWED_SIZE = 10
static otapp_pair_rule_t ad_button_deviceRules_all_allowed = {
    .allowed = {OTAPP_PAIR_NO_RULES, OTAPP_PAIR_END_OF_RULES} // NO RULES, pair every incoming device
};
static otapp_pair_rule_t ad_button_deviceRules_no_allowed = {
    .allowed = {OTAPP_PAIR_NO_ALLOWED, OTAPP_PAIR_END_OF_RULES} // every devices are not allowed
};
static otapp_pair_rule_t ad_button_deviceRules = {
    .allowed = {OTAPP_LIGHTING, OTAPP_LIGHTING_ON_OFF, OTAPP_LIGHTING_DIMM, OTAPP_LIGHTING_RGB, OTAPP_PAIR_END_OF_RULES}   
};

otapp_pair_rule_t *ad_button_pairRulesGetList_all_allowed()
{
    return &ad_button_deviceRules_all_allowed;
}

otapp_pair_rule_t *ad_button_pairRulesGetList_no_allowed()
{
    return &ad_button_deviceRules_no_allowed;
}

otapp_pair_rule_t *ad_button_pairRulesGetList()
{
    return &ad_button_deviceRules;
}

//////////////////////
//observer 

// Here you will see a correctly added device to the paired list with the subscribed device uri, e.g. light
void ad_button_pairedCallback(otapp_pair_Device_t *device)
{        
    ad_btn_assignDevice(device);

    printf("\n");
    printf("Dev Button detect DEVICE! %s \n", device->devNameFull);
    printf("      uri 0: %s\n", device->urisList[0].uri);
    printf("      uri 1: %s\n", device->urisList[1].uri);
    printf("      uri 2: %s\n", device->urisList[2].uri);
    printf("\n");
}

// Here you will see an update of the status of the subscribed device, e.g. Light
void ad_button_subscribedUrisCallback(oac_uri_dataPacket_t *data)  
{    
    uint32_t uriState_ = 0;
    uriState_ |= (uint32_t)data->buffer[0];
    uriState_ |= ((uint32_t)data->buffer[1] << 8);
    uriState_ |= ((uint32_t)data->buffer[2] << 16);
    uriState_ |= ((uint32_t)data->buffer[3] << 24);

    otapp_pair_DeviceList_t *pairHandle = drv->api.pair.getHandle();
    drv->api.pair.uriStateSet(pairHandle, data->token, &uriState_);
    
    printf("@ Dev button from subs: \n");
    printf(" @--> token: 0x%x 0x%x 0x%x 0x%x\n", data->token[0], data->token[1], data->token[2], data->token[3]);
    printf(" @--> data: %ld\n", uriState_);
}

void ad_button_task()
{
    ad_btn_task();
}

//////////////////////
// init
void ad_button_Init()
{    
    drv = ot_app_drv_getInstance();

    if(drv->api.nvs.init == NULL) return ;
    drv->api.nvs.init();

    ad_btn_init(drv);
    ad_btn_uri_init(drv);

    // drv->pairRuleGetList_clb = ad_button_pairRulesGetList_all_allowed;     // if you want to pair all devices
    // drv->pairRuleGetList_clb = ad_button_pairRulesGetList_no_allowed;      // if you do not want to pair devices
    drv->pairRuleGetList_clb = ad_button_pairRulesGetList;                    // if you have some rules
    
    drv->uriGetList_clb = ad_btn_uri_getList_clb;
    drv->uriGetListSize = ad_btn_uri_getListSize();

    drv->obs_pairedDevice_clb = ad_button_pairedCallback;
    drv->obs_subscribedUri_clb = ad_button_subscribedUrisCallback;

    drv->deviceName = AD_BUTTON_DEVICE_NAME;
    drv->deviceType = &ad_button_deviceType;
    drv->task = ad_button_task;
}

