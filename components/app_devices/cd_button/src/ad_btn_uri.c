/**
 * @file ad_btn_uri.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 12-11-2025
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
#include "ad_btn_uri.h"

static ot_app_devDrv_t *drv;

// static uint8_t coapBuffer[OAC_URI_OBS_BUFFER_SIZE + 1];

// static void ad_button_uri_printResult(int8_t result);

// void ad_button_uri_light_on_off_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
// {
//     int8_t result = 0;    
//     result = drv->api.coap.processUriRequest(aMessage, aMessageInfo, OTAPP_LIGHTING_ON_OFF, coapBuffer, OAC_URI_OBS_BUFFER_SIZE);

//     if(result > 0 || result == OAC_URI_OBS_ADDED_NEW_DEVICE || result == OAC_URI_OBS_NO_NEED_UPDATE)
//     {
//         printf("@ DEVICE URI light_on_off: observer - processed \n");
//         ad_button_uri_printResult(result);        

//     }else if(result == OTAPP_COAP_OK)
//     {
//         // handle uri request here

//         printf("@ DEVICE URI light_on_off payload: \n");
//         printf("   %s \n", (char*)coapBuffer);
//     }else
//     {
//          printf("@ DEVICE URI light_on_off: error: %d \n", result);
//     }    
// }

// void ad_button_uri_light_dimm_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
// {
//     int8_t result = 0;    
//     result = drv->api.coap.processUriRequest(aMessage, aMessageInfo, OTAPP_LIGHTING_DIMM, coapBuffer, OAC_URI_OBS_BUFFER_SIZE);

//     if(result > 0 || result == OAC_URI_OBS_ADDED_NEW_DEVICE || result == OAC_URI_OBS_NO_NEED_UPDATE)
//     {
//         printf("@ DEVICE URI light_dimm: observer - processed \n");
//         ad_button_uri_printResult(result);  
//     }else if(result == OTAPP_COAP_OK)
//     {
//         // handle uri request here

//         printf("@ DEVICE URI light_dimm payload: \n");
//         printf("   %s \n", (char*)coapBuffer);
//     }else
//     {
//          printf("@ DEVICE URI light_dimm: error: %d \n", result);
//     }      
// }

// void ad_button_uri_light_rgb_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
// {
//     int8_t result = 0;    
//     result = drv->api.coap.processUriRequest(aMessage, aMessageInfo, OTAPP_LIGHTING_RGB, coapBuffer, OAC_URI_OBS_BUFFER_SIZE);

//     if(result > 0 || result == OAC_URI_OBS_ADDED_NEW_DEVICE || result == OAC_URI_OBS_NO_NEED_UPDATE)
//     {
//         printf("@ DEVICE URI light_rgb: observer - processed \n");
//         ad_button_uri_printResult(result);  

//     }else if(result == OTAPP_COAP_OK)
//     {
//         // handle uri request here

//         printf("@ DEVICE URI light_rgb payload: \n");
//         printf("   %s \n", (char*)coapBuffer);
//     }else
//     {
//          printf("@ DEVICE URI light_rgb: error: %d \n", result);
//     }  
// }

// max uris:            OTAPP_PAIRED_URI_MAX
// max lengh uri name:  OTAPP_COAP_URI_MAX_LENGHT
// static otapp_coap_uri_t ad_button_uri[] = {   
//     {OTAPP_LIGHTING_ON_OFF,  {"light/on_off", ad_button_uri_light_on_off_CoreHandle, NULL, NULL},},
//     {OTAPP_LIGHTING_DIMM,    {"light/dimm", ad_button_uri_light_dimm_CoreHandle, NULL, NULL},},
//     {OTAPP_LIGHTING_RGB,     {"light/rgb", ad_button_uri_light_rgb_CoreHandle, NULL, NULL},},
 
// };
// #define AD_BUTTON_URI_SIZE (sizeof(ad_button_uri) / sizeof(ad_button_uri[0]))

// static void ad_button_uri_printResult(int8_t result)
// {
//     switch(result)
//     {
//     case 1: // OAC_URI_OBS_UPDATE_IP_ADDR_Msk
//         printf(" @--> update IP ADDR \n");
//         break;
        
//     case 2: // OAC_URI_OBS_UPDATE_URI_TOKEN_Msk
//         printf(" @--> update TOKEN uri \n");
//         break;

//     case 3: // OAC_URI_OBS_UPDATE_IP_ADDR_Msk OAC_URI_OBS_UPDATE_URI_TOKEN_Msk
//         printf(" @--> update IP ADDR and TOKEN uri \n");
//         break;

//     case 4: // OAC_URI_OBS_ADD_NEW_URI_Msk
//         printf(" @--> added new uri item \n");
//         break;

//     case 5: // OAC_URI_OBS_UPDATE_IP_ADDR_Msk OAC_URI_OBS_ADD_NEW_URI_Msk
//         printf(" @--> update IP ADDR and added new uri item \n");
//         break;
//     case OAC_URI_OBS_ADDED_NEW_DEVICE: 
//         printf(" @--> added new device to subscribe list \n");
//         break;
//     case OAC_URI_OBS_NO_NEED_UPDATE: 
//         printf(" @--> no need update subscriber data \n");
//         break;

//     default:
//         break;
//     }
// }

otapp_coap_uri_t *ad_btn_uri_getList_clb(void)
{
    // return ad_button_uri;
    return NULL;
}

uint8_t ad_btn_uri_getListSize(void)
{
    // return ad_button_uri;
    return 0;
}

void ad_btn_uri_init(ot_app_devDrv_t *devDrv)
{
    if(devDrv == NULL) return;

    drv = devDrv;
}

