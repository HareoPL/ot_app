/**
 * @file ad_temp.c
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


#include "ad_temp.h"
#include "string.h"
#include "ot_app_drv.h"

#define AD_TEMP_DEVICE_NAME ("device1")
static const otapp_deviceType_t ad_temp_deviceType = OTAPP_LIGHTING;

static ot_app_devDrv_t *drv;
// URI


void ad_temp_uri_light_on_off_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    printf("@@@@@@@@@@@@@@@ FROM light_on_off \n");
    otapp_coap_sendResponse(aMessage, aMessageInfo, (uint8_t*)otapp_coap_getMessage(OTAPP_MESSAGE_TEST), strlen(otapp_coap_getMessage(OTAPP_MESSAGE_TEST)) );
}

void ad_temp_uri_light_dimm_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    printf("@@@@@@@@@@@@@@@ FROM light_dimm\n");
    otapp_coap_sendResponse(aMessage, aMessageInfo, (uint8_t*)otapp_coap_getMessage(OTAPP_MESSAGE_TEST), strlen(otapp_coap_getMessage(OTAPP_MESSAGE_TEST)) );
}

void ad_temp_uri_light_rgb_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    printf("@@@@@@@@@@@@@@@ FROM light_rgb\n");
    otapp_coap_sendResponse(aMessage, aMessageInfo, (uint8_t*)otapp_coap_getMessage(OTAPP_MESSAGE_TEST), strlen(otapp_coap_getMessage(OTAPP_MESSAGE_TEST)) );
}

// max uris:            OTAPP_PAIRED_URI_MAX
// max lengh uri name:  OTAPP_COAP_URI_MAX_LENGHT
static otapp_coap_uri_t ad_temp_uri[] = {   
    {OTAPP_LIGHTING_ON_OFF,  {"light/on_off", ad_temp_uri_light_on_off_CoreHandle, NULL, NULL},},
    {OTAPP_LIGHTING_DIMM,    {"light/dimm", ad_temp_uri_light_dimm_CoreHandle, NULL, NULL},},
    {OTAPP_LIGHTING_RGB,     {"light/rgb", ad_temp_uri_light_rgb_CoreHandle, NULL, NULL},},
 
};
#define AD_TEMP_URI_SIZE (sizeof(ad_temp_uri) / sizeof(ad_temp_uri[0]))

otapp_coap_uri_t *ad_temp_uriGetList()
{
    return ad_temp_uri;
}

//////////////////////
// RULES
static otapp_pair_rule_t ad_temp_deviceRules_all_allowed = {
    .allowed = {OTAPP_PAIR_NO_RULES} // NO RULES, pair every incoming device
};
static otapp_pair_rule_t ad_temp_deviceRules_no_allowed = {
    .allowed = {OTAPP_PAIR_NO_ALLOWED} // every devices are not allowed
};
static otapp_pair_rule_t ad_temp_deviceRules = {
    .allowed = {OTAPP_LIGHTING, OTAPP_LIGHTING_ON_OFF, OTAPP_LIGHTING_DIMM, OTAPP_LIGHTING_RGB}   
};

otapp_pair_rule_t *ad_temp_pairRulesGetList_all_allowed()
{
    return &ad_temp_deviceRules_all_allowed;
}

otapp_pair_rule_t *ad_temp_pairRulesGetList_no_allowed()
{
    return &ad_temp_deviceRules_no_allowed;
}

otapp_pair_rule_t *ad_temp_pairRulesGetList()
{
    return &ad_temp_deviceRules;
}

//////////////////////
//observer 
void testHandleGetByte(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aResult)
{

}

void ad_temp_pairedCallback(otapp_pair_Device_t *newDevice)
{
    printf("\n");
    printf("Dev Temp detect NEW DEVICE! %s \n", newDevice->devNameFull);
    printf("      uri 0: %s\n", newDevice->urisList[0].uri);
    printf("      uri 1: %s\n", newDevice->urisList[1].uri);
    printf("      uri 2: %s\n", newDevice->urisList[2].uri);
    printf("\n");
    drv->api.coap.sendByteGet(&newDevice->ipAddr, newDevice->urisList[0].uri, testHandleGetByte, NULL);
    drv->api.coap.sendByteGet(&newDevice->ipAddr, newDevice->urisList[1].uri, testHandleGetByte, NULL);
    drv->api.coap.sendByteGet(&newDevice->ipAddr, newDevice->urisList[2].uri, testHandleGetByte, NULL);
}

void ad_temp_subscribedUrisCallback(oac_uri_dataPacket_t *newDevice)
{
    printf("Dev Temp from subs! \n");
}

//////////////////////
// init
void ad_tempInit()
{    
    drv = ot_app_drv_getInstance();

    drv->pairRuleGetList_clb = ad_temp_pairRulesGetList_all_allowed;     // if you want to pair all devices
    // drv->pairRuleGetList_clb = ad_temp_pairRulesGetList_no_allowed;      // if you do not want to pair devices
    // drv->pairRuleGetList_clb = ad_temp_pairRulesGetList;                    // if you have some rules
    
    drv->uriGetList_clb = ad_temp_uriGetList;
    drv->uriGetListSize = AD_TEMP_URI_SIZE;

    drv->obs_pairedDevice_clb = ad_temp_pairedCallback;
    drv->obs_subscribedUri_clb = ad_temp_subscribedUrisCallback;

    drv->deviceName = AD_TEMP_DEVICE_NAME;
    drv->deviceType = &ad_temp_deviceType;
}
