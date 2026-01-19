/**
 * @file ad_button.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Control device - button - implementation of hardware device for OpenThread app
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
 * @defgroup device_button Button Device
 * @ingroup devices
 * @{
 * 
 * @section description Description
 * 
 * Three-button control device with multi-function capabilities for OpenThread networks.
 * 
 * This device features three buttons with multi-function capabilities:
 * - **One-click**: Turns the light on or off
 * - **Double-click**: Changes the light color
 * - **Long-click**: Adjusts brightness continuously (cycles between dimming and brightening)
 * 
 * @section onebutton_library OneButton Library Integration
 * 
 * The button detection and event handling is implemented using a ported version of the
 * **OneButton library** originally created for Arduino, adapted for embedded systems.
 * 
 * @subsection onebutton_features OneButton Library Features
 * 
 * The library provides:
 * - **Click detection**: Single-click, double-click events
 * - **Long-press detection**: Continuous press with configurable timeout
 * - **Multi-click counting**: Track number of consecutive clicks
 * - **Debouncing**: Hardware debouncing for reliable button detection
 * - **Non-blocking operation**: Poll-based event detection
 * 
 * @subsection onebutton_platform_selection Platform Selection
 * 
 * The OneButton library supports multiple microcontroller platforms through compile-time
 * platform selection using preprocessor macros.
 * 
 * **To select your platform, define ONE of the following in `OneButton.h`:**
 * - `OB_STM32_PLATFORM` - For STM32 microcontrollers with HAL
 * - `OB_ESP32_PLATFORM` - For ESP32 microcontrollers with ESP-IDF
 * 
 * **Example in OneButton.h:**
 * @code{.c}
 * // Uncomment ONE platform:
 * // #define OB_STM32_PLATFORM   // For STM32
 * #define OB_ESP32_PLATFORM      // For ESP32 (currently active)
 * @endcode
 * 
 * @warning Define ONLY ONE platform macro at a time
 * 
 * @subsection onebutton_port Platform-Specific Configuration
 * 
 * Based on the selected platform macro, `OneButton.h` automatically configures
 * the appropriate GPIO and timing functions.
 * 
 * **STM32 Platform Configuration** (`OB_STM32_PLATFORM`):
 * @code{.c}
 * #ifdef OB_STM32_PLATFORM
 * #define OB_READ_PIN()            HAL_GPIO_ReadPin(Btn->GpioPort, Btn->GpioPin)
 * #define OB_GET_TICK()            HAL_GetTick()
 * #define OB_BUTTON_PRESSED        GPIO_PIN_RESET
 * #define OB_BUTTON_NOT_PRESSED    GPIO_PIN_SET
 * #endif
 * @endcode
 * 
 * **ESP32 Platform Configuration** (`OB_ESP32_PLATFORM`):
 * @code{.c}
 * #ifdef OB_ESP32_PLATFORM
 * #include "xtimers.h"
 * #include "ot_app.h"
 * #define OB_READ_PIN()            gpio_get_level(Btn->GpioPin)
 * #define OB_GET_TICK()            xTim_getTick()
 * #define OB_BUTTON_PRESSED        0
 * #define OB_BUTTON_NOT_PRESSED    1
 * #endif
 * @endcode
 * 
 * @subsection onebutton_definitions Macro Definitions by Platform
 * 
 * | Macro | Description | STM32 Implementation | ESP32 Implementation |
 * |-------|-------------|---------------------|---------------------|
 * | `OB_READ_PIN()` | Read current GPIO pin state | `HAL_GPIO_ReadPin()` | `gpio_get_level()` |
 * | `OB_GET_TICK()` | Get current system tick (milliseconds) | `HAL_GetTick()` | `xTim_getTick()` |
 * | `OB_BUTTON_PRESSED` | Logic level when button is pressed | `GPIO_PIN_RESET` (0) | `0` (active low) |
 * | `OB_BUTTON_NOT_PRESSED` | Logic level when button is released | `GPIO_PIN_SET` (1) | `1` (pull-up) |
 * 
 * @subsection onebutton_porting Porting to New Platforms
 * 
 * To add support for a new platform:
 * 
 * 1. **Add platform definition macro** in `OneButton.h`:
 *    @code{.c}
 *    // #define OB_STM32_PLATFORM
 *    // #define OB_ESP32_PLATFORM
 *    #define OB_YOUR_PLATFORM   // Add your new platform
 *    @endcode
 * 
 * 2. **Add platform-specific configuration block** in `OneButton.h`:
 *    @code{.c}
 *    #ifdef OB_YOUR_PLATFORM
 *    #include "your_platform_gpio.h"
 *    #include "your_platform_timer.h"
 *    #define OB_READ_PIN()         your_gpio_read(Btn->GpioPin)
 *    #define OB_GET_TICK()         your_get_milliseconds()
 *    #define OB_BUTTON_PRESSED     0  // or 1, depending on hardware
 *    #define OB_BUTTON_NOT_PRESSED 1  // or 0, depending on hardware
 *    #endif
 *    @endcode
 * 
 * 3. **Update OneButton_t structure** if needed (e.g., add platform-specific fields):
 *    @code{.c}
 *    typedef struct {
 *        ButtonState State;
 *    #ifdef OB_STM32_PLATFORM
 *        GPIO_TypeDef* GpioPort;  // STM32 needs port
 *    #endif
 *        uint16_t GpioPin;         // All platforms need pin
 *        // ... rest of structure
 *    } OneButton_t;
 *    @endcode
 * 
 * 4. **Add platform-specific initialization** in `OneButton.c`:
 *    @code{.c}
 *    #ifdef OB_YOUR_PLATFORM
 *    void OneButtonInit(OneButton_t *Btn, uint16_t GpioPin) {
 *        // Platform-specific initialization
 *    }
 *    #endif
 *    @endcode
 * 
 * @subsection onebutton_hardware Hardware Considerations
 * 
 * **Button Connection Types:**
 * 
 * **Active Low (Pull-up)** - Button connects GPIO to ground when pressed:
 * - STM32: Configure pin with `GPIO_PULLUP` in GPIO_InitTypeDef
 * - ESP32: Configure pin with `GPIO_PULLUP_ENABLE` in gpio_config_t
 * - Set: `OB_BUTTON_PRESSED = 0`, `OB_BUTTON_NOT_PRESSED = 1`
 * 
 * **Active High (Pull-down)** - Button connects GPIO to VCC when pressed:
 * - STM32: Configure pin with `GPIO_PULLDOWN` in GPIO_InitTypeDef
 * - ESP32: Configure pin with `GPIO_PULLDOWN_ENABLE` in gpio_config_t
 * - Set: `OB_BUTTON_PRESSED = 1`, `OB_BUTTON_NOT_PRESSED = 0`
 * 
 * @note Default configuration uses **active low** with internal pull-up resistors
 * @note Ensure GPIO pins support input mode with pull-up/pull-down configuration
 * 
 * @warning Platform macro definitions must match actual hardware configuration to ensure reliable button detection
 * 
 * @see OneButton.h for complete library API and platform configuration
 * @see ad_btn_assign.h for button event handling implementation
 * 
 * @section gpio_button_settings GPIO Configuration for Buttons
 * 
 * @subsection gpio_default_config Default GPIO Pin Assignment
 * 
 * The button device uses three GPIO pins defined in the @ref gpio_btn_conf group.
 * 
 * **Default configuration for ESP32-C6:**
 * - **Button 1** (index 0): GPIO 3
 * - **Button 2** (index 1): GPIO 9
 * - **Button 3** (index 2): GPIO 15
 * 
 * @subsection gpio_customization Customizing GPIO Configuration
 * 
 * To modify GPIO pin assignments:
 * 
 * 1. Edit the `ot_btn_gpioList[]` array in @ref gpio_btn_conf
 * 2. Update `AD_BUTTON_NUM_OF_BUTTONS` if changing the number of buttons
 * 3. Ensure selected pins support input mode with pull-up resistors
 * 
 * **Example - 4-button configuration:**
 * @code{.c}
 * #define AD_BUTTON_NUM_OF_BUTTONS 4
 * const static gpio_num_t ot_btn_gpioList[] = {GPIO_NUM_2, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_18};
 * @endcode
 * 
 * @subsection gpio_hardware_ref Hardware Reference (ESP32-C6)
 * 
 * From technical reference manual Table 7-3. IO MUX Functions List
 * 
 * | GPIO | Pin Name | Function 0 | Function 1 | Function 2 | Function 3 | DRV | Reset | Notes |
 * |------|----------|------------|------------|------------|------------|-----|-------|------------------------|
 * | 3    | GPIO3    | GPIO3      | GPIO3      | —          | —          | 2   | 1     | R VDDPST1 domain, ADC1_CH3 |
 * | 8    | GPIO8    | GPIO8      | GPIO8      | —          | —          | 2   | 1     | reserved for WS2812B   |
 * | 9    | GPIO9    | GPIO9      | GPIO9      | —          | —          | 2   | 3     | —                      |
 * | 15   | GPIO15   | GPIO15     | GPIO15     | —          | —          | 2   | 1     | —                      |
 * 
 * **Reset column legend:**
 * - 1 = IE = 1 (input enabled)
 * - 3 = IE = 1, WPU = 1 (input enabled, pull-up resistor enabled)
 * 
 * @warning Avoid using GPIO 8 for buttons as it is reserved for WS2812B LED control.
 * 
 * @see gpio_btn_conf
 * 
 * @section pairing_procedure Procedure for Assigning a New Device to a Button
 * 
 * 1. Press the selected button briefly.
 * 2. Turn on the corresponding device (button → light).
 * 3. After a short moment, the device will be assigned to the selected button.
 * 
 * @note To successfully assign a new device, it must be in the same device name group
 * (API: drv->deviceName) and be on the list of allowed devices
 * (API: drv->pairRuleGetList_clb, otapp_pair_rule_t).
 * 
 * @section device_rules Device Pairing Rules Configuration
 * 
 * The button device supports configurable pairing rules that control which OpenThread devices
 * can be paired with the buttons. Three predefined rule sets are available:
 * 
 * @subsection rule_custom Custom Device Rules (Default)
 * 
 * **Variable**: `ad_button_deviceRules`
 * 
 * Custom rule set allowing pairing only with specific device types.
 * 
 * **Default configuration allows:**
 * - `OTAPP_LIGHTING` - Generic lighting devices
 * - `OTAPP_LIGHTING_ON_OFF` - On/off switchable lights
 * - `OTAPP_LIGHTING_DIMM` - Dimmable lights
 * - `OTAPP_LIGHTING_RGB` - RGB color lights
 * 
 * **Example configuration:**
 * @code{.c}
 * static otapp_pair_rule_t ad_button_deviceRules = {
 *     .allowed = {
 *         OTAPP_LIGHTING,
 *         OTAPP_LIGHTING_ON_OFF,
 *         OTAPP_LIGHTING_DIMM,
 *         OTAPP_LIGHTING_RGB,
 *         OTAPP_PAIR_END_OF_RULES
 *     }
 * };
 * @endcode
 * 
 * @subsection rule_all_allowed Allow All Devices
 * 
 * **Variable**: `ad_button_deviceRules_all_allowed`
 * 
 * Special rule set that allows pairing with **any** OpenThread device without restrictions.
 * 
 * **Configuration:**
 * @code{.c}
 * static otapp_pair_rule_t ad_button_deviceRules_all_allowed = {
 *     .allowed = {OTAPP_PAIR_NO_RULES, OTAPP_PAIR_END_OF_RULES}
 * };
 * @endcode
 * 
 * **Use case:** Development, testing, or flexible deployment scenarios.
 * 
 * @subsection rule_no_allowed Block All Devices
 * 
 * **Variable**: `ad_button_deviceRules_no_allowed`
 * 
 * Special rule set that **blocks** pairing with all devices.
 * 
 * **Configuration:**
 * @code{.c}
 * static otapp_pair_rule_t ad_button_deviceRules_no_allowed = {
 *     .allowed = {OTAPP_PAIR_NO_ALLOWED, OTAPP_PAIR_END_OF_RULES}
 * };
 * @endcode
 * 
 * **Use case:** Security lock-down, maintenance mode, or when pairing should be temporarily disabled.
 * 
 * @subsection rule_selection Selecting Active Rule Set
 * 
 * During initialization in `ad_button_Init()`, choose which rule set to use:
 * 
 * @code{.c}
 * // Option 1: Use custom rules (default)
 * drv->pairRuleGetList_clb = ad_button_pairRulesGetList;
 * 
 * // Option 2: Allow all devices
 * drv->pairRuleGetList_clb = ad_button_pairRulesGetList_all_allowed;
 * 
 * // Option 3: Block all devices
 * drv->pairRuleGetList_clb = ad_button_pairRulesGetList_no_allowed;
 * @endcode
 * 
 * @section pairing_callbacks Device Pairing and Status Update Callbacks
 * 
 * The button device provides two important callbacks for handling device pairing events
 * and receiving status updates from subscribed devices.
 * 
 * @subsection paired_callback Paired Device Callback
 * 
 * **Function**: `ad_button_pairedCallback(otapp_pair_Device_t *device)`
 * 
 * This callback is invoked when a new device is successfully paired and passes the
 * configured device pairing rules (deviceRules).
 * 
 * **Parameters:**
 * - `device` - Pointer to structure containing paired device information
 * 
 * **Device Information Structure:**
 * @code{.c}
 * typedef struct {
 *     char devNameFull[OTAPP_PAIR_NAME_FULL_SIZE];    ///< Full device name identifier
 *     otIp6Address ipAddr;                             ///< IPv6 address of the paired device
 *     otapp_pair_uris_t urisList[OTAPP_PAIR_URI_MAX]; ///< List of CoAP resource URIs exposed by device
 * } otapp_pair_Device_t;
 * @endcode
 * 
 * **Callback Behavior:**
 * 1. Receives device information immediately after successful pairing
 * 2. Device has already passed the pairing rule filter (deviceRules)
 * 3. Device is ready for assignment to a button
 * 4. URI list contains all CoAP resources available on the paired device
 * 
 * **Implementation Example:**
 * @code{.c}
 * void ad_button_pairedCallback(otapp_pair_Device_t *device)
 * {
 *     // Assign device to a button
 *     ad_btn_assignDevice(device);
 *     
 *     // Log paired device information
 *     printf("New device paired: %s\n", device->devNameFull);
 *     printf("IPv6 Address: %s\n", ipv6_to_string(&device->ipAddr));
 *     
 *     // Log available URIs
 *     for (int i = 0; i < OTAPP_PAIR_URI_MAX; i++) {
 *         if (device->urisList[i].uri[0] != '\0') {
 *             printf("  URI %d: %s\n", i, device->urisList[i].uri);
 *         }
 *     }
 * }
 * @endcode
 * 
 * @subsection subscribed_uri_callback Subscribed URI Status Update Callback
 * 
 * **Function**: `ad_button_subscribedUrisCallback(oac_uri_dataPacket_t *data)`
 * 
 * This callback receives status updates from devices that have been subscribed to via
 * CoAP Observe mechanism. For example, when a light's state changes (on/off, brightness, color),
 * this callback receives the updated status information.
 * 
 * **Parameters:**
 * - `data` - Pointer to structure containing status update data
 * 
 * **Data Packet Structure:**
 * @code{.c}
 * typedef struct {
 *     oacu_token_t	token[OAC_URI_OBS_TOKEN_LENGTH];    ///< CoAP token identifying the URI subscription
 *     uint8_t buffer[OAC_URI_OBS_BUFFER_SIZE];         ///< Status data payload
 * } oac_uri_dataPacket_t;
 * @endcode
 * 
 * **Callback Behavior:**
 * 1. Receives status updates from all subscribed device URIs
 * 2. Token identifies which specific URI/device sent the update
 * 3. Buffer contains device-specific status data (format depends on device type)
 * 4. Typically used to update UI, trigger actions, or synchronize state
 * 
 * **Common Status Data Formats:**
 * - Lighting devices: on/off state, brightness level, RGB color values
 * - Switch devices: switch state, position
 * - Sensor devices: temperature, humidity, motion detection
 * 
 * **Implementation Example:**
 * @code{.c}
 * void ad_button_subscribedUrisCallback(oac_uri_dataPacket_t *data)
 * {
 *     // Parse status data (example: 32-bit state value)
 *     uint32_t uriState = 0;
 *     uriState |= (uint32_t)data->buffer[0];
 *     uriState |= ((uint32_t)data->buffer[1] << 8);
 *     uriState |= ((uint32_t)data->buffer[2] << 16);
 *     uriState |= ((uint32_t)data->buffer[3] << 24);
 *     
 *     // Update internal state tracking
 *     otapp_pair_DeviceList_t *pairHandle = drv->api.pair.getHandle();
 *     drv->api.pair.uriStateSet(pairHandle, data->token, &uriState);
 *     
 *     // Log update
 *     printf("URI status update received\n");
 *     printf("  Token: 0x%02x%02x%02x%02x\n", 
 *            data->token[0], data->token[1], data->token[2], data->token[3]);
 *     printf("  State: %lu\n", uriState);
 * }
 * @endcode
 * 
 * @section callback_flow Callback Flow Diagram
 * 
 * ```
 * Device Discovery
 *        ↓
 * Check Pairing Rules (deviceRules)
 *        ↓
 * [PASS] → ad_button_pairedCallback() → Device assigned to button
 *        ↓
 * Subscribe to device URIs (CoAP Observe)
 *        ↓
 * Device status changes
 *        ↓
 * ad_button_subscribedUrisCallback() → Update local state
 * ```
 * 
 * @note Callbacks are invoked by the OpenThread framework asynchronously
 * @note Callbacks should be non-blocking and complete quickly
 * @note Heavy processing should be deferred to application task
 * 
 * @see ad_button_deviceRules for pairing rule configuration
 * @see otapp_pair_Device_t for paired device structure definition
 * @see oac_uri_dataPacket_t for status update data structure
 * 
 * @section factory_reset_procedure Procedure for Restoring the Device to Factory Condition
 * 
 * 1. Press and hold the first button (OT_BTN_RESET_LONGPRESS_GPIO) continuously.
 * 2. While holding first button, click the second button (OT_BTN_RESET_ONE_CLICK_GPIO) one-click 5 (OT_BTN_RESET_OC_CNT_RESET) times
 *    within a specified short time window (10 seconds OT_BTN_RESET_TIME_MS).
 * 3. Upon detecting this combination, the device will signal (blink an LED) that
 *    the factory reset process has started.
 * 4. After the signal, the device will erase all user settings regarding devices
 *    assigned to buttons.
 * 
 * @see def_factory_sec
 * @see ad_btn_assign.h for button function assignment API
 * @see ad_btn_uri.h for CoAP URI configuration
 * @see ad_btn_dimControl.h for brightness control configuration
 */

