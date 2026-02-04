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


#define OT_APP_MSG_TLV_SIZE             (sizeof(otapp_msg_tlv_t))
#define OT_APP_MSG_TLV_RESERVED_BYTES   2  // two first bytes in buffer has been reserved for pUsedBytes(used bytes in buffer for keys)

typedef struct {
    uint16_t key;
    uint16_t length;
} HRO_TOOL_PACKED_FIELD otapp_msg_tlv_t;

int8_t otapp_msg_tlv_writenBytesGet(uint8_t *buffer, const uint16_t bufferSize, uint16_t *writtenBytesOut)
{
    if(buffer == NULL || writtenBytesOut == NULL || bufferSize < (OT_APP_MSG_TLV_SIZE + OT_APP_MSG_TLV_RESERVED_BYTES))
    {
        return OT_APP_MSG_TLV_ERROR;
    } 
    uint16_t *writtenBytes = (uint16_t*)buffer; 
    *writtenBytesOut = *writtenBytes;

    return OT_APP_MSG_TLV_OK;
}   

static int8_t otapp_msg_tlv_writenBytesSet(uint8_t *buffer, const uint16_t bufferSize, uint16_t *writtenBytesIn)
{
    if(buffer == NULL || writtenBytesIn == NULL || bufferSize < (OT_APP_MSG_TLV_SIZE + OT_APP_MSG_TLV_RESERVED_BYTES))
    {
        return OT_APP_MSG_TLV_ERROR;
    } 

    uint16_t *writtenBytes = (uint16_t*)buffer;     
    *writtenBytes = *writtenBytesIn;

    return OT_APP_MSG_TLV_OK;
}   

int8_t otapp_msg_tlv_keyAdd(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, const uint16_t valueLengthIn, uint8_t *valueIn)
{
    if(buffer == NULL || valueIn == NULL || valueLengthIn == 0 || bufferSize < (OT_APP_MSG_TLV_SIZE + valueLengthIn + OT_APP_MSG_TLV_RESERVED_BYTES))
    {
        return OT_APP_MSG_TLV_ERROR;
    } 

    int8_t error;

    const uint16_t usableBufferSize = bufferSize - OT_APP_MSG_TLV_RESERVED_BYTES;
    const uint16_t newBlockLength = OT_APP_MSG_TLV_SIZE + valueLengthIn;
  
    uint16_t usedBytes = 0;

    error = otapp_msg_tlv_writenBytesGet(buffer, bufferSize, &usedBytes);
    if(error == OT_APP_MSG_TLV_ERROR || usedBytes > usableBufferSize) // check error and buffer overflow
    {
        return OT_APP_MSG_TLV_ERROR;
    } 
   
    uint8_t *pWrite = buffer + OT_APP_MSG_TLV_RESERVED_BYTES + usedBytes;  // Pointer to next free byte for writing
    
    if(newBlockLength <= (usableBufferSize - usedBytes)) // checking if there is free space in the buffer for new block of key
    {
        if(usedBytes > OT_APP_MSG_TLV_SIZE) // checking if there is anything in the buffer and if currently adding key does not already exist there
        {
            if(otapp_msg_tlv_keyGet(buffer, bufferSize, key, NULL, NULL) == OT_APP_MSG_TLV_KEY_EXIST)
            {
                return OT_APP_MSG_TLV_KEY_EXIST;
            }
        }

        otapp_msg_tlv_t * currentBlock = (otapp_msg_tlv_t *)pWrite;
        currentBlock->key = key;
        currentBlock->length = valueLengthIn;

        memcpy((uint8_t*)(pWrite + OT_APP_MSG_TLV_SIZE), valueIn, valueLengthIn);
        
        usedBytes += newBlockLength; // Update used bytes in buffer
        if(otapp_msg_tlv_writenBytesSet(buffer, bufferSize, &usedBytes) == OT_APP_MSG_TLV_ERROR)
        {
            return OT_APP_MSG_TLV_ERROR;
        }

        return OT_APP_MSG_TLV_OK;
    }else // there is no space for another key
    {
        error = OT_APP_MSG_TLV_ERROR_NO_SPACE;
    }

    return error;
}

