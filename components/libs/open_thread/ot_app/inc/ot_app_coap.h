/**
 * @file ot_app_coap.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 21-07-2025
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
#ifndef OT_APP_COAP_H_
#define OT_APP_COAP_H_

#include "esp_openthread.h"
#include <openthread/message.h>

#define OTAPP_COAP_PORT 5683 ///< Default CoAP port, as specified in RFC 7252
#define OTAPP_CHAR_BUFFER 1024



void otapp_coap_init(void);
void otapp_coap_sendResponse(otMessage *requestMessage, const otMessageInfo *aMessageInfo, const char *responceContent);
void otapp_coap_printSenderIP(const otMessageInfo *aMessageInfo);
void otapp_coapSendtoTestPut();
void otapp_coapSendtoTestGet();

#endif  /* OT_APP_COAP_H_ */