#ifndef AD_BUTTON_H_
#define AD_BUTTON_H_

#include "ot_app_drv.h"

// #include "driver/gpio.h"
#include "hal/gpio_types.h"

/**
 * @defgroup def_factory_sec Defines for factory reset settings
 * @{
 */
#define OT_BTN_RESET_LONGPRESS_GPIO  GPIO_NUM_9  ///< First button GPIO pin (must be held continuously)
#define OT_BTN_RESET_ONE_CLICK_GPIO  GPIO_NUM_3  ///< Second button GPIO pin (clicked multiple times)
#define OT_BTN_RESET_OC_CNT_RESET    5           ///< Number of one-click presses to trigger factory reset
#define OT_BTN_RESET_TIME_MS         10000       ///< Time window in milliseconds to perform reset sequence
/**
 * @}
 */

/**
 * @defgroup gpio_btn_conf GPIO button configuration
 * @brief GPIO pin assignment for physical buttons
 * 
 * @details
 * This configuration defines which GPIO pins are used for button inputs.
 * - Array index corresponds to button number (0-based)
 * - First button = index 0
 * - Total number of buttons must match AD_BUTTON_NUM_OF_BUTTONS
 * 
 * @note AD_BUTTON_NUM_OF_BUTTONS must equal the number of elements in ot_btn_gpioList[] array
 * @note Selected GPIO pins must support input mode with internal pull-up resistors
 * @{
 */
