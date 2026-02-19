/**
 * @file ws2812b_drv_RMT.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief WS2812B driver implementation using ESP32 RMT peripheral
 * 
 * @version 0.1
 * @date 15-04-2025
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2025
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
 * @defgroup ws2812b_rmt WS2812B RMT Driver
 * @ingroup ws2812b_if
 * @{
 * 
 * @section ws2812b_rmt_desc Description
 * 
 * Hardware-specific implementation of WS2812B driver using ESP32-C6 RMT
 * (Remote Control Transceiver) peripheral with DMA support.
 * 
 * **Features:**
 * - Hardware-accurate WS2812B timing (0.3µs / 0.9µs)
 * - DMA-based transmission (no CPU intervention)
 * - Configurable GPIO and LED count
 * - Built-in gamma correction and sine lookup tables
 * 
 * **RMT Timing:**
 * - Bit 0: 0.3µs HIGH + 0.9µs LOW = 1.2µs total
 * - Bit 1: 0.9µs HIGH + 0.3µs LOW = 1.2µs total
 * - Reset: >50µs LOW
 * 
 * @section ws2812b_rmt_config Configuration
 * 
 * **Hardware Configuration:**
 * - GPIO: Defined by RMT_LED_STRIP_GPIO_NUM (default: GPIO 8)
 * - LED count: Defined by WS2812B_IF_LEDS (default: 5)
 * - RMT resolution: 10MHz (0.1µs tick)
 * 
 * **Example:**
 * @code{.c}
 * // Change configuration in ws2812b_drv_RMT.h:
 * #define WS2812B_IF_LEDS 50          // 50 LEDs
 * #define RMT_LED_STRIP_GPIO_NUM 15   // GPIO 15
 * @endcode
 * 
 * @section ws2812b_rmt_usage Usage Example
 * 
 * @code{.c}
 * #include "ws2812b_drv_RMT.h"
 * 
 * void app_main() {
 *     // Initialize RMT peripheral
 *     ws2812b_if_init();
 *     
 *     // Get driver interface
 *     const ws2812b_drv_t *drv = ws2812b_if_getDrvRMT();
 *     
 *     // Set all LEDs to red
 *     for (int i = 0; i < WS2812B_IF_LEDS; i++) {
 *         drv->SetOneDiodeRGB(i, 255, 0, 0);
 *     }
 *     
 *     // Update physical LEDs
 *     drv->Refresh();
 * }
 * @endcode
 */

#ifndef WS2812B_DRV_RMT_H_
#define WS2812B_DRV_RMT_H_

#include "ws2812b_if.h"

/**
 * @defgroup ws2812b_rmt_config Configuration Macros
 * @brief Hardware configuration for RMT driver
 * @{
 */

/**
 * @brief Number of WS2812B LEDs in strip
 * 
 * @details
 * Defines LED buffer size and maximum addressable LEDs.
 * 
 * **Memory Usage:** 3 bytes per LED + RMT buffers
 * 
 * **Typical Values:**
 * - Small projects: 5-10 LEDs
 * - Medium: 50-100 LEDs
 * - Large: 200-500 LEDs
 * 
 * @note Maximum limited by available RAM
 * @warning Higher values increase memory usage
 */
#define WS2812B_IF_LEDS 5

/**
 * @brief GPIO pin number for WS2812B data line
 * 
 * @details
 * ESP32-C6 GPIO pin connected to LED strip DI (Data In).
 * 
 * **Requirements:**
 * - Must support RMT output
 * - Must be available (not used by other peripherals)
 * 
 * **Common GPIOs on ESP32-C6:**
 * - GPIO 8: Default (can control WS2812B)
 * - GPIO 0-7, 9-21: Available for general use
 * 
 * @warning Avoid GPIOs used for flash/UART/JTAG
 */
#define RMT_LED_STRIP_GPIO_NUM 8

/**
 * @brief RMT peripheral clock resolution
 * 
 * @details
 * RMT clock frequency in Hz. Higher resolution = more accurate timing.
 * 
 * **Resolution:** 10MHz = 0.1µs tick
 * 
 * **WS2812B Timing Requirements:**
 * - 0.3µs = 3 ticks
 * - 0.9µs = 9 ticks
 * - 1.2µs = 12 ticks
 * 
 * @note 10MHz provides optimal accuracy for WS2812B
 */
