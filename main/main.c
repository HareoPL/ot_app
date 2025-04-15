/**
 * @file main.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 14-04-2025
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
#include "ws2812b_fx.h"
#include "ws2812b_if_RMT.h"



void print_task_list(TimerHandle_t xTimer); 
void sysTick(TimerHandle_t xTimer); 

char buffer[512];

void app_main(void)
{
    TimerHandle_t print_task_listTimer = xTimerCreate("print_task_listTimer", pdMS_TO_TICKS(1000), pdTRUE, NULL, print_task_list); // 1s
    xTimerStart(print_task_listTimer, 0);

    TimerHandle_t sysTickTimer = xTimerCreate("sysTickTimer", pdMS_TO_TICKS(1), pdTRUE, NULL, sysTick); // 1ms
    xTimerStart(sysTickTimer, 0);

    ws2812b_if_init();
    WS2812BFX_Init(ws2812b_if_getDrvRMT(), 1);
    
    WS2812BFX_SetSpeed(0, 100);	// Speed of segment 0
    WS2812BFX_SetColorRGB(0, 5,0,0);	// Set color 0
    WS2812BFX_SetMode(0, FX_MODE_COLOR_WIPE);	// Set mode segment 0

    WS2812BFX_Start(0);	// Start segment 0

    while (1) 
    {
        WS2812BFX_Callback();	// FX effects calllback
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/****************************************************
 *  freeRtos hooks
 */
void vApplicationTickHook(void) // calling from Irq
{
    
}

void vApplicationIdleHook(void)
{

}

/****************************************************
 *  freeRtos timmers
 */
void sysTick(TimerHandle_t xTimer)
{
    WS2812BFX_SysTickCallback();
}

void print_task_list(TimerHandle_t xTimer)
{
    vTaskList(buffer);
    printf("Task Name\tState\tPrio\tStack\tNum\n");
    printf("%s\n", buffer);
}