/**
 * @file ad_btn_uri.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief CoAP URI management for button device
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
 * 
 * @defgroup device_btn_uri Button Device URI Management
 * @ingroup device_button
 * @{
 * 
 * @section uri_overview Overview
 * 
 * This module manages CoAP URI endpoints for the button device, enabling remote control
 * and status queries over the OpenThread network.
 * 
 * The button device exposes a single CoAP endpoint:
 * - **btn/state**: Query button state and receive button press notifications
 * 
 * @section uri_architecture URI Architecture
 * 
 * The URI system uses a callback-based architecture:
 * 
 * 1. **URI Registration**: URI is stored in static array `ad_button_uri[]` (ad_btn_uri.c)
 * 2. **Driver Callback**: `ad_btn_uri_getList_clb()` is registered with the device driver
 * 3. **Driver Access**: The framework calls the callback to retrieve registered URI
 * 4. **Request Handling**: CoAP requests to the URI invoke the handler
 * 
 * @section uri_handler_flow URI Request Flow
 * 
 * ```
 * CoAP Request → Driver → ad_btn_uri_getList_clb() → URI List
 *                    ↓
 *             Find Matching URI ("btn/state")
 *                    ↓
 *          Invoke ad_button_uri_btn_state_CoreHandle()
 *                    ↓
 *        processUriRequest() → Parse Message
 *                    ↓
 *            Process & Respond
 * ```
 * 
 * @section uri_endpoint Button State Endpoint
 * 
 * **URI Path**: `btn/state`
 * 
 * **Type**: OTAPP_SWITCH
 * 
 * **Supported Operations**:
 * - **Observer Mode**: Subscribe to button state changes (CoAP Observe)
 * - **Direct Request**: Query current button state (CoAP GET)
 * 
 * **Use Cases**:
 * - Remote devices can subscribe to receive notifications when button is pressed
 * - Other devices can query current button state
 * - Button device can push state updates to subscribed observers
 * 
 * @section uri_adding Adding Custom URI Handlers
 * 
 * To add additional URI endpoints:
 * 
 * **Step 1: Define URI handler function** with CoAP handler signature:
 * @code{.c}
 * void my_custom_uri_handler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
 * {
 *     int8_t result = 0;
 *     result = drv->api.coap.processUriRequest(aMessage, aMessageInfo, 
 *                                              OTAPP_CUSTOM_TYPE, 
 *                                              coapBuffer, 
 *                                              OAC_URI_OBS_BUFFER_SIZE);
 *     
 *     if(result == OTAPP_COAP_OK) {
 *         // Handle URI request here
 *         printf("Custom URI payload: %s\n", (char*)coapBuffer);
 *     } else {
 *         printf("Custom URI error: %d\n", result);
 *     }
 * }
 * @endcode
 * 
 * **Step 2: Add URI to the array** in ad_btn_uri.c:
 * @code{.c}
 * static otapp_coap_uri_t ad_button_uri[] = {
 *     {OTAPP_SWITCH, {"btn/state", ad_button_uri_btn_state_CoreHandle, NULL, NULL},},
 *     // Add your custom URI here
 *     {OTAPP_CUSTOM_TYPE, {"btn/config", my_custom_uri_handler, NULL, NULL},},
 * };
 * @endcode
 * 
 * **Step 3: Size is automatically calculated** by `AD_BUTTON_URI_SIZE` macro:
 * @code{.c}
 * #define AD_BUTTON_URI_SIZE (sizeof(ad_button_uri) / sizeof(ad_button_uri[0]))
 * @endcode
 * 
 * @section uri_constraints URI Constraints
 * 
 * - **Maximum URIs**: `OTAPP_PAIRED_URI_MAX` (defined in ot_app_drv.h)
 * - **Maximum URI name length**: `OTAPP_COAP_URI_MAX_LENGHT` (defined in ot_app_drv.h)
 * - **URI format**: Use forward slash separator (e.g., "btn/state", "btn/config")
 * 
 * @section uri_usage Usage Example
 * 
 * @subsection uri_init_example Initialization
 * @code{.c}
 * // Initialize device driver
 * ot_app_devDrv_t *driver = get_device_driver();
 * 
 * // Initialize URI handler - registers callback with driver
 * ad_btn_uri_init(driver);
 * 
 * // Driver will call ad_btn_uri_getList_clb() to retrieve URI list
 * @endcode
 * 
 * @subsection uri_query_example Querying Registered URI
 * @code{.c}
 * // Get registered URI via driver callback
 * otapp_coap_uri_t *uri = ad_btn_uri_getList_clb();
 * uint8_t count = ad_btn_uri_getListSize();
 * 
 * printf("Registered URI: %s (count: %d)\n", uri[0].coap_uri.mUriPath, count);
 * // Output: Registered URI: btn/state (count: 1)
 * @endcode
 * 
 * @see ot_app_drv.h for otapp_coap_uri_t structure definition
 * @see ad_button.h for device initialization
 */

