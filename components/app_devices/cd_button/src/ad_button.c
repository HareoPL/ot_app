/**
 * @file ad_button.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Button device implementation for OpenThread application
 * @version 0.1
 * @date 06-09-2025
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
 * 
 */

#include "ad_button.h"
#include "ad_btn_assign.h"
#include "ad_btn_uri.h"

#include "string.h"
#include "ot_app_drv.h"

/**
 * @brief Device name group buffer
 * 
 * @details
 * Internal buffer storing the device name group identifier used for network pairing.
 * 
 * **Buffer Properties:**
 * - Size: OTAPP_DEVICENAME_SIZE (10 bytes)
 * - Maximum string length: 9 characters + null terminator
 * - Initialized by ad_button_Init(char *deviceNameGroup)
 * - Used by OpenThread framework for device discovery and pairing
 * 
 * **Purpose:**
 * - Identifies this button device on the OpenThread network
 * - Filters pairing: only devices with matching name group can pair
 * - Enables logical grouping of devices (e.g., by room or function)
 * 
 * @note This buffer persists for the lifetime of the application
 * @note Content is set once during initialization via strcpy()
 * @note Buffer is assigned to drv->deviceName during ad_button_Init()
 * 
 * @see ad_button_Init() for initialization
 * @see OTAPP_DEVICENAME_SIZE for buffer size definition
 */
static char ad_button_deviceNameTab[OTAPP_DEVICENAME_SIZE];

/**
 * @brief Device type identifier
 * @note Declared as OTAPP_SWITCH to indicate this is a switch/button control device
 */
static const otapp_deviceType_t ad_button_deviceType = OTAPP_SWITCH;

/**
 * @brief Static pointer to device driver instance
 * @note Set during ad_button_Init() and used throughout the module
 */
static ot_app_devDrv_t *drv;

//////////////////////
// PAIRING RULES
//////////////////////

/**
 * @brief Pairing rule set that allows all devices without restrictions
 * 
 * @details
 * This rule set permits pairing with any OpenThread device regardless of type.
 * Useful for development, testing, or flexible deployment scenarios.
 * 
 * Configuration:
 * - OTAPP_PAIR_NO_RULES: Special flag indicating no filtering
 * - OTAPP_PAIR_END_OF_RULES: Terminator for rule array
 * 
 * @note OTAPP_PAIR_RULES_ALLOWED_SIZE = 10 (maximum rules per set)
 * @see ad_button_pairRulesGetList_all_allowed()
 */
static otapp_pair_rule_t ad_button_deviceRules_all_allowed = {
    .allowed = {OTAPP_PAIR_NO_RULES, OTAPP_PAIR_END_OF_RULES} // NO RULES, pair every incoming device
};

/**
 * @brief Pairing rule set that blocks all devices
 * 
 * @details
 * This rule set prevents pairing with any device.
 * Useful for security lock-down, maintenance mode, or when pairing should be disabled.
 * 
 * Configuration:
 * - OTAPP_PAIR_NO_ALLOWED: Special flag blocking all pairing attempts
 * - OTAPP_PAIR_END_OF_RULES: Terminator for rule array
 * 
 * @see ad_button_pairRulesGetList_no_allowed()
 */
static otapp_pair_rule_t ad_button_deviceRules_no_allowed = {
    .allowed = {OTAPP_PAIR_NO_ALLOWED, OTAPP_PAIR_END_OF_RULES} // every device is not allowed
};

/**
 * @brief Custom pairing rule set allowing specific device types
 * 
 * @details
 * Default rule set permitting pairing only with lighting devices.
 * 
 * Allowed device types:
 * - OTAPP_LIGHTING: Generic lighting devices
 * - OTAPP_LIGHTING_ON_OFF: On/off switchable lights
 * - OTAPP_LIGHTING_DIMM: Dimmable lights
 * - OTAPP_LIGHTING_RGB: RGB color lights
 * - OTAPP_PAIR_END_OF_RULES: Terminator
 * 
 * @note Modify this array to customize allowed device types for your application
 * @see ad_button_pairRulesGetList()
 */
static otapp_pair_rule_t ad_button_deviceRules = {
    .allowed = {OTAPP_LIGHTING, OTAPP_LIGHTING_ON_OFF, OTAPP_LIGHTING_DIMM, OTAPP_LIGHTING_RGB, OTAPP_PAIR_END_OF_RULES}   
};

/**
 * @brief Get pairing rules that allow all devices
 * 
 * @return Pointer to ad_button_deviceRules_all_allowed rule set
 * 
 * @details
 * Use this callback when you want to pair with any device without filtering.
 * 
 * @see ad_button_deviceRules_all_allowed
 */
otapp_pair_rule_t *ad_button_pairRulesGetList_all_allowed()
{
    return &ad_button_deviceRules_all_allowed;
}

/**
 * @brief Get pairing rules that block all devices
 * 
 * @return Pointer to ad_button_deviceRules_no_allowed rule set
 * 
 * @details
 * Use this callback when you want to prevent any device pairing.
 * 
 * @see ad_button_deviceRules_no_allowed
 */
otapp_pair_rule_t *ad_button_pairRulesGetList_no_allowed()
{
    return &ad_button_deviceRules_no_allowed;
}

