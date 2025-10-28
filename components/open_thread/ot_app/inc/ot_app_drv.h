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

#include "ot_app_pair.h"
#include "stdint.h"

#include "ot_app_coap_uri_obs.h"

#ifndef UNIT_TEST
    #include "ot_app_coap.h"
    #include "ot_app.h"
#else
    #include "mock_ot_app.h"
    #include "mock_ot_app_coap.h"
    // #include "mock_ot_app_coap_uri_obs.`h"       
#endif

/**
 * @brief NVS Non-Volatile Storage functions
 */
typedef struct ot_app_drv_nvs_t{
    /**
     * @brief save data into NVS partition
     * 
     * @param inData        [in] ptr to data
     * @param keyId         [in] it is id for saved data. It will be necessary for update or read data
     * @return int8_t       OT_APP_NVS_ERROR or OT_APP_NVS_OK
     */
    int8_t (*saveString)(const char *inData, const uint8_t keyId);

    /**
     * @brief read data from NVS partition
     * 
     * @param outData        [out] ptr to data
     * @param outDataSize    [out] ptr to the size of the data that was read
     * @param keyId          [in] it is id for saved data. It will be necessary for update or read data
     * @return int8_t        OT_APP_NVS_ERROR or OT_APP_NVS_OK
     */
    int8_t (*readString)(char *outBuff, uint8_t outBuffSize, const uint8_t keyId);

    int8_t (*init)(void);

}ot_app_drv_nvs_t;

typedef struct ot_app_drv_devName_t{
    int8_t (*devNameFullToEUI)(const char *deviceNameFull, uint8_t stringLength, char **outEuiChrPtr);    
}ot_app_drv_devName_t;

typedef struct ot_app_drv_obs_t{
    /**
     * @brief get observer instance 
     * @return oac_uri_observer_t pointer
     */
    struct{
        oac_uri_dataPacket_t *(*getDataPacket)(void);
        int8_t (*parseMessage)(const uint8_t *inBuffer, oac_uri_dataPacket_t *out);
        int8_t (*sendSubscribeRequest)(const otIp6Address *ipAddr, const char *aUriPath, uint8_t *outToken);

    }client;
    
    struct{
        oac_uri_observer_t *(*getHandle)(void);
        int8_t (*notify)(oac_uri_observer_t *subListHandle, oacu_uriIndex_t serverUri, const uint8_t *dataToNotify, uint16_t dataSize);        
        int8_t (*subscribe)(oac_uri_observer_t *subListHandle, otMessage *aMessage, const otMessageInfo *aMessageInfo, oacu_uriIndex_t uriId, char* deviceNameFull);
        int8_t (*unsubscribe)(oac_uri_observer_t *subListHandle, char* deviceNameFull, const oacu_token_t *token);
        int8_t (*XdeleteAll)(oac_uri_observer_t *subListHandle);
        
    }server;
}ot_app_drv_obs_t;

typedef struct ot_app_drv_coap_t{
    void (*sendBytePut)(const otIp6Address *peer_addr, const char *aUriPath, const uint8_t *payloadMsg, const uint16_t payloadMsgSize, otCoapResponseHandler responseHandler,  void *aContext);
    void (*sendByteGet)(const otIp6Address *peer_addr, const char *aUriPath, otCoapResponseHandler responseHandler, void *aContext);
    void (*sendResponse)(otMessage *requestMessage, const otMessageInfo *aMessageInfo, const uint8_t *responceContent, uint16_t responceLength);
    int8_t (*readPayload)(otMessage *aMessage, uint8_t *bufferOut, uint16_t bufferSize, uint16_t *readBytesOut);
    int8_t (*processUriRequest)(otMessage *aMessage, const otMessageInfo *aMessageInfo, oacu_uriIndex_t uriId, uint8_t *bufOut, uint16_t bufSize);
}ot_app_drv_coap_t;

typedef struct ot_app_devDrvAPI_t{
    ot_app_drv_devName_t devName;
    ot_app_drv_nvs_t    nvs;
    ot_app_drv_obs_t    obs;    // uri observer functions
    ot_app_drv_coap_t   coap;   // coap functions  
}ot_app_devDrvAPI_t;

typedef uint8_t ot_app_size_t;
typedef otapp_pair_rule_t *(*pairRuleGet_callback_t)(void);
typedef otapp_coap_uri_t *(*uriGet_callback_t)(void);
typedef void (*subscribedUris_callback_t)(oac_uri_dataPacket_t *dataPacket);
typedef void (*mainTask_callback_t)(void);

typedef struct ot_app_devDrv_t{
    subscribedUris_callback_t     obs_subscribedUri_clb; // it will be called from subscribed_uris uri

    /**
     * @brief it will be called when new device has been properly paired. All data are saved in otapp_pair_DeviceList_t.
     * @param [out] newDevice ptr to data struct otapp_pair_Device_t
     */
    otapp_pair_observerCallback_t obs_pairedDevice_clb;  

    pairRuleGet_callback_t      pairRuleGetList_clb;
    uriGet_callback_t           uriGetList_clb;

    const char *deviceName;
    const otapp_deviceType_t *deviceType;

    ot_app_size_t               uriGetListSize;
    mainTask_callback_t     task;
    
    ot_app_devDrvAPI_t api;
}ot_app_devDrv_t;


ot_app_devDrv_t *ot_app_drv_getInstance(void);
void ot_app_drv_task(void);

#endif  /* OT_APP_DRV_H_ */
