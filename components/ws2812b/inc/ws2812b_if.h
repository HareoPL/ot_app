/**
 * @file ws2812b_if.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief WS2812B driver interface - Hardware abstraction layer
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
 * @defgroup ws2812b_if WS2812B Interface Layer
 * @ingroup libs
 * @{
 * 
 * @section ws2812b_if_desc Description
 * 
 * Hardware abstraction layer for WS2812B LED driver providing platform-independent
 * interface through function pointers. Allows different hardware implementations
 * (RMT, SPI, etc.) to be used interchangeably.
 * 
 * @section ws2812b_if_usage Usage
 * 
 * @code{.c}
 * // Get driver instance
 * const ws2812b_drv_t *driver = ws2812b_if_getDrvRMT();
 * 
 * // Set LED color
 * driver->SetOneDiodeRGB(0, 255, 0, 0);  // LED 0 = Red
 * 
 * // Update strip
 * driver->Refresh();
 * @endcode
 */

#ifndef WS2812B_IF_H_
#define WS2812B_IF_H_

#include "stdint.h"

/**
 * @brief RGB color structure
 * 
 * @details
 * Represents single LED color in RGB format with GRB byte order
 * (matching WS2812B hardware requirements).
 * 
 * **Memory Layout:**
 * - green: First byte sent to LED
 * - red: Second byte sent to LED  
 * - blue: Third byte sent to LED
 * 
 * @note WS2812B uses GRB order, not RGB!
 */
typedef struct
{
    uint8_t green;  ///< Green channel (0-255), sent first
    uint8_t red;    ///< Red channel (0-255), sent second
    uint8_t blue;   ///< Blue channel (0-255), sent third
} ws2812b_color;

/**
 * @brief WS2812B driver interface structure
 * 
 * @details
 * Function pointer interface for hardware-independent LED control.
 * Different hardware implementations (RMT, SPI, PWM) provide their
 * own implementations of these functions.
 * 
 * **Design Pattern:** Strategy pattern for hardware abstraction
 * 
 * **Usage:**
 * @code{.c}
 * const ws2812b_drv_t *drv = ws2812b_if_getDrvRMT();
 * drv->SetOneDiodeRGB(0, 255, 0, 0);
 * drv->Refresh();
 * @endcode
 */
typedef struct
{
    /**
     * @brief Update physical LEDs with buffered data
     * 
     * @details
     * Transmits LED buffer to strip. Implementation varies by hardware:
     * - RMT: Uses DMA transfer
     * - SPI: Uses SPI with DMA
     * - PWM: Uses PWM modulation
     */
    void (*Refresh)(void);
    
    /**
     * @brief Get sine wave value for given angle
     * 
     * @param[in] x Angle (0-255 representing 0-360 degrees)
     * @return uint8_t Sine value (0-255)
     * 
     * @details
     * Lookup table-based sine calculation for smooth animations.
     * Used internally by effects library.
     */
    uint8_t (*sine8)(uint8_t x);
    
    /**
     * @brief Get pointer to LED pixel buffer
     * 
     * @return uint8_t* Pointer to LED buffer (3 bytes per LED)
     * 
     * @details
     * Direct access to LED buffer for advanced operations.
     * 
     * @warning Modifying buffer directly requires Refresh() call
     */
    uint8_t* (*GetPixels)(void);
    
    /**
     * @brief Get color of specific LED
     * 
     * @param[in] DiodeID LED index (0-based)
     * @return uint32_t Color in 0x00RRGGBB format
     * 
     * @details
     * Returns buffered color value, not physical LED state.
     */
    uint32_t (*GetColor)(int16_t DiodeID);
    
    /**
     * @brief Get total number of LEDs
     * 
     * @return uint16_t Number of LEDs in strip
     * 
     * @details
     * Returns LED count configured during initialization.
     */
    uint16_t (*GetNumberOfLeds)(void);
    
    /**
     * @brief Set LED color using 32-bit value
     * 
     * @param[in] DiodeID LED index (0-based)
     * @param[in] color Color in 0x00RRGGBB format
     * 
     * @details
     * Sets LED color in buffer. Call Refresh() to update physical LEDs.
     * 
     * **Color Format:**
     * - Bits 16-23: Red
     * - Bits 8-15: Green
     * - Bits 0-7: Blue
     * 
     * @note Does not validate DiodeID range
     */
    void (*SetOneDiode)(int16_t DiodeID, uint32_t color);
    
    /**
     * @brief Set LED color using separate RGB values
     * 
     * @param[in] DiodeID LED index (0-based)
     * @param[in] R Red value (0-255)
     * @param[in] G Green value (0-255)
     * @param[in] B Blue value (0-255)
     * 
     * @details
     * Convenience function for setting LED color.
     * Internally converts to 32-bit format and calls SetOneDiode().
     */
    void (*SetOneDiodeRGB)(int16_t DiodeID, uint8_t R, uint8_t G, uint8_t B);
    
    /**
     * @brief Set LED color using color structure
     * 
     * @param[in] DiodeID LED index (0-based)
     * @param[in] colorStruct Color structure (ws2812b_color)
     * 
     * @details
     * Direct assignment of color structure to LED buffer.
     * Most efficient method for setting color.
     */
    void (*SetDiodeColorStruct)(int16_t DiodeID, ws2812b_color colorStruct);
    
} ws2812b_drv_t;

#endif /* WS2812B_IF_H_ */

/**
 * @}
 */