#define RMT_LED_STRIP_RESOLUTION_HZ (10 * 1000 * 1000)

/**
 * @brief RMT duration for 0.3µs pulse
 * 
 * @details
 * Number of RMT ticks for 0.3µs duration.
 * Used for WS2812B bit timing.
 * 
 * **Calculation:** 0.3µs * 10MHz = 3 ticks
 * 
 * **Used For:**
 * - Bit 0: HIGH duration
 * - Bit 1: LOW duration
 */
#define RMT_TIME_03_US (0.3 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000)

/**
 * @brief RMT duration for 0.9µs pulse
 * 
 * @details
 * Number of RMT ticks for 0.9µs duration.
 * Used for WS2812B bit timing.
 * 
 * **Calculation:** 0.9µs * 10MHz = 9 ticks
 * 
 * **Used For:**
 * - Bit 0: LOW duration
 * - Bit 1: HIGH duration
 */
#define RMT_TIME_09_US (0.9 * RMT_LED_STRIP_RESOLUTION_HZ / 1000000)

/**
 * @}
 */

/**
 * @defgroup ws2812b_rmt_functions Public API Functions
 * @brief RMT driver initialization and control functions
 * @{
 */

/**
 * @brief Get RMT driver interface
 * 
 * @return const ws2812b_drv_t* Pointer to driver interface structure
 * 
 * @details
 * Returns function pointer structure for LED control.
 * Use this interface to interact with LEDs.
 * 
 * **Example:**
 * @code{.c}
 * const ws2812b_drv_t *drv = ws2812b_if_getDrvRMT();
 * drv->SetOneDiodeRGB(0, 255, 0, 0);
 * drv->Refresh();
 * @endcode
 * 
 * @note Must call ws2812b_if_init() before using driver
 */
const ws2812b_drv_t *ws2812b_if_getDrvRMT(void);

/**
 * @brief Initialize RMT peripheral for WS2812B control
 * 
 * @details
 * Initializes ESP32-C6 RMT peripheral with WS2812B-specific configuration.
 * 
 * **Initialization Steps:**
 * 1. Configure RMT bytes encoder (0.3µs / 0.9µs timing)
 * 2. Create RMT TX channel on specified GPIO
 * 3. Configure 64-symbol memory blocks (reduces flickering)
 * 4. Set up 4-deep transaction queue for smooth updates
 * 5. Enable RMT channel
 * 
 * **RMT Configuration:**
 * - Resolution: 10MHz (0.1µs per tick)
 * - Bit 0: 0.3µs HIGH + 0.9µs LOW
 * - Bit 1: 0.9µs HIGH + 0.3µs LOW
 * - MSB first transmission
 * - DMA enabled
 * 
 * @note Call once during application startup
 * @note Must be called before any LED operations
 * @warning Crashes if RMT channel allocation fails
 * 
 * @see ws2812b_if_getDrvRMT() to get driver interface
 */
void ws2812b_if_init(void);

/**
 * @brief Run simple LED test sequence
 * 
 * @details
 * Cycles through LEDs one by one in red, green, blue sequence.
 * Useful for testing LED strip connectivity and order.
 * 
 * **Test Sequence:**
 * 1. Light LED 0 red
 * 2. Light LED 1 red
 * 3. ... through all LEDs red
 * 4. Light LED 0 green
 * 5. ... through all LEDs green
 * 6. Light LED 0 blue
 * 7. ... through all LEDs blue
 * 8. Repeat
 * 
 * **Usage:**
 * @code{.c}
 * void test_task(void *pvParameters) {
 *     ws2812b_if_init();
 *     
 *     while(1) {
 *         ws2812b_if_simpleTest();
 *         vTaskDelay(pdMS_TO_TICKS(100));  // 100ms delay
 *     }
 * }
 * @endcode
 * 
 * @note Add to FreeRTOS task with delay
 * @note Static state maintained between calls
 * @warning Do not call faster than 10ms
 */
void ws2812b_if_simpleTest(void);

/**
 * @}
 */

#endif /* WS2812B_DRV_RMT_H_ */

/**
 * @}
 */
