/**
 * @file softTimers.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Software timers - Lightweight callback-based timer library for FreeRTOS
 * @version 0.1
 * @date 30-11-2024
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2024
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @defgroup softtimers Software Timers
 * @ingroup libs
 * @{
 * 
 * @section softtim_overview Overview
 * 
 * Software Timers is a lightweight, callback-based timer library for embedded systems
 * using FreeRTOS. It provides non-blocking timer functionality with support for both
 * one-shot and repeating timers.
 * 
 * **Key Features:**
 * - ✅ Non-blocking timer operation
 * - ✅ Callback-based event handling
 * - ✅ One-shot and repeating timer modes
 * - ✅ Millisecond precision
 * - ✅ FreeRTOS integration
 * - ✅ Minimal memory footprint
 * - ✅ Multiple independent timer instances
 * 
 * @section softtim_usage Basic Usage
 * 
 * **One-Shot Timer Example:**
 * @code{.c}
 * #include "softTimers.h"
 * 
 * // Define timer instance
 * softTim_t uart_timer;
 * 
 * // Callback function
 * void uart_task(void) {
 *     LPUART1_SendString("Timer expired!\r\n");
 * }
 * 
 * int main(void) {
 *     // Initialize timer with callback (one-shot mode)
 *     SoftTim_init(&uart_timer, uart_task, 1);
 *     
 *     // Start timer with 700ms delay
 *     SoftTim_start(&uart_timer, 700);
 *     
 *     while(1) {
 *         // Process timer events
 *         SoftTim_eventTask(&uart_timer);
 *     }
 * }
 * @endcode
 * 
 * **Repeating Timer Example:**
 * @code{.c}
 * softTim_t led_timer;
 * 
 * void led_toggle(void) {
 *     HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
 * }
 * 
 * int main(void) {
 *     // Initialize repeating timer (doActionOnes = 0)
 *     SoftTim_init(&led_timer, led_toggle, 0);
 *     
 *     // Start with 500ms interval
 *     SoftTim_start(&led_timer, 500);
 *     
 *     while(1) {
 *         SoftTim_eventTask(&led_timer);
 *         // Callback is called every 500ms
 *     }
 * }
 * @endcode
 * 
 * @section softtim_multi_timer Multiple Timers Example
 * 
 * @code{.c}
 * softTim_t timer1, timer2, timer3;
 * 
 * void task1(void) { // Task 1 code  }
 * void task2(void) { // Task 2 code  }
 * void task3(void) { // Task 3 code  }
 * 
 * int main(void) {
 *     // Initialize multiple timers
 *     SoftTim_init(&timer1, task1, 0);  // Repeating, every 100ms
 *     SoftTim_init(&timer2, task2, 0);  // Repeating, every 500ms
 *     SoftTim_init(&timer3, task3, 1);  // One-shot, 2000ms
 *     
 *     SoftTim_start(&timer1, 100);
 *     SoftTim_start(&timer2, 500);
 *     SoftTim_start(&timer3, 2000);
 *     
 *     while(1) {
 *         SoftTim_eventTask(&timer1);
 *         SoftTim_eventTask(&timer2);
 *         SoftTim_eventTask(&timer3);
 *     }
 * }
 * @endcode
 * 
 * @section softtim_operation Timer Operation
 * 
 * **Timer Lifecycle:**
 * ```
 * 1. SoftTim_init()    → Initialize timer with callback
 * 2. SoftTim_start()   → Start timer with delay
 * 3. SoftTim_eventTask() → Check and execute (call in main loop)
 *    ↓
 *    ├─ Repeating mode (doActionOnes=0) → Loop back to step 3
 *    └─ One-shot mode (doActionOnes=1)  → Timer stops automatically
 * 4. SoftTim_stop()    → Manual stop (optional)
 * ```
 * 
 * @note This library uses FreeRTOS tick count for timing
 * @note All timers must be processed regularly via SoftTim_eventTask()
 * @warning Callback functions should be short and non-blocking
 * 
 * @see SoftTim_init() for timer initialization
 * @see SoftTim_start() for starting timers
 * @see SoftTim_eventTask() for timer processing
 */

#ifndef SOFTTIMERS_H_
#define SOFTTIMERS_H_

// #include "main.h"
#include "ot_app_port_rtos.h"

