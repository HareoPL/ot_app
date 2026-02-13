/**
 * @file ot_app_buffer.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 03-02-2026
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

#define OTAPP_BUF_INIT_CONGIG_IMPL

#include "ot_app_buffer.h"
#include "hro_utils.h"
#include "string.h"

#ifdef UNIT_TEST
    #ifdef TEST_PTHREAD
        #include "mock_freertos_semaphore_pthread.h"
    #else
        #include "mock_freertos_semaphore.h"
    #endif
#else
    #include "ot_app_port_rtos.h"
#endif

#define OTAPP_BUF_KEYS_QTY  (sizeof(otapp_buf_init_config) / sizeof(otapp_buf_init_config[0]))
typedef struct {
    uint16_t key;
    uint16_t offset;      // Where does the data start in a large array
    uint16_t max_size;    // How many has been reserved bytes
    uint16_t current_len; // How many actually written bytes there are
    uint8_t  write_lock;  // It is only locked when getWriteOnly_ptr() is using
} indexEntry_t;

typedef struct {
    indexEntry_t index[OTAPP_BUF_KEYS_QTY]; // Metadata array
    uint8_t      data[OTAPP_BUF_SIZE];      // Raw data
} otapp_buf_t;

static HRO_SEC_NOINIT_AL4 otapp_buf_t buf;

static SemaphoreHandle_t otapp_buf_mutex = NULL;

static int8_t otapp_buf_initKeysIndex(void) 
{
    uint16_t running_offset = 0; // count of position in memory

    // We clean everything at the start (Safety: deterministic start)
    memset(&buf, 0, sizeof(buf));

    for(uint8_t i = 0; i < OTAPP_BUF_KEYS_QTY; i++) 
    {
        // Kopiujemy klucz i wymagany rozmiar
        buf.index[i].key = otapp_buf_init_config[i].key;
        buf.index[i].max_size = otapp_buf_init_config[i].size;
        
        // Przypisujemy wyliczony offset
        buf.index[i].offset = running_offset;
        
        // Resetujemy aktualną długość danych (bo bufor jest pusty)
        buf.index[i].current_len = 0;

        //  Przesuwamy licznik dla NASTĘPNEGO elementu
        running_offset += otapp_buf_init_config[i].size;

        // SAFETY CHECK: Czy nie wyszliśmy poza fizyczny bufor 1024?
        if(running_offset > OTAPP_BUF_SIZE) 
        {
            // CRITICAL ERROR! Konfiguracja wymaga więcej pamięci niż mamy.
            return OTAPP_BUF_ERROR;
        }
    }
    return OTAPP_BUF_OK;
}

static int8_t otapp_buf_mutex_lock(void)
{
   if(xSemaphoreTake(otapp_buf_mutex, portMAX_DELAY) == pdTRUE)
   {
       return OTAPP_BUF_OK;
   }
   return OTAPP_BUF_ERROR;
}

static void otapp_buf_mutex_unlock(void)
{
    xSemaphoreGive(otapp_buf_mutex);
}

static indexEntry_t* otapp_buf_find_entry(uint16_t key)
{
    if(key == 0)
    {
        return NULL;
    }
    
    for (uint8_t i = 0; i < OTAPP_BUF_KEYS_QTY; i++)
    {
        if(buf.index[i].key == key)
        {
            return &buf.index[i];
        }
    }
    
    return NULL;
}

static int8_t otapp_buf_writeLock(uint16_t key)
{
    if(key == 0) return OTAPP_BUF_ERROR;

    indexEntry_t* entry = otapp_buf_find_entry(key);     
    if(entry == NULL) return OTAPP_BUF_ERROR_KEY_NOT_FOUND;

    entry->write_lock = 1;

    return OTAPP_BUF_OK;
}

int8_t otapp_buf_writeUnlock(uint16_t key)
{
    if(key == 0) return OTAPP_BUF_ERROR;

    indexEntry_t* entry = otapp_buf_find_entry(key);     
    if(entry == NULL) return OTAPP_BUF_ERROR_KEY_NOT_FOUND;

    // --- Critical section START ---
    if(otapp_buf_mutex_lock() != OTAPP_BUF_OK) return OTAPP_BUF_ERROR;
        entry->write_lock = 0;
        entry->current_len = 0;
    otapp_buf_mutex_unlock();
    // --- Critical section STOP ---
    return OTAPP_BUF_OK;
}

int8_t otapp_buf_append(uint16_t key, const uint8_t* new_data, uint16_t len) 
{
    if(new_data == NULL || len == 0 || key == 0) return OTAPP_BUF_ERROR;

    // Find the entry in index[] (fast loops over a small array)
    indexEntry_t* entry = otapp_buf_find_entry(key);     
    if(entry == NULL) return OTAPP_BUF_ERROR_KEY_NOT_FOUND;

    if(entry->write_lock) return OTAPP_BUF_ERROR_WRITE_LOCK;

    // --- Critical section START ---
    if(otapp_buf_mutex_lock() != OTAPP_BUF_OK) return OTAPP_BUF_ERROR;
        
    // Safety Check (Bounds Check)
        if(entry->current_len + len > entry->max_size) 
        {   
            otapp_buf_mutex_unlock();
            return OTAPP_BUF_ERROR_OVERFLOW; 
        }

        uint16_t write_pos = entry->offset + entry->current_len;   

        // Copying data
        memcpy(&buf.data[write_pos], new_data, len);

        // Update length
        entry->current_len += len;

    otapp_buf_mutex_unlock();
    // --- Critical section STOP ---

    return OTAPP_BUF_OK;
}

int8_t otapp_buf_getData(uint16_t key, uint8_t* bufOut, uint16_t bufSize, uint16_t *lenBufOut) 
{
    if(key == 0 || bufOut == NULL || lenBufOut == NULL || bufSize == 0) return OTAPP_BUF_ERROR;

    *lenBufOut = 0;

    // Find the entry in index[] (fast loops over a small array)
    indexEntry_t* entry = otapp_buf_find_entry(key);     
    if(entry == NULL) return OTAPP_BUF_ERROR_KEY_NOT_FOUND;

    // --- Critical section START ---
    if(otapp_buf_mutex_lock() != OTAPP_BUF_OK) return OTAPP_BUF_ERROR;

        uint16_t curLen = entry->current_len;
        if(curLen == 0 || curLen > bufSize)
        {            
            otapp_buf_mutex_unlock();
            return OTAPP_BUF_ERROR;
        }else
        {
            // Copying data
            memcpy(bufOut, &buf.data[entry->offset], curLen);
            
            // save length         
            *lenBufOut = curLen;
        }

    otapp_buf_mutex_unlock();
    // --- Critical section STOP ---

    return OTAPP_BUF_OK;
}

const uint8_t *otapp_buf_getReadOnly_ptr(uint16_t key, uint16_t *bufSize_out)
{
    if(bufSize_out == NULL)
    {        
        return NULL;
    }

    // Domyślnie zerujemy, żeby użytkownik nie dostał śmieci w razie błędu
    *bufSize_out = 0;

    if(key == 0) return NULL;

    // Find the entry in index[] (fast loops over a small array)
    indexEntry_t* entry = otapp_buf_find_entry(key); 
    if(entry == NULL) return NULL;
    
    // --- Critical section START ---
    if(otapp_buf_mutex_lock() != OTAPP_BUF_OK) return NULL;

        // getting current length
        *bufSize_out = entry->current_len;
    otapp_buf_mutex_unlock();
    // --- Critical section STOP ---

    const uint8_t *buffer = &buf.data[entry->offset];

    return buffer;    
}
 
uint8_t* otapp_buf_getWriteOnly_ptr(uint16_t key, uint16_t required_size) 
{
    if(key == 0 || required_size == 0) return NULL;

    indexEntry_t* entry = otapp_buf_find_entry(key);
    if(entry == NULL) return NULL;
    
    // --- Critical section START ---
    if (otapp_buf_mutex_lock() != OTAPP_BUF_OK) return NULL;

        // check write_lock
        if(entry->write_lock)
        {
            otapp_buf_mutex_unlock();
            return NULL; 
        }

        // Validation: Can we fit this data in the slot? 
        if(required_size > (entry->max_size)) 
        {
            otapp_buf_mutex_unlock();
            return NULL; 
        }
        
        // write will be locked for next call until call writeUnlock()
        otapp_buf_writeLock(key);

        // update length (reservation)
        entry->current_len = required_size; 
    otapp_buf_mutex_unlock();
    // --- Critical section STOP ---

    uint8_t *buffer = &buf.data[entry->offset];
    // return a pointer to the beginning of this key's data
    return buffer;
}

uint16_t otapp_buf_getMaxSize(uint16_t key)
{
    if(key == 0) return 0;

    indexEntry_t* entry = otapp_buf_find_entry(key);
    if(entry == NULL) return 0;

    return entry->max_size;
}

uint16_t otapp_buf_getCurrentLenSize(uint16_t key)
{
    if(key == 0) return 0;

    indexEntry_t* entry = otapp_buf_find_entry(key);
    if(entry == NULL) return 0;

    // --- Critical section START ---
    if(otapp_buf_mutex_lock() != OTAPP_BUF_OK) return 0;
        uint16_t curLen = entry->current_len;
    otapp_buf_mutex_unlock();
    // --- Critical section STOP ---

    return curLen;
}

int8_t otapp_buff_clear(uint16_t key)
{
    if(key == 0) return OTAPP_BUF_ERROR;

    // Find the entry in index[] (fast loops over a small array)
    indexEntry_t* entry = otapp_buf_find_entry(key);
    if(entry == NULL) return OTAPP_BUF_ERROR;

    // --- Critical section START ---
    if(otapp_buf_mutex_lock() != OTAPP_BUF_OK) return OTAPP_BUF_ERROR;

        // clear key data
        memset(&buf.data[entry->offset], 0, entry->max_size); 

        // update length
        entry->current_len = 0;
    otapp_buf_mutex_unlock();
    // --- Critical section STOP ---

    return OTAPP_BUF_OK;
}

void otapp_buffer_init(void)
{
    int8_t result;

    if(otapp_buf_mutex == NULL) 
    {
        otapp_buf_mutex = xSemaphoreCreateMutex();

        result = otapp_buf_initKeysIndex();
    
        if(otapp_buf_mutex == NULL || result == OTAPP_BUF_ERROR) 
        {       
            while(1);  // error
        }
    }
       
    
}