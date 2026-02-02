/**
 * @file ot_app_msg_tlv.c
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
#include "ot_app_msg_tlv.h"
#include "hro_utils.h"
#include "string.h"
 /*
ot_app_tlv.c
TLV to skrót od "Tag-Length-Value"

typedef struct __attribute__((packed)){
    char uri[OTAPP_URI_MAX_NAME_LENGHT];     
    otapp_deviceType_t devTypeUriFn;    // It tell you what functions this uri has
    uint8_t obs;                        // is this uri has observer support 
}otapp_pair_resUrisParseData_t;


trzeba to zrobic na zasadzie kluczy

funkcja tworzaca bufer z kluczami
funkcja czytajaca konkretne klucze z danego bufora

dla uri resource beda potrzebne kludze
1 klucz ilosc uris
2 klucz uri_name, 
3 klucz uri_devtype
4 klucz uri_obs

czyli 1 klucz bedzie mowil ile razy beda podane klucze od 2 do 4 po sobie. 
*/

// PRIVATE uint8_t otapp_msg_tlv_buffer[MSG_TLV_BUFFER_SIZE];

#define OT_APP_MSG_TLV_SIZE             (sizeof(otapp_msg_tlv_t))
#define OT_APP_MSG_TLV_RESERVED_BYTES   2  // 2 bytes on the end of the buffer are reserved for pUsedBytes (used bytes in buffer for keys)

typedef struct {
    uint16_t key;
    uint16_t length;
} HRO_TOOL_PACKED_FIELD otapp_msg_tlv_t;

    // to bedzie dzialac przy zalozeniach ze mamy wyczyszczony buffer. czyli przydala by sie osobna funkcja dla uzytkownika do przygotowania buffera ?
    // a moze trzeba tutaj dac glowny buffer a dalej aplikacja bedzie albo kopiowac w celu zachowania albo poprostu zostanie wyslane. wtedy trzeba muteksa dac na cala operacje. 
int8_t otapp_msg_tlv_keyAdd(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, const uint16_t valueLengthIn, uint8_t *valueIn)
{
    if(buffer == NULL || valueIn == NULL || valueLengthIn == 0 || bufferSize < (OT_APP_MSG_TLV_SIZE + valueLengthIn + OT_APP_MSG_TLV_RESERVED_BYTES))
    {
        return OT_APP_MSG_TLV_ERROR;
    } 

    int8_t error;

    const uint16_t usableBufferSize = bufferSize - OT_APP_MSG_TLV_RESERVED_BYTES;
    const uint16_t newBlockLength = OT_APP_MSG_TLV_SIZE + valueLengthIn;

    uint8_t *pDataEnd = buffer + usableBufferSize;   // End of buffer for tokens
    uint16_t *pUsedBytes = (uint16_t*)(pDataEnd);    // Number of bytes stored in buffer. it is always saved to last 2 bytes in buffer.

    if(*pUsedBytes > usableBufferSize || *pUsedBytes < OT_APP_MSG_TLV_SIZE)
    {
        return OT_APP_MSG_TLV_ERROR;    
    }

    uint8_t *pWrite = buffer + *pUsedBytes;          // Pointer to next free byte for writing
    
    if(newBlockLength <= (usableBufferSize - *pUsedBytes)) 
    {
        otapp_msg_tlv_t * currentBlock = (otapp_msg_tlv_t *)pWrite;
        currentBlock->key = key;
        currentBlock->length = valueLengthIn;

        memcpy((uint8_t*)(pWrite + OT_APP_MSG_TLV_SIZE), valueIn, valueLengthIn);
        
        *pUsedBytes += newBlockLength; // Update used bytes in buffer

        return OT_APP_MSG_TLV_OK;
    }else
    {
        error = OT_APP_MSG_TLV_ERROR;
    }

    return error;
}



int8_t otapp_msg_tlv_keyGet(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, uint16_t *valueLengthOut, uint8_t *valueOut)
{
    if(buffer == NULL || valueOut == NULL || valueLengthOut == NULL || bufferSize < (OT_APP_MSG_TLV_SIZE + OT_APP_MSG_TLV_RESERVED_BYTES))
    {
        return OT_APP_MSG_TLV_ERROR;
    } 

    const uint16_t usableBufferSize = bufferSize - OT_APP_MSG_TLV_RESERVED_BYTES; 
    otapp_msg_tlv_t *currentBlock = (otapp_msg_tlv_t*)buffer;

    uint8_t *pDataEnd = buffer + usableBufferSize;   // End of buffer for tokens
    uint16_t *pUsedBytes = (uint16_t*)(pDataEnd);    // Number of bytes stored in buffer. it is always saved to last 2 bytes in buffer.
    
    if(*pUsedBytes > usableBufferSize || *pUsedBytes < OT_APP_MSG_TLV_SIZE)
    {
        return OT_APP_MSG_TLV_ERROR;    
    }

    uint16_t currentBytes = OT_APP_MSG_TLV_SIZE;

    while (currentBytes <= *pUsedBytes)
    {
        if(currentBlock->key == key)
        {
            if(currentBlock->length > (usableBufferSize - currentBytes))
            {
                return OT_APP_MSG_TLV_ERROR;
            }

            memcpy(valueOut, currentBlock + 1, currentBlock->length); // (currentBlock + 1) --> pointer arithmetic will point to the beginning of key value 
            *valueLengthOut = currentBlock->length;

            return OT_APP_MSG_TLV_OK;
        }else
        {
            currentBytes += currentBlock->length;                                                   // first, add length of key to currendBytes
            currentBlock = (otapp_msg_tlv_t *)((uint8_t *)currentBlock + currentBlock->length);     // next, increase ptr to next key data
        }
    }
    
    return OT_APP_MSG_TLV_ERROR;
}

// int8_t otapp_msg_tlv_keyDelete(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key)
// {

// }

// int8_t otapp_msg_tlv_keyUpdate(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, const uint16_t keyLenght, uint8_t *keyValue)
// {

// }