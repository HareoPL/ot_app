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
#include "fff.h"

#define otapp_deviceNameIsMatching mock_oadevName_otapp_deviceNameIsMatching

#define OTAPP_DEVICENAME_IS                 (1)
#define OTAPP_DEVICENAME_IS_NOT             (2)
#define OTAPP_DEVICENAME_OK                 (-1)

#define OTAPP_DEVICENAME_ERROR              (-2)
#define OTAPP_DEVICENAME_TOO_LONG           (-3)
#define OTAPP_DEVICENAME_TOO_SHORT          (-4)
#define OTAPP_DEVICENAME_BUFFER_TOO_SMALL   (-5)
#define OTAPP_DEVICENAME_CALL_DEVICE_NAME_SET_FN   (-6)

#ifndef OTAPP_DEVICENAME_FULL_SIZE
    #define OTAPP_DEVICENAME_FULL_SIZE  32 // OT_DNS_MAX_LABEL_SIZE host name: "device1_1_588c81fffe301ea4"
#endif


int8_t mock_oadevName_otapp_deviceNameIsMatching(char *deviceFullName);
void mock_oadevName_state(int8_t returnState);

DECLARE_FAKE_VALUE_FUNC0(const char *, otapp_deviceNameFullGet);
DECLARE_FAKE_VALUE_FUNC2(int16_t, otapp_deviceNameGetDevId, const char *, uint8_t);


#endif  /* MOCK_OT_APP_DEVICENAME_H_ */
