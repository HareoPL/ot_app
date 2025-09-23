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
    if(subListHandle == NULL || subListIndex >= OAC_URI_OBS_SUBSCRIBERS_MAX_NUM)
    {
        return OAC_URI_OBS_ERROR;
    }

    subListHandle[subListIndex].takenPosition = 1;
    return OAC_URI_OBS_OK;
}

PRIVATE int8_t oac_uri_obs_spaceIsTaken(oac_uri_observer_t *subListHandle, uint8_t subListIndex)
{ 
    if(subListHandle == NULL || subListIndex >= OAC_URI_OBS_SUBSCRIBERS_MAX_NUM)
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

PRIVATE int8_t oac_uri_obs_checkTableIsInit(uint8_t *tabPtr, uint16_t tabSize)
{
    for (uint16_t i = 0; i < tabSize; i++) 
    {
        if (tabPtr[i] != 0) 
        {
            return OAC_URI_OBS_IS;
        }
    }
    return OAC_URI_OBS_IS_NOT;
}

PRIVATE int8_t oac_uri_obs_subscribeIsValidData(oac_uri_observer_t *subListHandle, oac_uri_observer_t *subscribeData)
{
    if(subListHandle == NULL || subscribeData == NULL)
    {
        return OAC_URI_OBS_ERROR; 
    }
    
    if(subscribeData->serverData.uriIndex_client == 0 || subscribeData->serverData.uriIndex_server == 0 )
    {
        return OAC_URI_OBS_ERROR;
    }

    if(oac_uri_obs_checkTableIsInit(subscribeData->serverData.ipAddr.mFields.m8, sizeof(subscribeData->serverData.ipAddr)) == OAC_URI_OBS_IS_NOT)
    {
        return OAC_URI_OBS_ERROR;
    }

    if(oac_uri_obs_checkTableIsInit(subscribeData->serverData.token, sizeof(subscribeData->serverData.token)) == OAC_URI_OBS_IS_NOT)
    {
        return OAC_URI_OBS_ERROR;
    }

    return OAC_URI_OBS_IS;
}

int8_t oac_uri_obs_subscribe(oac_uri_observer_t *subListHandle, oac_uri_observer_t *subscribeData)
{
    oacu_result_t result_;
    if(oac_uri_obs_subscribeIsValidData(subListHandle, subscribeData) != OAC_URI_OBS_IS)
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
            return OAC_URI_OBS_OK;
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

    if(result_ == OAC_URI_OBS_ERROR)
    {
        return OAC_URI_OBS_ERROR;
    }
    
    if(result_ != OAC_URI_OBS_IS_NOT)
    {
        subListHandle[result_].takenPosition = 0;
        memset(&subListHandle[result_].serverData, 0, sizeof(subListHandle[result_].serverData));
        return OAC_URI_OBS_OK;     
    }

    return OAC_URI_OBS_TOKEN_NOT_EXIST;
}

int8_t oac_uri_obs_notify(oac_uri_observer_t *subListHandle, oacu_uriIndex_t serverUri, const uint8_t *dataToNotify, uint16_t dataSize)
{
    if(subListHandle == NULL || dataToNotify == NULL || serverUri == 0)
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
// int8_t oac_uri_obs_sendSubscribeRequest(const otIp6Address *ipAddr, const char *aUriPath)
// {
//     void otapp_coapSendPutSubscribeRequest(ipAddr, aUriPath);
//     return OAC_URI_OBS_OK;
// }

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

int8_t oac_uri_obs_deleteAll(oac_uri_observer_t *subListHandle)
{
    if(subListHandle == NULL)
    {
        return OAC_URI_OBS_ERROR;
    }

    for (uint8_t i = 0; i < OAC_URI_OBS_SUBSCRIBERS_MAX_NUM; i++)
    {
        memset(&subListHandle[i], 0, sizeof(subListHandle[0]));
    }
    
    return OAC_URI_OBS_OK;
}


#ifdef UNIT_TEST
    
    oac_uri_observer_t test_obs={
        .takenPosition = 1,
        .serverData.ipAddr.mFields.m8 = {
                        0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
                        0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34},
        .serverData.token = {
                        0xFA, 0x04, 0xB6, 0xD1},
        .serverData.uriIndex_client = 1,
        .serverData.uriIndex_server = 2,
    };

    int8_t test_obs_fillListExampleData(oac_uri_observer_t *subListHandle)
    {
        for (uint8_t i = 0; i < OAC_URI_OBS_SUBSCRIBERS_MAX_NUM; i++)
        {
           memcpy(&subListHandle[i], &test_obs, sizeof(test_obs));
        }
        
        return OAC_URI_OBS_OK;
    }
#endif


/////////////////////////

//  void regobserv()
//  {
//     // otCoapMessageGenerateToken();
//     uint8_t otCoapMessageGetTokenLength(const otMessage *aMessage); 
// const uint8_t *otCoapMessageGetToken(const otMessage *aMessage);
    
//  }

//  #include <openthread/coap.h>
//  // register observer 
// static void CoapRequestHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
// {
//     otError error;
//     otMessage *response;

//     response = otCoapNewMessage((otInstance *)aContext, NULL);
//     if (response == NULL)
//     {
//         return;
//     }

//     error = otCoapMessageInitResponse(response, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CONTENT);
//     if (error != OT_ERROR_NONE)
//     {
//         otMessageFree(response);
//         return;
//     }

//     // Generujemy token zgodny z tokenem w zapytaniu klienta (zazwyczaj kopiujemy token z zapytania)
//     error = otCoapCopyMessageToken(response, aMessage);


//     // Dodanie opcji Observe do odpowiedzi - numer sekwencji powiadomień
//     otCoapMessageAppendObserveOption(response, 0);

//     error = otCoapSendResponse((otInstance *)aContext, response, aMessageInfo);
//     if (error != OT_ERROR_NONE)
//     {
//         otMessageFree(response);
//     }
// }
// 


// #include <openthread/coap.h>

// static void HandleCoapResponse(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo, otError aError)
// {
//     if (aError == OT_ERROR_NONE && aMessage != NULL)
//     {
//         // Obsługa odpowiedzi serwera - np. potwierdzenie rejestracji observe
//     }
//     else
//     {
//         // Obsługa błędu
//     }
// }

// void SendCoapObserveRequest(otInstance *aInstance, const otIp6Address *aServerAddress)
// {
//     otError error;
//     otMessage *message;
//     otMessageInfo messageInfo;
//     uint8_t tokenLength = 4; // długość tokena (np. 4 bajty)

//     // Tworzymy nową wiadomość CoAP typu Confirmable (CON)
//     message = otCoapNewMessage(aInstance, NULL);
//     if (message == NULL)
//     {
//         return;
//     }

//     // Inicjujemy zapytanie z metodą GET
//     otCoapMessageInit(message, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_GET);
    
//     // Ustawiamy URI zasobu do obserwacji, np. "my/observe/resource"
//     error = otCoapMessageAppendUriPathOption(message, "my");
//     error = otCoapMessageAppendUriPathOption(message, "observe");
//     error = otCoapMessageAppendUriPathOption(message, "resource");
//     if (error != OT_ERROR_NONE)
//     {
//         otMessageFree(message);
//         return;
//     }

//     // Generujemy token dla obserwacji (unikalny)
//    otCoapMessageGenerateToken(message, tokenLength);
    

//     // Dodajemy opcję Observe z wartością 0, co oznacza rejestrację obserwatora
//     error = otCoapMessageAppendObserveOption(message, 0);
//     if (error != OT_ERROR_NONE)
//     {
//         otMessageFree(message);
//         return;
//     }

//     // Przygotowujemy adres wysyłki (IPv6 + port CoAP - domyślnie 5683)
//     memset(&messageInfo, 0, sizeof(messageInfo));
//     messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;
//     memcpy(&messageInfo.mPeerAddr, aServerAddress, sizeof(otIp6Address));

//     // Wysyłamy żądanie do serwera CoAP
//     error = otCoapSendRequest(aInstance, message, &messageInfo, HandleCoapResponse, NULL);
//     if (error != OT_ERROR_NONE)
//     {
//         otMessageFree(message);
//         return;
//     }
// }
