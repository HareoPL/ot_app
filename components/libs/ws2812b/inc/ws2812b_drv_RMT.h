/**
 * @file ws2812b_drv_RMT.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief ws2812b interface for esp32-c6 using RMT 
 * 
 * @version 0.1
 * @date 15-04-2025
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
#ifndef WS2812B_DRV_RMT_H_
#define WS2812B_DRV_RMT_H_

#include "ws2812b_if.h"

#define WS2812B_IF_LEDS             5

#define RMT_LED_STRIP_GPIO_NUM      8
#define RMT_LED_STRIP_RESOLUTION_HZ (10 * 1000 * 1000) // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)

#define RMT_TIME_03_US              (0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000) // T=0.3us
#define RMT_TIME_09_US              (0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000) // T=0.9us


const ws2812b_drv_t *ws2812b_if_getDrvRMT(void);

void ws2812b_if_init(void);
void ws2812b_if_simpleTest(void); // add to your task with osDelay


#endif  /* WS2812B_DRV_RMT_H_ */
