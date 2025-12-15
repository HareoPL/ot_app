/**
 * @file ad_btn_assign.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief button function assignment
 * @version 0.1
 * @date 28-10-2025
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
#ifndef AD_BTN_ASSIGN_H_
#define AD_BTN_ASSIGN_H_

#include "ot_app_drv.h"

#define OT_BTN_OB_ONE_CLICK     1
#define OT_BTN_OB_LONG_PRESS    2

void ad_btn_task(void);
int8_t ad_btn_init(ot_app_devDrv_t *drvPtr);
int8_t ad_btn_assignDevice(otapp_pair_Device_t *newDevice);

#endif  /* AD_BTN_ASSIGN_H_ */
