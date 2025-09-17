/**
 * @file ot_app_coap_uri_obs.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief observer pattern for uri 
 * @version 0.1
 * @date 16-09-2025
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
#ifndef OT_APP_COAP_URI_OBS_H_
#define OT_APP_COAP_URI_OBS_H_

#include "hro_utils.h"
#include <openthread/coap.h>

#define OAC_URI_OBS_TOKEN_LENGTH            4
#define OAC_URI_OBS_SUBSCRIBERS_MAX_NUM     20
#define OAC_URI_OBS_BUFFER_SIZE             256

typedef int8_t oacu_result_t;
typedef uint8_t oacu_uriIndex_t;
typedef uint8_t oacu_token_t;

typedef enum{
    OAC_URI_OBS_OK              = (-1),
    OAC_URI_OBS_ERROR           = (-2),
    OAC_URI_OBS_IS              = (-3),
    OAC_URI_OBS_IS_NOT          = (-4),
    OAC_URI_OBS_TOKEN_EXIST     = (-5),
    OAC_URI_OBS_TOKEN_NOT_EXIST = (-6),
    OAC_URI_OBS_LIST_FULL       = (-7),

}oac_obsError_t;

typedef struct {
    oacu_token_t token[OAC_URI_OBS_TOKEN_LENGTH];
    oacu_uriIndex_t uriIndex_client;
    uint8_t buffer[OAC_URI_OBS_BUFFER_SIZE];
} oac_uri_dataPacket_t;

typedef struct oac_uri_observer_t oac_uri_observer_t; // forward declaration

oac_uri_observer_t *oac_uri_obs_getSubListHandle(void);
int8_t oac_uri_obs_notify(oac_uri_observer_t *subListHandle, oacu_uriIndex_t serverUri, const uint8_t *dataToNotify, uint16_t dataSize);
oac_uri_dataPacket_t *oac_uri_obs_parseMessage(uint8_t *buffer, uint16_t bufferSize);
int8_t oac_uri_obs_subscribe(oac_uri_observer_t *subListHandle, oac_uri_observer_t *subscribeData);
int8_t oac_uri_obs_unsubscribe(oac_uri_observer_t *subListHandle, const oacu_token_t *token);
void oac_uri_obs_deleteAll(oac_uri_observer_t *subListHandle);

// oac_uri_dataPacket_t *oac_uri_obs_getdataPacketHandle(void);
#ifdef UNIT_TEST

/**
 * @brief check free space in subscribe list
 * @return int8_t         [out] free index position of oac_obsSubList[]
 *                              or  OAC_URI_OBS_ERROR (-1)
 */
PRIVATE int8_t oac_uri_obsIsFreeSpace();

#endif /* UNIT_TEST */

#endif  /* OT_APP_COAP_URI_OBS_H_ */