#ifndef AD_BTN_URI_H_
#define AD_BTN_URI_H_

#include "ot_app_drv.h"
#include "stdint.h"

/**
 * @brief Driver callback to get list of registered CoAP URI handlers
 * 
 * @details
 * This function is a **callback** registered with the device driver during initialization.
 * The driver framework calls this function to retrieve the button device's CoAP URI
 * and its associated handler.
 * 
 * **Callback Flow:**
 * 1. `ad_btn_uri_init()` registers this callback with driver
 * 2. Driver stores callback pointer: `driver->uriGetList_clb = ad_btn_uri_getList_clb`
 * 3. When needed, driver invokes: `otapp_coap_uri_t *list = driver->uriGetList_clb()`
 * 4. Driver uses returned URI to route CoAP "btn/state" requests to the handler
 * 
 * @return Pointer to static array of otapp_coap_uri_t structures
 * @retval ad_button_uri Array containing single URI: "btn/state"
 * @retval NULL Can be returned if URI list is empty (currently not implemented)
 * 
 * @note This function returns a pointer to a **static internal array** valid for application lifetime
 * @note The returned array contains exactly 1 element (AD_BUTTON_URI_SIZE = 1)
 * @note This is a callback function - typically called by driver, not directly by user code
 * 
 * @warning Do not modify the returned array - it is read-only
 * 
 * @see ad_btn_uri_getListSize() to get number of URIs in the returned array (returns 1)
 * @see ad_btn_uri_init() for callback registration
 */
otapp_coap_uri_t *ad_btn_uri_getList_clb(void);

/**
 * @brief Get number of registered URI handlers
 * 
 * @details
 * Returns the count of URI endpoints registered in the internal URI list.
 * This corresponds to the size of the array returned by ad_btn_uri_getList_clb().
 * 
 * @return Number of URIs in the list
 * @retval 1 Current implementation registers single URI: "btn/state"
 * @retval 0 Can be returned if no URIs registered (currently not implemented)
 * 
 * @note Current implementation returns: `AD_BUTTON_URI_SIZE` = 1
 * 
 * @see ad_btn_uri_getList_clb()
 */
uint8_t ad_btn_uri_getListSize(void);

/**
 * @brief Initialize URI handlers for button device
 * 
 * @param[in] devDrv Pointer to device driver structure
 * 
 * @details
 * This function performs the following initialization steps:
 * 1. Validates device driver pointer
 * 2. Stores driver reference in static variable `drv` for use by URI handler
 * 3. **Registers callback** `ad_btn_uri_getList_clb()` with the driver
 * 
 * After initialization, the driver can:
 * - Call `ad_btn_uri_getList_clb()` to retrieve the "btn/state" URI
 * - Route incoming CoAP "btn/state" requests to the registered handler
 * - Enable remote devices to query button state or subscribe to button events
 * 
 * **Typical initialization sequence:**
 * @code{.c}
 * ot_app_devDrv_t driver;
 * // ... initialize driver structure ...
 * 
 * ad_btn_uri_init(&driver);  // Register URI callback
 * ad_button_Init();          // Complete device initialization
 * @endcode
 * 
 * @note Must be called after device driver structure allocation
 * @note Must be called before ad_button_Init()
 * @note Calling with NULL pointer has no effect (safe, but initialization will fail)
 * 
 * @warning Do not call this function multiple times with different drivers
 * 
 * @see ad_button_Init() for complete device initialization
 * @see ad_btn_uri_getList_clb() for registered callback function
 */
void ad_btn_uri_init(ot_app_devDrv_t *devDrv);

#endif /* AD_BTN_URI_H_ */

/**
 * @}
 */
