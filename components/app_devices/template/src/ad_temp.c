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

#include "ot_app_drv.h"

#define AD_TEMP_DEVICE_NAME ("device1")
static const otapp_deviceType_t ad_temp_deviceType = OTAPP_SWITCH;

static ot_app_devDrv_t *drv;
// URI

typedef enum {
        AD_TEMP_NO_URI_INDEX = 0,

        AD_TEMP_LIGHT_ON_OFF,
        AD_TEMP_LIGHT_DIMM,
        AD_TEMP_LIGHT_RGB,

        AD_TEMP_END_OF_INDEX,
    }ad_temp_uriIndex_t;


/* Common Web Linking Parameters in CoRE Link Format (RFC 6690):
 *
 * rt       - Resource Type: Describes the semantic type of the resource.
 *            Example values: "temperature-c", "sensor"
 *
 * if       - Interface Description: Describes the interface or interaction method.
 *            Example values: "on,off,set-color"
 *
 * title    - Human-readable title to describe the resource.
 *            Example: "Outdoor temperature sensor"
 *
 * ct       - Content Type: Media type of the resource representation (Content-Format in CoAP).
 *            Example: 50 (JSON), 0 (plain text)
 *
 * sz       - Size: Size of the resource in bytes.
 *            Example: 123
 *
 * obs      - Observable: Indicates that the resource supports observation (Observe feature).
 *            Typically used as a flag with no value.
 * otCoapOptionContentFormat
 */
static const char ad_temp_uriResources[] =  // format: Web Linking RFC 6690
    "</light/on_off>;rt=\"actuator\";if=\"on,off\";title=\"Light on-off\";ct=0;obs,"                      // OT_COAP_OPTION_CONTENT_FORMAT_TEXT_PLAIN
    "</light/dimm>;rt=\"actuator\";if=\"on,off,dimm\";title=\"Light dimmable\";ct=50;obs;sz=4,"      // OT_COAP_OPTION_CONTENT_FORMAT_JSON
    "</light/rgb>;rt=\"actuator\";if=\"on,off,set-color\";title=\"Light RGB\";ct=50;obs;sz=4";     // OT_COAP_OPTION_CONTENT_FORMAT_JSON
   
void ad_temp_uri_light_on_off_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{

}

void ad_temp_uri_light_dimm_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{

}

void ad_temp_uri_light_rgb_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{

}

static otapp_coap_uri_t ad_temp_uri[] ={
    {AD_TEMP_LIGHT_ON_OFF,  {"light/on_off", ad_temp_uri_light_on_off_CoreHandle, NULL, NULL},},
    {AD_TEMP_LIGHT_DIMM,    {"light/dimm", ad_temp_uri_light_dimm_CoreHandle, NULL, NULL},},
    {AD_TEMP_LIGHT_RGB,     {"light/dimm", ad_temp_uri_light_rgb_CoreHandle, NULL, NULL},},
 
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

void ad_temp_subscribedUrisCallback(oac_uri_dataPacket_t *newDevice)
{
    printf("Dev Temp from subs! \n");
}


void ad_tempInit()
{    
    drv = ot_app_drv_getInstance();
    drv->pairRuleGetList = ad_temp_pairRulesGetList;
    drv->pairRuleGetListSize = AD_TEMP_RULES_SIZE;

    drv->uriGetList = ad_temp_uriGetList;
    drv->uriGetListSize = AD_TEMP_URI_SIZE;

    drv->obs_pairedDevice = ad_temp_pairedCallback;
    drv->obs_subscribedUri = ad_temp_subscribedUrisCallback;

    drv->deviceName = AD_TEMP_DEVICE_NAME;
    drv->deviceType = &ad_temp_deviceType;
    drv->uriResources = ad_temp_uriResources;

}
