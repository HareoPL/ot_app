/**
 * @file ad_light.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief exevutive device - light - implementation of hardware device for openThread app
 * @version 0.1
 * @date 06-09-2025
 * 
 * * 
 * @defgroup device_light Light Device
 * @ingroup devices
 * @{
 * 
 * @section description Description
 *  description!!!
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
 * 1. Press and hold the first button (button 1) continuously.
 * 2. While holding btn1, click the second button (button 2) one-click four times 
 *    within a specified short time window (e.g., 3–5 seconds).
 * 3. Upon detecting this combination, the device will signal (blink an LED) that 
 *    the factory reset process has started.
 * 4. After the signal, the device will erase all user settings regarding devices 
 *    assigned to buttons.
 * @}
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

#ifndef AD_LIGHT_H_
#define AD_LIGHT_H_

void ad_light_init(char *deviceNameGroup);

#endif  /* AD_LIGHT_H_ */
