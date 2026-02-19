/**
 * @file softTimers.c
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
 */

#include "softTimers.h"

void SoftTim_eventTask(softTim_t *timer)
{
    if((timer->delayTime != 0) && (SOFT_TIM_GET_MS() - timer->lastTick >= timer->delayTime))
    {
        timer->lastTick = SOFT_TIM_GET_MS();

        if(timer->action == NULL)
        {
            return;
        }else
        {
            timer->action();

            if(timer->doActionOnes)
            {
                timer->delayTime = 0;
            }
        }
    }
}

void SoftTim_init(softTim_t *timer, softTim_callback _action, uint8_t doActionOnes)
{
    timer->action = _action;
    timer->doActionOnes = doActionOnes;
}

void SoftTim_start(softTim_t *timer, uint32_t delay)
{
    if(delay != 0)
    {
        timer->delayTime = delay;
        timer->lastTick = SOFT_TIM_GET_MS();
    }
}

void SoftTim_stop(softTim_t *timer)
{
    timer->delayTime = 0;
}
