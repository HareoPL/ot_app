/**
 * @file mock_freertos_queue.c
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
#include "mock_freertos_queue.h"
#include "ot_app_pair.h"
#include "mock_ip6.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    otapp_pair_queueItem_t *items; 
    size_t length;
    size_t count;
    size_t head;
    size_t tail;
} MockQueue_t;

static MockQueue_t queue;
QueueHandle_t fq_mock_xQueueCreate(size_t length, size_t itemSize) 
{
    (void)itemSize; // not used
 

    queue.items = (otapp_pair_queueItem_t*) malloc(sizeof(otapp_pair_queueItem_t) * length);

    if (queue.items == NULL) 
    {        
        return NULL;
    }

    queue.length = length;
    queue.count = 0;
    queue.head = 0;
    queue.tail = 0;
    return (QueueHandle_t)&queue;
}

int fq_mock_xQueueSend(QueueHandle_t queueHandle, const void* item, int ticksToWait)
{
    (void)ticksToWait; // not used
    (void)queueHandle;

    if (queue.count == queue.length) 
    {
        return pdFALSE; // queue is full
    }

    memcpy(&queue.items[queue.tail], item, sizeof(otapp_pair_queueItem_t));

    queue.tail = (queue.tail + 1) % queue.length;
    queue.count++;

    return pdTRUE; 
}

int fq_mock_xQueueReceive(QueueHandle_t queueHandle, void* buffer, int ticksToWait)
{
    (void)ticksToWait; // not used
    (void)queueHandle;
    
    if (queue.count == 0) 
    {
        return pdFALSE; // there is no more elements
    }

    memcpy(buffer, &queue.items[queue.head], sizeof(otapp_pair_queueItem_t));

    queue.head = (queue.head + 1) % queue.length;
    queue.count--;

    return pdTRUE;
}

void fq_mock_QueueDeleteAllItems(void)
{
    queue.count = 0;
    queue.head = 0;
    queue.tail = 0;
}

size_t fq_mock_QueueGetItemQuantity(void)
{
    return queue.count;
}
