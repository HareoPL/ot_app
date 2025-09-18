/**
 * @file ot_app_coap_uri_obs.c
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

#include "ot_app_coap_uri_obs.h"
#include "string.h"

#ifdef UNIT_TEST
    #include "mock_ot_app_coap.h"
#else
    #include "ot_app_coap.h"
#endif
typedef struct oac_uri_observer_t{
    union {
        struct {
            oacu_token_t token[OAC_URI_OBS_TOKEN_LENGTH];
            oacu_uriIndex_t uriIndex_client; 
        }clientData; // use it when you are going to send subscribe request
        struct {
            oacu_token_t token[OAC_URI_OBS_TOKEN_LENGTH];
            oacu_uriIndex_t uriIndex_client; // uri index from client
            oacu_uriIndex_t uriIndex_server; // uri index from server 
            otIp6Address ipAddr; 
        }serverData; // use it when you are going to register subscriber
    };
    uint8_t takenPosition;
} oac_uri_observer_t;

static oac_uri_observer_t oac_obsSubList[OAC_URI_OBS_SUBSCRIBERS_MAX_NUM];
static oac_uri_dataPacket_t oac_dataPacket;


PRIVATE int8_t oac_uri_obs_spaceIsFree(oac_uri_observer_t *subListHandle)
{    
    if(subListHandle == NULL)
    {
        return OAC_URI_OBS_ERROR;
    }

    for (uint8_t i = 0; i < OAC_URI_OBS_SUBSCRIBERS_MAX_NUM; i++)
    {
        if(subListHandle[i].takenPosition == 0)
        {
            return i; 
        }
    }

    return OAC_URI_OBS_LIST_FULL; 
}

PRIVATE int8_t oac_uri_obs_spaceTake(oac_uri_observer_t *subListHandle, uint8_t subListIndex)
{
    if(subListHandle == NULL)
    {
        return OAC_URI_OBS_ERROR;
    }

    subListHandle[subListIndex].takenPosition = 1;
    return OAC_URI_OBS_OK;
}

PRIVATE int8_t oac_uri_obs_spaceIsTaken(oac_uri_observer_t *subListHandle, uint8_t subListIndex)
{ 
    if(subListHandle == NULL)
    {
        return OAC_URI_OBS_ERROR;
    }

    return (subListHandle[subListIndex].takenPosition);
}

PRIVATE int8_t oac_uri_obs_tokenIsSame(oac_uri_observer_t *subListHandle, uint8_t subListIndex, const oacu_token_t *tokenToCheck)
{
    if(tokenToCheck == NULL || subListHandle == NULL)
    {
        return OAC_URI_OBS_ERROR;
    }

    for (uint8_t i = 0; i < OAC_URI_OBS_TOKEN_LENGTH; i++)
    {
       if(subListHandle[subListIndex].serverData.token[i] != tokenToCheck[i])
       {
            return OAC_URI_OBS_IS_NOT;
       }
    }
        
    return OAC_URI_OBS_IS;
}

PRIVATE int8_t oac_uri_obs_tokenIsExist(oac_uri_observer_t *subListHandle, const oacu_token_t *token)
{
    if(token == NULL || subListHandle == NULL)
    {
        return OAC_URI_OBS_ERROR;
    }

    for (int8_t i = 0; i < OAC_URI_OBS_SUBSCRIBERS_MAX_NUM; i++)
    {
        if(oac_uri_obs_spaceIsTaken(subListHandle, i))
        {
            if(oac_uri_obs_tokenIsSame(subListHandle, i, token) == OAC_URI_OBS_IS)
            {
                return i;
            }
        }
    }        

    return OAC_URI_OBS_IS_NOT;
}

oac_uri_observer_t *oac_uri_obs_getSubListHandle()
{
    return oac_obsSubList;
}

oac_uri_dataPacket_t *oac_uri_obs_getdataPacketHandle()
{
    return &oac_dataPacket;
}

int8_t oac_uri_obs_subscribe(oac_uri_observer_t *subListHandle, oac_uri_observer_t *subscribeData)
{
    oacu_result_t result_;

    if(subListHandle == NULL || subscribeData == NULL)
    {
        return OAC_URI_OBS_ERROR; 
    }
    
    if(oac_uri_obs_tokenIsExist(subListHandle, subscribeData->serverData.token) == OAC_URI_OBS_IS_NOT)
    {
        result_ = oac_uri_obs_spaceIsFree(subListHandle);
        if(result_ != OAC_URI_OBS_LIST_FULL || result_ != OAC_URI_OBS_ERROR)
        {
            oac_uri_obs_spaceTake(subListHandle, result_);
            memcpy(&subListHandle[result_].serverData, &subscribeData->serverData, sizeof(subscribeData->serverData));  
        }else
        {
            return OAC_URI_OBS_LIST_FULL;
        }
    }

    return OAC_URI_OBS_TOKEN_EXIST; 
}

int8_t oac_uri_obs_unsubscribe(oac_uri_observer_t *subListHandle, const oacu_token_t *token)
{
    oacu_result_t result_;

    if(subListHandle == NULL || token == NULL)
    {
        return OAC_URI_OBS_ERROR;
    }
    
    result_ = oac_uri_obs_tokenIsExist(subListHandle, token);
    if(result_ != OAC_URI_OBS_IS_NOT || result_ != OAC_URI_OBS_ERROR)
    {
        subListHandle[result_].takenPosition = 0;
        memset(&subListHandle[result_].serverData, 0, sizeof(subListHandle[result_].serverData));
        return OAC_URI_OBS_OK;     
    }

    return OAC_URI_OBS_TOKEN_NOT_EXIST;
}

int8_t oac_uri_obs_notify(oac_uri_observer_t *subListHandle, oacu_uriIndex_t serverUri, const uint8_t *dataToNotify, uint16_t dataSize)
{
    if(subListHandle == NULL || dataToNotify == NULL)
    {
        return OAC_URI_OBS_ERROR;
    }

    if(dataSize >= OAC_URI_OBS_BUFFER_SIZE)
    {
        return OAC_URI_OBS_ERROR;
    }

    for (uint8_t i = 0; i < OAC_URI_OBS_SUBSCRIBERS_MAX_NUM; i++)
    {
        if(oac_uri_obs_spaceIsTaken(subListHandle, i))
        {
            if(subListHandle[i].serverData.uriIndex_server == serverUri)
            {
                
                memcpy(&oac_dataPacket, &subListHandle[i].clientData, sizeof(subListHandle->clientData));
                memcpy(oac_dataPacket.buffer, dataToNotify, dataSize);

                otapp_coapSendPutUri_subscribed_uris(&subListHandle[i].serverData.ipAddr, (uint8_t *)&oac_dataPacket, sizeof(oac_dataPacket));
            }
        }
    }
    
    return OAC_URI_OBS_OK;
}

// todo powinna byc tutaj kolejka?  chodzi o to ze jesli nie zdozy sie z przetworzeniem w 
// uriSub to sie straci dane ? czyli trzeba stworzyc nowy task do przetwarzania w kolejce ? 

oac_uri_dataPacket_t *oac_uri_obs_parseMessage(uint8_t *buffer, uint16_t bufferSize)
{
    if(buffer == NULL || bufferSize >= OAC_URI_OBS_BUFFER_SIZE)
    {
        return NULL; 
    }
    
    memcpy(&oac_dataPacket, buffer, bufferSize);

    return &oac_dataPacket;
}

void oac_uri_obs_deleteAll(oac_uri_observer_t *subListHandle)
{
    for (uint8_t i = 0; i < OAC_URI_OBS_SUBSCRIBERS_MAX_NUM; i++)
    {
        memset(&subListHandle[i], 0, sizeof(subListHandle));
    }   
}
