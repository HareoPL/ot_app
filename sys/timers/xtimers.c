/**
 * @file xtimers.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief  freeRtos timmers
 * @version 0.1
 * @date 15-04-2025
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
#include "main.h"
#include "xtimers.h"
#include "ws2812b_fx.h"

static TimerHandle_t xTim_print_task_listTimer;
static TimerHandle_t xTim_sysTick;

static char xTim_printBufferTaskList[512];

void xTim_printTaskList(TimerHandle_t xTimer); 
void xTim_sysTickMS(TimerHandle_t xTimer); 

static uint32_t currentTick = 0;

void xTim_Init(void)
{
    xTim_print_task_listTimer = xTimerCreate("print_task_listTimer", pdMS_TO_TICKS(1000), pdTRUE, NULL, xTim_printTaskList); // 1s
    

    xTim_sysTick = xTimerCreate("sysTickTimer", pdMS_TO_TICKS(1), pdTRUE, NULL, xTim_sysTickMS); // 1ms
    xTimerStart(xTim_sysTick, 0);
}

uint32_t xTim_getTick()
{
    return currentTick;
}

void xTim_sysTickMS(TimerHandle_t xTimer)
{
    currentTick ++;
    WS2812BFX_SysTickCallback();
}

void xTim_printTaskList(TimerHandle_t xTimer)
{
    vTaskList(xTim_printBufferTaskList);
    printf("Task Name\tState\tPrio\tStack\tNum\n");
    printf("%s\n", xTim_printBufferTaskList);
}

void xTim_printTaskListDisable(void)
{
    xTimerStop(xTim_print_task_listTimer, 0);
}

void xTim_printTaskListEnable(void)
{
    xTimerStart(xTim_print_task_listTimer, 0);
}