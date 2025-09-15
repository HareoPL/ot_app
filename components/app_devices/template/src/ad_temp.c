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
#include "ot_app.h"
#include "ot_app_coap.h"
#include "ot_app_pair.h"
#include "ot_app_drv.h"

#define AD_TEMP_DEVICE_NAME ("device1")
static const otapp_deviceType_t ad_temp_deviceType = OTAPP_SWITCH;

// URI
static const char ad_temp_resourceContentTEST[] = 
    "</temp>;rt=\"sensor\","
    "</led>;rt=\"actuator\"";

void ad_temp_uri_well_knownCoreHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
{
    otapp_coap_printSenderIP(aMessageInfo);

    if (request)
    {
        otapp_coap_sendResponse(request, aMessageInfo, ad_temp_resourceContentTEST);
    }
}


static otapp_coap_uri_t ad_temp_uri[] ={
    {OTAPP_URI_WELL_KNOWN_CORE, {".well-known/core", ad_temp_uri_well_knownCoreHandle, NULL, NULL},},

};
#define AD_TEMP_URI_SIZE (sizeof(ad_temp_uri) / sizeof(ad_temp_uri[0]))

otapp_coap_uri_t *ad_temp_uriGetList()
{
    return ad_temp_uri;
}


// RULES
static otapp_pair_rule_t ad_temp_deviceRules[] = {
    {ad_temp_deviceType, {OTAPP_PAIR_NO_RULES}}, // NO RULES, pair every incoming device
};

// static otapp_pair_rule_t ad_temp_deviceRules[] = {
//     {ad_temp_deviceType, {OTAPP_LIGHTING_ON_OFF, OTAPP_LIGHTING_DIMM, OTAPP_LIGHTING_RGB}},   
// };

#define AD_TEMP_RULES_SIZE (sizeof(ad_temp_deviceRules) / sizeof(ad_temp_deviceRules[0]))

otapp_pair_rule_t *ad_temp_pairRulesGetList()
{
    return ad_temp_deviceRules;
}

//observer 

void ad_temp_pairedCallback(otapp_pair_Device_t *newDevice)
{
    printf("Dev Temp detect NEW DEVICE! %s \n", newDevice->devNameFull);
}

void ad_temp_subscribed_uris(otapp_pair_Device_t *newDevice)
{
    printf("Dev Temp from subs! %s \n", newDevice->devNameFull);
}

// drv
ot_app_devDrv_t switchDriver = {
    .pairRuleGetList = ad_temp_pairRulesGetList,
    .pairRuleGetListSize = AD_TEMP_RULES_SIZE,

    .uriGetList = ad_temp_uriGetList,
    .uriGetListSize = AD_TEMP_URI_SIZE,

    .obs_pairedDevice = ad_temp_pairedCallback,
    .obs_subscribedUri = ad_temp_subscribed_uris,

    .deviceName = AD_TEMP_DEVICE_NAME,
    .deviceType = &ad_temp_deviceType,
};


ot_app_devDrv_t *ad_devDrvGetTemp(void)
{
    return &switchDriver;
}