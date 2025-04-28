/**
 * @file utils.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 28-04-2025
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
#ifndef UTILS_H_
#define UTILS_H_

#include "main.h"

#define UTILS_ENABLE_CHECK_RTOS_FREE_STACK_ON_TASKS


#ifdef UTILS_ENABLE_CHECK_RTOS_FREE_STACK_ON_TASKS  
    #define UTILS_RTOS_CHECK_FREE_STACK() \
        do{ \
            const char *task_name = pcTaskGetName(NULL); \
            UBaseType_t stack_free = uxTaskGetStackHighWaterMark(NULL); \
            ESP_LOGI(TAG, "task %s free stack: %d ", task_name, stack_free); \
        }while(0)
#else
    #define UTILS_RTOS_CHECK_FREE_STACK() do{}while(0)
#endif 

#endif  /* UTILS_H_ */
