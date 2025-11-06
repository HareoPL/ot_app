/**
 * @file softTimers.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief software timers - opjective library
 * @version 0.1
 * @date 30-11-2024
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2024 
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
 * example to us it
 *  
 * softTim_t uart_timer;
 *
 * void uart_task(void)
 * {
 *    LPUART1_SendString("test\r\n");
 * }
 * int main(void)
 * {
 *     SoftTim_init(&uart_timer, uart_task, 0);
 *     SoftTim_start(&uart_timer, 700);
 *
 *     while(1)
 *     {
 *        SoftTim_event(&uart_timer);         
 *     }
 * }
 */
#ifndef SOFTTIMERS_H_
#define SOFTTIMERS_H_

#include "main.h"

#define SOFT_TIM_GET_TICK()     xTaskGetTickCount()
#define SOFT_TIM_GET_MS()       pdTICKS_TO_MS(SOFT_TIM_GET_TICK())

typedef void (*softTim_callback)(void);

typedef struct
{
    uint32_t delayTime;
    uint32_t lastTick;
    softTim_callback action;
    uint8_t doActionOnes : 1;
}softTim_t;
    
/**
 * @brief Check event. It shoul be in infinity loop
 * 
 * @param timer pointer to softTim_t struct
 */
void SoftTim_eventTask(softTim_t *timer);

/**
 * @brief added action function
 * 
 * @param timer pointer to softTim_t struct
 * @param _action pointer to action function 
 * @param doActionOnes if 1 - action will call only one time
 */
void SoftTim_init(softTim_t *timer, softTim_callback action, uint8_t doActionOnes);

/**
 * @brief set delay time
 * 
 * @param timer pointer to softTim_t struct
 * @param delay delay time in ms
 */
void SoftTim_start(softTim_t *timer, uint32_t delay);

/**
 * @brief stop timmer
 * 
 * @param timer pointer to softTim_t struct
 */
void SoftTim_stop(softTim_t *timer);

#endif  /* SOFTTIMERS_H_ */