int8_t otapp_msg_tlv_keyGet(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, uint16_t *valueLengthOut, uint8_t *valueOut)
{
    // if(buffer == NULL || valueOut == NULL || valueLengthOut == NULL || bufferSize < (OT_APP_MSG_TLV_SIZE + OT_APP_MSG_TLV_RESERVED_BYTES))
    if(buffer == NULL ||  bufferSize < (OT_APP_MSG_TLV_SIZE + OT_APP_MSG_TLV_RESERVED_BYTES))
    {
        return OT_APP_MSG_TLV_ERROR;
    } 

    const uint16_t usableBufferSize = bufferSize - OT_APP_MSG_TLV_RESERVED_BYTES; 
    otapp_msg_tlv_t *currentBlock = (otapp_msg_tlv_t*)(buffer + OT_APP_MSG_TLV_RESERVED_BYTES);

    uint16_t usedBytes;

    if(otapp_msg_tlv_writenBytesGet(buffer, bufferSize, &usedBytes) == OT_APP_MSG_TLV_ERROR)
    {
        return OT_APP_MSG_TLV_ERROR;
    } 

    if(usedBytes > usableBufferSize) // check for buffer overflow
    {
        return OT_APP_MSG_TLV_ERROR;    
    }

    if(usedBytes < OT_APP_MSG_TLV_SIZE + OT_APP_MSG_TLV_RESERVED_BYTES) // check empty buffer
    {
        return OT_APP_MSG_TLV_EMPTY_BUFFER;
    }

    uint16_t currentBytes = OT_APP_MSG_TLV_SIZE;

    while (currentBytes <= usedBytes)
    {
        if(currentBlock->key == key)
        {
            if(currentBlock->length > (usableBufferSize - currentBytes))
            {
                return OT_APP_MSG_TLV_ERROR;
            }
            
            if(valueOut == NULL || valueLengthOut == NULL)
            {
                return OT_APP_MSG_TLV_KEY_EXIST;
            }else
            {
                memcpy(valueOut, currentBlock + 1, currentBlock->length); // (currentBlock + 1) --> pointer arithmetic will point to the beginning of key value 
                *valueLengthOut = currentBlock->length;

                return OT_APP_MSG_TLV_KEY_EXIST;
            }            
        }else
        {
            currentBytes += currentBlock->length;                                                   // first, add length of key to currendBytes
            currentBlock = (otapp_msg_tlv_t *)((uint8_t *)currentBlock + currentBlock->length);     // next, increase ptr to next key data
        }
    }
    
    return OT_APP_MSG_TLV_KEY_NO_EXIST;
}

int8_t otapp_msg_tlv_freeBufSpaceGet(uint8_t *buffer, const uint16_t bufferSize, uint16_t *freeBufSpaceOut)
{
    if(buffer == NULL || freeBufSpaceOut == NULL || bufferSize < (OT_APP_MSG_TLV_SIZE + OT_APP_MSG_TLV_RESERVED_BYTES))
    {
        return OT_APP_MSG_TLV_ERROR;
    } 

    uint16_t writtenBytes = 0;
    uint16_t freeBufSpace = 0;

    if(otapp_msg_tlv_writenBytesGet(buffer, bufferSize, &writtenBytes) == OT_APP_MSG_TLV_ERROR)
    {
        return OT_APP_MSG_TLV_ERROR;
    }
    
    freeBufSpace = bufferSize - OT_APP_MSG_TLV_RESERVED_BYTES - writtenBytes;
    *freeBufSpaceOut = freeBufSpace;

    return OT_APP_MSG_TLV_OK;
}

// todo feature
// int8_t otapp_msg_tlv_keyDelete(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key)
// {

// }

// int8_t otapp_msg_tlv_keyUpdate(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, const uint16_t keyLenght, uint8_t *keyValue)
// {

// }

