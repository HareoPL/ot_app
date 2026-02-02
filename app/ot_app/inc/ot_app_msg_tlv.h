/**
 * @file ot_app_msg_tlv.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 29-01-2026
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2026 
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
#ifndef OT_APP_MSG_TLV_H_
#define OT_APP_MSG_TLV_H_

#define OT_APP_MSG_TLV_OK        (-1)
#define OT_APP_MSG_TLV_ERROR     (-2)

#include "stdint.h"

int8_t otapp_msg_tlv_keyAdd(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, const uint16_t valueLengthIn, uint8_t *valueIn);
int8_t otapp_msg_tlv_keyGet(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, uint16_t *valueLengthOut, uint8_t *valueOut);


#endif  /* OT_APP_MSG_TLV_H_ */
