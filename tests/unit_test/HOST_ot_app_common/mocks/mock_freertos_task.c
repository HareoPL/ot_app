/**
 * @file mock_freertos_task.c
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
#include "mock_freertos_task.h"

#include <stdio.h>
#include <string.h>


// Definicje FreeRTOS (przykład)
typedef void (*TaskFunction_t)(void*);

typedef void* TaskHandle_t;
typedef int BaseType_t;
typedef unsigned int UBaseType_t;


// Struct do przechowywania argumentów ostatniego wywołania mocka (opcjonalnie)
typedef struct {
    TaskFunction_t pxTaskCode;
    const char* pcName;
    uint16_t usStackDepth;
    void* pvParameters;
    UBaseType_t uxPriority;
    TaskHandle_t* pxCreatedTask;
} xTaskCreateMockArgs_t;

xTaskCreateMockArgs_t last_xTaskCreate_args;

BaseType_t ft_mock_xTaskCreate(TaskFunction_t pxTaskCode,
                           const char* const pcName,
                           uint16_t usStackDepth,
                           void* pvParameters,
                           UBaseType_t uxPriority,
                           TaskHandle_t* pxCreatedTask)
{
    // Zapisujemy argumenty do celów testów / asercji
    last_xTaskCreate_args.pxTaskCode = pxTaskCode;
    last_xTaskCreate_args.pcName = pcName;
    last_xTaskCreate_args.usStackDepth = usStackDepth;
    last_xTaskCreate_args.pvParameters = pvParameters;
    last_xTaskCreate_args.uxPriority = uxPriority;
    last_xTaskCreate_args.pxCreatedTask = pxCreatedTask;

    // Można wygenerować fałszywy uchwyt zadania
    if(pxCreatedTask)
    {
        static int dummyTaskHandle; // prosty identyfikator
        *pxCreatedTask = &dummyTaskHandle;
    }

    // Nie uruchamiamy rzeczywistego zadania - tylko zwracamy sukces
    return pdPASS;
}

void *ft_mock_xTaskGetCurrentTaskHandle()
{
    return NULL;
}

void ft_mock_runTaskCallback()
{
    last_xTaskCreate_args.pxTaskCode(NULL);
}