/**
 * @defgroup softtim_macros Timing Macros
 * @brief Macros for FreeRTOS tick count access
 * @{
 */

/**
 * @brief Get current FreeRTOS tick count
 * @return uint32_t Current tick count
 * @note Uses FreeRTOS xTaskGetTickCount() function
 */
#define SOFT_TIM_GET_TICK() xTaskGetTickCount()

/**
 * @brief Get current time in milliseconds
 * @return uint32_t Current time in milliseconds since system start
 * @note Converts FreeRTOS ticks to milliseconds using pdTICKS_TO_MS()
//  */
// #define SOFT_TIM_GET_MS() pdTICKS_TO_MS(SOFT_TIM_GET_TICK())
#define SOFT_TIM_GET_MS() ( ( TickType_t ) ( ( uint64_t ) ( SOFT_TIM_GET_TICK() ) * 1000 / configTICK_RATE_HZ ) ) 

/**
 * @}
 */

/**
 * @defgroup softtim_types Type Definitions
 * @brief Type definitions for software timers
 * @{
 */

/**
 * @brief Callback function type for timer events
 * 
 * @details
 * Function signature for timer callback functions.
 * 
 * **Requirements:**
 * - Must be non-blocking
 * - Should execute quickly
 * - No parameters, no return value
 * 
 * **Example:**
 * @code{.c}
 * void my_timer_callback(void) {
 *     // Quick, non-blocking code
 *     GPIO_TogglePin(LED_Port, LED_Pin);
 * }
 * @endcode
 */
typedef void (*softTim_callback)(void);

/**
 * @brief Software timer structure
 * 
 * @details
 * Contains all state and configuration for a software timer instance.
 * Each timer requires its own softTim_t instance.
 * 
 * **Structure Fields:**
 * - `delayTime`: Timer interval in milliseconds (0 = stopped)
 * - `lastTick`: Last time callback was executed (milliseconds)
 * - `action`: Callback function pointer
 * - `doActionOnes`: Mode flag (1 = one-shot, 0 = repeating)
 * 
 * **Memory Usage:** 10 bytes per timer instance
 * 
 * @note Do not modify structure fields directly - use provided API functions
 */
typedef struct
{
    uint32_t delayTime;           ///< Timer delay in milliseconds (0 = timer stopped)
    uint32_t lastTick;            ///< Last execution time in milliseconds
    softTim_callback action;      ///< Callback function pointer (NULL = no action)
    uint8_t doActionOnes : 1;     ///< Timer mode: 1 = one-shot, 0 = repeating
} softTim_t;

/**
 * @}
 */

/**
 * @defgroup softtim_functions Public API Functions
 * @brief Software timer control functions
 * @{
 */

/**
 * @brief Process timer event and execute callback if time has elapsed
 * 
 * @param[in,out] timer Pointer to timer structure
 * 
 * @details
 * This function must be called periodically (typically in main loop or task).
 * It checks if the timer delay has elapsed and executes the callback if needed.
 * 
 * **Operation:**
 * 1. Check if timer is running (delayTime != 0)
 * 2. Check if elapsed time >= delayTime
 * 3. If yes, execute callback (if not NULL)
 * 4. Update lastTick for next interval
 * 5. If one-shot mode, stop timer automatically
 * 
 * **Call Frequency:**
 * - Should be called frequently (every 1-10ms recommended)
 * - Faster polling = better timing accuracy
 * - Must be faster than shortest timer interval
 * 
 * **Example:**
 * @code{.c}
 * softTim_t myTimer;
 * 
 * void app_task(void) {
 *     while(1) {
 *         SoftTim_eventTask(&myTimer);  // Process timer
 *         vTaskDelay(pdMS_TO_TICKS(1)); // 1ms delay
 *     }
 * }
 * @endcode
 * 
 * @note Non-blocking function - returns immediately
 * @note Safe to call even if timer is stopped
 * @warning Timer pointer must be valid and initialized
 */
void SoftTim_eventTask(softTim_t *timer);

