/**
 * @file mock_freertos_queue.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 01-09-2025
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
#ifndef MOCK_FREERTOS_QUEUE_H_
#define MOCK_FREERTOS_QUEUE_H_

#include "stdint.h"

#define xQueueCreate    fq_mock_xQueueCreate
#define xQueueSend      fq_mock_xQueueSend
#define xQueueReceive   fq_mock_xQueueReceive

typedef int BaseType_t;

#define pdFALSE       ( ( BaseType_t ) 0 )
#define pdTRUE        ( ( BaseType_t ) 1 )
#define portMAX_DELAY (TickType_t)  0xffffffffUL

typedef uint32_t TickType_t;
typedef void* QueueHandle_t;



QueueHandle_t fq_mock_xQueueCreate(size_t length, size_t itemSize);
int fq_mock_xQueueSend(QueueHandle_t queueHandle, const void* item, int ticksToWait);
int fq_mock_xQueueReceive(QueueHandle_t queueHandle, void* buffer, int ticksToWait);

void fq_mock_QueueDeleteAllItems(void);
size_t fq_mock_QueueGetItemQuantity(void);

#endif  /* MOCK_FREERTOS_QUEUE_H_ */
