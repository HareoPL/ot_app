/**
 * @file ad_btn_uri.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief CoAP URI management implementation for button device
 * @version 0.1
 * @date 12-11-2025
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2025
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ad_btn_uri.h"

#define TAG "ad_btn_uri "

/**
 * @brief Static pointer to device driver structure
 * @note Set during ad_btn_uri_init(), used by URI handler
 */
static ot_app_devDrv_t *drv;

/**
 * @brief Buffer for CoAP message processing
 * @note Shared by URI handler to store incoming/outgoing CoAP payloads
 */
static uint8_t coapBuffer[OAC_URI_OBS_BUFFER_SIZE + 1];

/**
 * @brief Print human-readable description of URI processing result
 * @param[in] result Result code from processUriRequest()
 */
static void ad_button_uri_printResult(int8_t result);

/**
 * @brief CoAP handler for "btn/state" URI
 * @param[in] aContext Context pointer (unused)
 * @param[in] aMessage Pointer to received CoAP message
 * @param[in] aMessageInfo Pointer to message metadata (source address, port, etc.)
 * 
 * @details
 * Handles requests to the "btn/state" endpoint.
 * 
 * **Supported Modes:**
 * - **Observer Mode**: Remote devices can subscribe (CoAP Observe) to receive
 *   automatic notifications when button state changes
 * - **Direct Request**: Remote devices can query current button state (CoAP GET)
 * 
 * **Processing Flow:**
 * 1. Receives CoAP message via OpenThread
 * 2. Calls `processUriRequest()` to parse message and determine request type
 * 3. Handles three result categories:
 *    - Observer operations (subscribe/update/add)
 *    - Successful direct request (OTAPP_COAP_OK)
 *    - Error conditions
 * 
 * **Observer Operations**:
 * - New device subscription
 * - IP address updates for existing subscriber
 * - Token updates for existing subscriber
 * - URI list updates
 * 
 * @note This function is called by the CoAP framework when "btn/state" requests arrive
 * @see ad_button_uri_printResult() for detailed result interpretation
 */
void ad_button_uri_btn_state_CoreHandle(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    int8_t result = 0;
    result = drv->api.coap.processUriRequest(aMessage, aMessageInfo, OTAPP_SWITCH, coapBuffer, OAC_URI_OBS_BUFFER_SIZE);
    
    if(result > 0 || result == OAC_URI_OBS_ADDED_NEW_DEVICE || result == OAC_URI_OBS_NO_NEED_UPDATE)
    {
        // Observer mode processing (subscribe/update operations)
        OTAPP_PRINTF(TAG, "@ DEVICE URI btn_state: observer - processed \n");
        ad_button_uri_printResult(result);
    }
    else if(result == OTAPP_COAP_OK)
    {
        // Direct request mode - handle button state query
        OTAPP_PRINTF(TAG, "@ DEVICE URI btn_state payload: \n");
        OTAPP_PRINTF(TAG, "  %s \n", (char*)coapBuffer);
    }
    else
    {
        // Error occurred
        OTAPP_PRINTF(TAG, "@ DEVICE URI btn_state: error: %d \n", result);
    }
}

/**
 * @brief Static array of registered CoAP URIs and their handlers
 * 
 * @details
 * This array defines the CoAP endpoint supported by the button device.
 * 
 * **Array Structure:**
 * Each entry contains:
 * - **URI type identifier**: OTAPP_SWITCH (button/switch device type)
 * - **URI path string**: "btn/state" (the CoAP endpoint path)
 * - **Handler function pointer**: ad_button_uri_btn_state_CoreHandle
 * - **Additional callback pointers**: NULL (reserved for future use)
 * 
 * **Current Configuration:**
 * - Single endpoint: `btn/state`
 * - Type: OTAPP_SWITCH
 * - Handler: ad_button_uri_btn_state_CoreHandle()
 * 
 * @note Maximum URIs: OTAPP_PAIRED_URI_MAX
 * @note Maximum URI name length: OTAPP_COAP_URI_MAX_LENGHT
 * @see ad_btn_uri_getList_clb() returns pointer to this array
 */
static otapp_coap_uri_t ad_button_uri[] = {
    {OTAPP_SWITCH, {"btn/state", ad_button_uri_btn_state_CoreHandle, NULL, NULL},},
};

/**
 * @brief Calculate number of URIs in the array
 * @note Automatically adjusts when URIs are added/removed from ad_button_uri[]
 * @note Current value: 1 (single URI registered)
 */
#define AD_BUTTON_URI_SIZE (sizeof(ad_button_uri) / sizeof(ad_button_uri[0]))

/**
 * @brief Print human-readable description of URI processing result
 * @param[in] result Result code from processUriRequest()
 * 
 * @details
 * Interprets and prints the result of CoAP observer operations.
 * 
 * **Result Codes:**
 * - 1: IP address updated for existing subscriber
 * - 2: URI token updated for existing subscriber
 * - 3: Both IP address and token updated
 * - 4: New URI item added to subscriber's list
 * - 5: IP address updated and new URI item added
 * - OAC_URI_OBS_ADDED_NEW_DEVICE: New device added to subscriber list
 * - OAC_URI_OBS_NO_NEED_UPDATE: Subscriber data is current, no update needed
 */
static void ad_button_uri_printResult(int8_t result)
{
    switch(result)
    {
        case 1: // OAC_URI_OBS_UPDATE_IP_ADDR_Msk
            OTAPP_PRINTF(TAG, "  @--> update IP ADDR \n");
            break;
        case 2: // OAC_URI_OBS_UPDATE_URI_TOKEN_Msk
            OTAPP_PRINTF(TAG, "  @--> update TOKEN uri \n");
            break;
        case 3: // OAC_URI_OBS_UPDATE_IP_ADDR_Msk | OAC_URI_OBS_UPDATE_URI_TOKEN_Msk
            OTAPP_PRINTF(TAG, "  @--> update IP ADDR and TOKEN uri \n");
            break;
        case 4: // OAC_URI_OBS_ADD_NEW_URI_Msk
            OTAPP_PRINTF(TAG, "  @--> added new uri item \n");
            break;
        case 5: // OAC_URI_OBS_UPDATE_IP_ADDR_Msk | OAC_URI_OBS_ADD_NEW_URI_Msk
            OTAPP_PRINTF(TAG, "  @--> update IP ADDR and added new uri item \n");
            break;
        case OAC_URI_OBS_ADDED_NEW_DEVICE:
            OTAPP_PRINTF(TAG, "  @--> added new device to subscribe list \n");
            break;
        case OAC_URI_OBS_NO_NEED_UPDATE:
            OTAPP_PRINTF(TAG, "  @--> no need update subscriber data \n");
            break;
        default:
            break;
    }
}

otapp_coap_uri_t *ad_btn_uri_getList_clb(void)
{
    return ad_button_uri;
    // return NULL; // if uri list is empty
}

uint8_t ad_btn_uri_getListSize(void)
{
    return AD_BUTTON_URI_SIZE;
    // return 0; // if uri list is empty
}

void ad_btn_uri_init(ot_app_devDrv_t *devDrv)
{
    if(devDrv == NULL) return;
    drv = devDrv;
}
