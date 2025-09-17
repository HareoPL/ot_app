/**
 * @file ot_app_drv.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 08-09-2025
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
#ifndef OT_APP_DRV_H_
#define OT_APP_DRV_H_

#include "ot_app.h"
#include "ot_app_coap.h"
#include "ot_app_pair.h"

#include "stdint.h"


typedef uint8_t ot_app_size_t;
typedef otapp_pair_rule_t *(*pairRuleGet_callback_t)(void);
typedef otapp_coap_uri_t *(*uriGet_callback_t)(void);

typedef struct ot_app_devDrv_t{
        otapp_pair_observerCallback_t obs_subscribedUri; // it will be called from subscribed_uris uri
        otapp_pair_observerCallback_t obs_pairedDevice;  // it will be called when new device has been properly paired

        pairRuleGet_callback_t      pairRuleGetList;
        uriGet_callback_t           uriGetList;
        const char *uriResources;

        const char *deviceName;
        const otapp_deviceType_t *deviceType;

        ot_app_size_t               pairRuleGetListSize;
        ot_app_size_t               uriGetListSize;

}ot_app_devDrv_t;


ot_app_devDrv_t *ot_app_drv_getInstance(void);

#endif  /* OT_APP_DRV_H_ */