#define AD_BUTTON_NUM_OF_BUTTONS 3  ///< Total number of physical buttons
const static gpio_num_t ot_btn_gpioList[] = {GPIO_NUM_3, GPIO_NUM_9, GPIO_NUM_15};  ///< GPIO pin mapping for buttons
/**
 * @}
 */

/** @defgroup btn_error_codes Button Error Codes
 * @{
 */
#define AD_BUTTON_OK                 (-1)  ///< Operation successful
#define AD_BUTTON_ERROR              (-2)  ///< General error
#define AD_BUTTON_IS                 (-3)  ///< Condition is true
#define AD_BUTTON_IS_NOT             (-4)  ///< Condition is false
#define AD_BUTTON_IS_IN_RESET_STATE  (-5)  ///< Device is performing factory reset
/**
 * @}
 */

/**
 * @brief Initialize button device driver with custom device name group
 * 
 * @param[in] deviceNameGroup Device name group identifier for network pairing
 * 
 * @details
 * Performs complete initialization of the button device including:
 * 
 * **Step 1: Validate device name group**
 * - Checks if deviceNameGroup parameter is valid (not NULL)
 * - Returns immediately if NULL to prevent crashes
 * 
 * **Step 2: Set device name group**
 * - Copies deviceNameGroup string to internal buffer (ad_button_deviceNameTab)
 * - Maximum length: 9 characters (OTAPP_DEVICENAME_SIZE - 1)
 * - Uses strcpy() for string copy
 * 
 * **Step 3: Get driver instance**
 * - Retrieves singleton device driver instance
 * 
 * **Step 4: Initialize NVS (Non-Volatile Storage)**
 * - Required for storing paired device information persistently
 * 
 * **Step 5: Initialize sub-modules**
 * - ad_btn_init(): Button GPIO and event detection
 * - ad_btn_uri_init(): CoAP URI handler registration
 * 
 * **Step 6: Register pairing rules callback**
 * Choose one of three options:
 * - ad_button_pairRulesGetList(): Custom rules (default - lighting devices only)
 * - ad_button_pairRulesGetList_all_allowed(): Allow all devices
 * - ad_button_pairRulesGetList_no_allowed(): Block all devices
 * 
 * **Step 7: Register URI callbacks**
 * - uriGetList_clb: Provides list of supported CoAP URIs
 * - uriGetListSize: Returns number of supported URIs
 * 
 * **Step 8: Register observer callbacks**
 * - obs_pairedDevice_clb: Called when device is paired
 * - obs_subscribedUri_clb: Called when subscribed URI status updates
 * 
 * **Step 9: Set device metadata**
 * - deviceName: Points to ad_button_deviceNameTab containing the name group
 * - deviceType: OTAPP_SWITCH type
 * - task: Periodic task function pointer
 * 
 * @section device_name_group Device Name Group
 * 
 * The device name group is used for:
 * - **Network Discovery**: Identifies this device on the OpenThread network
 * - **Pairing Filter**: Only devices with matching name group can be paired
 * - **Device Grouping**: Groups related devices together (e.g., "living_room", "bedroom")
 * 
 * **Naming Conventions:**
 * - Maximum length: 9 characters (OTAPP_DEVICENAME_SIZE - 1)
 * - Valid characters: a-z, A-Z, 0-9, underscore (_)
 * - No spaces or special characters
 * - Case-sensitive
 * 
 * **Examples:**
 * @code{.c}
 * // Room-based grouping
 * ad_button_Init("kitchen");
 * ad_button_Init("bedroom1");
 * ad_button_Init("garage");
 * 
 * // Function-based grouping
 * ad_button_Init("lights");
 * ad_button_Init("security");
 * 
 * // Building-based grouping
 * ad_button_Init("house_a");
 * ad_button_Init("house_b");
 * @endcode
 * 
 * @note Device name group must match between button device and target devices for pairing
 * @note Devices can only pair with others in the same name group
 * 
 * @warning Must be called before any button operations
 * @warning deviceNameGroup must not be NULL
 * @warning deviceNameGroup length must not exceed 9 characters
 * @warning NVS must be initialized or function will return early
 * 
 * @see ad_btn_init() for button module initialization
 * @see ad_btn_uri_init() for URI module initialization
 * @see ad_button_pairedCallback() for device pairing handler
 * @see ad_button_subscribedUrisCallback() for status update handler
 */
void ad_button_Init(char *deviceNameGroup);

#endif /* AD_BUTTON_H_ */
/**
 * @}
 */