/**
 * @brief Initialize timer with callback and mode
 * 
 * @param[out] timer Pointer to timer structure to initialize
 * @param[in] action Callback function pointer (or NULL for no action)
 * @param[in] doActionOnes Timer mode: 1 = one-shot, 0 = repeating
 * 
 * @details
 * Initializes a timer instance with callback function and operating mode.
 * 
 * **Timer Modes:**
 * - **One-shot** (doActionOnes = 1): Callback executes once, then timer stops
 * - **Repeating** (doActionOnes = 0): Callback executes repeatedly at interval
 * 
 * **Callback Requirements:**
 * - Can be NULL (timer will run but do nothing)
 * - Must be non-blocking
 * - Should execute quickly (<1ms recommended)
 * - Can be changed later by calling SoftTim_init() again
 * 
 * **Example - One-shot:**
 * @code{.c}
 * softTim_t delayTimer;
 * 
 * void delayed_action(void) {
 *     printf("Delayed action executed!\n");
 * }
 * 
 * SoftTim_init(&delayTimer, delayed_action, 1);  // One-shot
 * SoftTim_start(&delayTimer, 1000);              // Execute once after 1 second
 * @endcode
 * 
 * **Example - Repeating:**
 * @code{.c}
 * softTim_t ledTimer;
 * 
 * void toggle_led(void) {
 *     LED_Toggle();
 * }
 * 
 * SoftTim_init(&ledTimer, toggle_led, 0);  // Repeating
 * SoftTim_start(&ledTimer, 500);           // Execute every 500ms
 * @endcode
 * 
 * @note Timer is not started by this function - call SoftTim_start() to begin
 * @note Can be called multiple times to reconfigure timer
 */
void SoftTim_init(softTim_t *timer, softTim_callback action, uint8_t doActionOnes);

/**
 * @brief Start or restart timer with specified delay
 * 
 * @param[in,out] timer Pointer to timer structure
 * @param[in] delay Timer interval in milliseconds (must be > 0)
 * 
 * @details
 * Starts a timer with the specified delay interval.
 * If timer is already running, it is restarted with new delay.
 * 
 * **Behavior:**
 * - Sets timer interval to `delay` milliseconds
 * - Captures current time as starting point
 * - Timer becomes active immediately
 * - Previous timing is reset
 * 
 * **Valid Delay Range:**
 * - Minimum: 1ms (practical minimum depends on call frequency)
 * - Maximum: ~49 days (uint32_t milliseconds)
 * 
 * **Usage Patterns:**
 * 
 * **Pattern 1 - Start timer:**
 * @code{.c}
 * SoftTim_init(&timer, callback, 0);
 * SoftTim_start(&timer, 1000);  // Start with 1 second interval
 * @endcode
 * 
 * **Pattern 2 - Restart with different interval:**
 * @code{.c}
 * SoftTim_start(&timer, 500);   // Change to 500ms interval
 * @endcode
 * 
 * **Pattern 3 - Restart from beginning:**
 * @code{.c}
 * SoftTim_start(&timer, timer_delay);  // Reset timing
 * @endcode
 * 
 * @note If delay is 0, timer will not start (safety feature)
 * @note Timer must be initialized with SoftTim_init() before starting
 * @warning Do not call with delay = 0 if you want timer to run
 */
void SoftTim_start(softTim_t *timer, uint32_t delay);

/**
 * @brief Stop timer immediately
 * 
 * @param[in,out] timer Pointer to timer structure
 * 
 * @details
 * Stops a running timer immediately. Timer will not execute callback
 * until restarted with SoftTim_start().
 * 
 * **Operation:**
 * - Sets delayTime to 0 (timer inactive)
 * - Callback will not be called
 * - Timer state is preserved (callback, mode)
 * - Can be restarted anytime with SoftTim_start()
 * 
 * **Use Cases:**
 * - Temporarily disable timer
 * - Stop repeating timer
 * - Cancel pending one-shot timer
 * - Conditional timer control
 * 
 * **Example:**
 * @code{.c}
 * softTim_t timer;
 * 
 * // Start timer
 * SoftTim_init(&timer, callback, 0);
 * SoftTim_start(&timer, 1000);
 * 
 * // Later, stop timer based on condition
 * if (stop_condition) {
 *     SoftTim_stop(&timer);
 * }
 * 
 * // Restart when needed
 * SoftTim_start(&timer, 1000);
 * @endcode
 * 
 * @note Safe to call on already stopped timer
 * @note Does not deallocate or reset timer structure
 * @note Timer can be restarted without re-initialization
 */
void SoftTim_stop(softTim_t *timer);

/**
 * @}
 */

#endif /* SOFTTIMERS_H_ */

/**
 * @}
 */
