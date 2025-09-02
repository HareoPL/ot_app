/**
 * @file mock_ot_app_deviceName.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 01-09-2025
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
#ifndef MOCK_OT_APP_DEVICENAME_H_
#define MOCK_OT_APP_DEVICENAME_H_

#include "stdint.h"

#define otapp_deviceNameIsMatching mock_oadevName_otapp_deviceNameIsMatching


#define OTAPP_DEVICENAME_IS                 (1)
#define OTAPP_DEVICENAME_IS_NOT             (2)

#define OTAPP_DEVICENAME_ERROR              (-2)


int8_t mock_oadevName_otapp_deviceNameIsMatching(char *deviceFullName);
void mock_oadevName_state(int8_t returnState);

#endif  /* MOCK_OT_APP_DEVICENAME_H_ */
