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

#include "ot_app_buffer.h"
#include "ot_app_port_rtos.h"
#include "hro_utils.h"
#include "string.h"

static HRO_SEC_NOINIT uint8_t otapp_buffer[OT_APP_BUFFER_SIZE];
static SemaphoreHandle_t otapp_buffer_mutex = NULL;

uint8_t *otapp_buffer_get_withMutex(void)
{
    if(xSemaphoreTake(otapp_buffer_mutex, portMAX_DELAY) == pdTRUE)
    {
        return otapp_buffer;
    }
}

void otapp_buffer_releaseMutex(void)
{
    xSemaphoreGive(otapp_buffer_mutex);
}

uint16_t otapp_buffer_getSize(void)
{
    return OT_APP_BUFFER_SIZE;
}

void otapp_buffer_clear(void)
{
    memset(otapp_buffer, 0, OT_APP_BUFFER_SIZE); 
}

void otapp_buffer_init(void)
{
    otapp_buffer_mutex = xSemaphoreCreateMutex();
    if (otapp_buffer_mutex == NULL) 
    {       
        while(1);  // error
    }
}