/**
 * @brief Get custom pairing rules for specific device types
 * 
 * @return Pointer to ad_button_deviceRules rule set
 * 
 * @details
 * Use this callback when you want to filter devices based on type.
 * Current configuration allows only lighting devices.
 * 
 * @see ad_button_deviceRules
 */
otapp_pair_rule_t *ad_button_pairRulesGetList()
{
    return &ad_button_deviceRules;
}

//////////////////////
// OBSERVER CALLBACKS
//////////////////////

/**
 * @brief Callback invoked when a device is successfully paired
 * 
 * @param[in] device Pointer to paired device information structure
 * 
 * @details
 * This callback is triggered when:
 * 1. A new device is discovered on the network
 * 2. The device passes the configured pairing rules (deviceRules)
 * 3. The device has been added to the paired device list
 * 
 * The callback receives complete device information including:
 * - Full device name
 * - IPv6 address
 * - List of available CoAP resource URIs
 * 
 * **Actions performed:**
 * - Assigns the device to a button via ad_btn_assignDevice()
 * - Logs device information and available URIs
 * 
 * @note This function is called by the OpenThread framework automatically
 * @note Device is ready for immediate use after this callback
 * 
 * @see otapp_pair_Device_t for device structure definition
 * @see ad_btn_assignDevice() for button assignment logic
 */
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

/**
 * @brief Callback invoked when a subscribed URI status is updated
 * 
 * @param[in] data Pointer to URI status update data packet
 * 
 * @details
 * This callback is triggered when:
 * 1. A paired device's CoAP resource state changes
 * 2. The device sends an update via CoAP Observe mechanism
 * 3. The update contains new status data (e.g., light on/off, brightness, color)
 * 
 * **Data Processing:**
 * 1. Extracts 32-bit state value from buffer (little-endian format)
 * 2. Updates internal state tracking via pair.uriStateSet()
 * 3. Logs token and state value for debugging
 * 
 * **Common Status Updates:**
 * - Light on/off state changes
 * - Brightness level adjustments
 * - RGB color changes
 * - Switch position updates
 * 
 * **Token Identification:**
 * The 4-byte token uniquely identifies which device/URI sent the update,
 * allowing the system to track multiple subscribed resources.
 * 
 * @note This function is called asynchronously by the CoAP framework
 * @note Buffer contains device-specific status data (format varies by device type)
 * 
 * @see oac_uri_dataPacket_t for data packet structure definition
 */
void ad_button_subscribedUrisCallback(oac_uri_dataPacket_t *data)  
{    
    // Parse 32-bit state value from buffer (little-endian)
    uint32_t uriState_ = 0;
    uriState_ |= (uint32_t)data->buffer[0];
    uriState_ |= ((uint32_t)data->buffer[1] << 8);
    uriState_ |= ((uint32_t)data->buffer[2] << 16);
    uriState_ |= ((uint32_t)data->buffer[3] << 24);

    // Update internal state tracking
    otapp_pair_DeviceList_t *pairHandle = drv->api.pair.getHandle();
    drv->api.pair.uriStateSet(pairHandle, data->token, &uriState_);
    
    // Log status update for debugging
    printf("@ Dev button from subs: \n");
    printf(" @--> token: 0x%x 0x%x 0x%x 0x%x\n", data->token[0], data->token[1], data->token[2], data->token[3]);
    printf(" @--> data: %ld\n", uriState_);
}

/**
 * @brief Main task function for button device
 * 
 * @details
 * This function should be called periodically from the main loop or RTOS task.
 * It processes button events and triggers appropriate actions.
 * 
 * @note Non-blocking function - returns immediately after processing
 * 
 * @see ad_btn_task() for button processing implementation
 */
void ad_button_task()
{
    ad_btn_task();
}

//////////////////////
// INITIALIZATION
//////////////////////

void ad_button_Init(char *deviceNameGroup)
{    
    if(deviceNameGroup == NULL) return;
    strcpy(ad_button_deviceNameTab, deviceNameGroup);

    // Get singleton driver instance
    drv = ot_app_drv_getInstance();

    // Initialize NVS for persistent storage
    if(drv->api.nvs.init == NULL) return;
    drv->api.nvs.init();

    // Initialize button and URI sub-modules
    ad_btn_init(drv);
    ad_btn_uri_init(drv);

    // Register pairing rules callback (choose one option):
    // drv->pairRuleGetList_clb = ad_button_pairRulesGetList_all_allowed;     // if you want to pair all devices
    // drv->pairRuleGetList_clb = ad_button_pairRulesGetList_no_allowed;      // if you do not want to pair devices
    drv->pairRuleGetList_clb = ad_button_pairRulesGetList;                    // if you have some rules (default)
    
    // Register URI callbacks
    drv->uriGetList_clb = ad_btn_uri_getList_clb;
    drv->uriGetListSize = ad_btn_uri_getListSize();

    // Register observer callbacks
    drv->obs_pairedDevice_clb = ad_button_pairedCallback;
    drv->obs_subscribedUri_clb = ad_button_subscribedUrisCallback;

    // Set device name gropu. length of it should be max OTAPP_DEVICENAME_SIZE - 1 (9 bytes)
    drv->deviceName = ad_button_deviceNameTab;
    drv->deviceType = &ad_button_deviceType;
    drv->task = ad_button_task;
}
