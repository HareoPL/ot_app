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
 * 
 * @defgroup device_button Button Device
 * @ingroup devices
 * @{
 * 
 * @section description Description
 *  Three-button control device with multi-function capabilities
 * 
 * This device features three buttons with multi-function capabilities:
 * - **One-click**: Turns the light on or off
 * - **Double-click**: Changes the light color
 * - **Long-click**: Adjusts brightness continuously (cycles between dimming and brightening)
 * 
 * @section pairing_procedure Procedure for Assigning a New Device to a Button
 * 
 * 1. Press the selected button briefly.
 * 2. Turn on the corresponding device (button → light).
 * 3. After a short moment, the device will be assigned to the selected button.
 * 
 * @note To successfully assign a new device, it must be in the same device name group 
 *       (API: drv->deviceName) and be on the list of allowed devices 
 *       (API: drv->pairRuleGetList_clb, otapp_pair_rule_t).
 * 
 * @section factory_reset_procedure Procedure for Restoring the Device to Factory Condition
 * 
 * 1. Press and hold the first button (OT_BTN_RESET_LONGPRESS_GPIO) continuously.
 * 2. While holding first button, click the second button (OT_BTN_RESET_ONE_CLICK_GPIO) one-click 5 (OT_BTN_RESET_OC_CNT_RESET) times 
 *    within a specified short time window (10 seconds OT_BTN_RESET_TIME_MS ).
 * 3. Upon detecting this combination, the device will signal (blink an LED) that 
 *    the factory reset process has started.
 * 4. After the signal, the device will erase all user settings regarding devices 
 *    assigned to buttons.
 * 
 * @see def_factory_sec 
 * 
 * @section gpio_button_settings GPIO for button config
 * 
 * From technical reference manual Table 7-3. IO MUX Functions List
 *
 * | GPIO | Pin Name | Function 0 | Function 1 | Function 2 | Function 3 | DRV | Reset | Notes                  |
 * |------|----------|------------|------------|------------|------------|-----|-------|------------------------|
 * | 3    | GPIO3    | GPIO3      | GPIO3      | —          | —          | 2   | 1     | R VDDPST1 domain, ADC1_CH3 |
 * | 8    | GPIO8    | GPIO8      | GPIO8      | —          | —          | 2   | 1     | reserved for WS2812B  |
 * | 9    | GPIO9    | GPIO9      | GPIO9      | —          | —          | 2   | 3     | —                      |
 * | 15   | GPIO15   | GPIO15     | GPIO15     | —          | —          | 2   | 1     | —                      |
 *
 * “Reset” column shows the default configuration of each pin after reset:
 *  - 1 - IE = 1 (input enabled)
 *  - 3 - IE = 1, WPU = 1 (input enabled, pull-up resistor enabled)
 *
 * @see gpio_btn_conf
 */


#ifndef AD_BUTTON_H_
#define AD_BUTTON_H_

#include "ot_app_drv.h"
/**
 * @defgroup def_factory_sec Defines for factory reset settings
 * @{
 */
#define OT_BTN_RESET_LONGPRESS_GPIO         GPIO_NUM_9  ///< First button GPIO pin
#define OT_BTN_RESET_ONE_CLICK_GPIO         GPIO_NUM_3  ///< Second button GPIO pin
#define OT_BTN_RESET_OC_CNT_RESET           5           ///< Number of one-click presses to trigger factory reset 
#define OT_BTN_RESET_TIME_MS                10000       ///< Time in milliseconds to hold button for reset
/**
 * @}
 */

/**
 * @defgroup gpio_btn_conf GPIO button config
 * first button = index 0
 * @note  AD_BUTTON_NUM_OF_BUTTONS equals the number of initialized buttons in ot_btn_gpioList[] table
 */
#define AD_BUTTON_NUM_OF_BUTTONS    3
const static gpio_num_t ot_btn_gpioList[] = {GPIO_NUM_3, GPIO_NUM_9, GPIO_NUM_15};

/**
 * @} 
 */

#define AD_BUTTON_OK        (-1)
#define AD_BUTTON_ERROR     (-2)
#define AD_BUTTON_IS        (-3)
#define AD_BUTTON_IS_NOT    (-4)
#define AD_BUTTON_IS_IN_RESET_STATE    (-5)

void ad_button_Init();

#endif  /* AD_BUTTON_H_ */
