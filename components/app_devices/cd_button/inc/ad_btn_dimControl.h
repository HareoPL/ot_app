/**
 * @file ad_btn_dimControl.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Brightness control and dimming logic for button device
 * @version 0.1
 * @date 06-11-2025
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
 * @defgroup device_butn_dim Button Device Brightness Control
 * @ingroup device_button
 * @{
 * 
 * @section dim_overview Overview
 * 
 * This module implements adaptive brightness control with configurable thresholds and step sizes.
 * The dimming behavior changes based on current brightness level - brighter levels use larger steps
 * for faster adjustment, while darker levels use smaller steps for fine control.
 * 
 * @section dim_algorithm Algorithm Description
 * 
 * The brightness adjustment uses two lookup tables:
 * - **threshTab**: Defines brightness range boundaries (thresholds)
 * - **stepsTab**: Defines adjustment step size for each range
 * 
 * When adjusting brightness, the algorithm:
 * 1. Determines current brightness range using threshTab
 * 2. Selects appropriate step size from stepsTab
 * 3. Increments/decrements brightness by that step
 * 
 * @section dim_customization Customizing Brightness Behavior
 * 
 * @subsection modify_thresholds Modifying Brightness Thresholds
 * 
 * Edit the `threshTab[]` array to change brightness range boundaries:
 * 
 * @code{.c}
 * // Default configuration (5 ranges)
 * static const uint8_t threshTab[OT_BTN_LEVELS_COUNT] = {30, 80, 160, 220, OT_BTN_MAX_BRIGHTNESS};
 * 
 * // Custom configuration - more ranges for finer control
 * #define OT_BTN_LEVELS_COUNT 7
 * static const uint8_t threshTab[OT_BTN_LEVELS_COUNT] = {20, 50, 100, 150, 200, 230, OT_BTN_MAX_BRIGHTNESS};
 * @endcode
 * 
 * @subsection modify_steps Modifying Brightness Steps
 * 
 * Edit the `stepsTab[]` array to change adjustment speed for each range:
 * 
 * @code{.c}
 * // Default configuration
 * static const uint8_t stepsTab[OT_BTN_LEVELS_COUNT] = {1, 4, 6, 8, 16};
 * 
 * // Custom - slower dimming for more precise control
 * static const uint8_t stepsTab[OT_BTN_LEVELS_COUNT] = {1, 2, 3, 4, 8};
 * 
 * // Custom - faster dimming for quick adjustment
 * static const uint8_t stepsTab[OT_BTN_LEVELS_COUNT] = {2, 8, 12, 16, 32};
 * @endcode
 * 
 * @subsection modify_limits Modifying Brightness Limits
 * 
 * Adjust minimum and maximum brightness values:
 * 
 * @code{.c}
 * #define OT_BTN_MAX_BRIGHTNESS 200  // Reduce maximum brightness to 78%
 * #define OT_BTN_MIN_BRIGHTNESS 10   // Increase minimum visible brightness
 * @endcode
 * 
 * @section dim_examples Configuration Examples
 * 
 * @subsection example_smooth Smooth Fine Control
 * @code{.c}
 * #define OT_BTN_LEVELS_COUNT 8
 * static const uint8_t threshTab[] = {15, 30, 60, 90, 120, 160, 200, 255};
 * static const uint8_t stepsTab[] = {1, 1, 2, 3, 4, 6, 8, 12};
 * @endcode
 * 
 * @subsection example_fast Fast Adjustment
 * @code{.c}
 * #define OT_BTN_LEVELS_COUNT 3
 * static const uint8_t threshTab[] = {85, 170, 255};
 * static const uint8_t stepsTab[] = {10, 20, 40};
 * @endcode
 * 
 * @note Both arrays must have exactly OT_BTN_LEVELS_COUNT elements
 * @note Last element of threshTab must equal OT_BTN_MAX_BRIGHTNESS
 * @note Array elements must be in ascending order
 * 
 * @warning After modifying tables, recompile the entire project to ensure consistency
 */

#ifndef AD_BTN_DIMCONTROL_H_
#define AD_BTN_DIMCONTROL_H_

#include "stdint.h"

/**
 * @defgroup dim_constants Brightness Constants
 * @{
 */
#define OT_BTN_MAX_BRIGHTNESS  255  ///< Maximum brightness value (full brightness)
#define OT_BTN_MIN_BRIGHTNESS  3    ///< Minimum brightness value (below this, light turns off)
#define OT_BTN_LEVELS_COUNT    5    ///< Number of brightness threshold levels
/**
 * @}
 */

/**
 * @brief Brightness threshold table
 * 
 * @details
 * Defines the upper boundary for each brightness range.
 * Current brightness level determines which step size from stepsTab[] is used.
 * 
 * Array structure:
 * - Index 0: Threshold for dimmest range (0 to threshTab[0])
 * - Index 1-3: Intermediate brightness ranges
 * - Index 4: Maximum brightness (must equal OT_BTN_MAX_BRIGHTNESS)
 * 
 * Default ranges:
 * - Range 0: 0-30 (very dim) → step 1
 * - Range 1: 31-80 (dim) → step 4
 * - Range 2: 81-160 (medium) → step 6
 * - Range 3: 161-220 (bright) → step 8
 * - Range 4: 221-255 (very bright) → step 16
 */
static const uint8_t threshTab[OT_BTN_LEVELS_COUNT] = {30, 80, 160, 220, OT_BTN_MAX_BRIGHTNESS};

/**
 * @brief Brightness step size table
 * 
 * @details
 * Defines how many brightness units to change per adjustment for each range.
 * Larger steps provide faster dimming at higher brightness levels.
 * 
 * Array structure parallels threshTab[]:
 * - stepsTab[i] is the step size used when brightness is in range defined by threshTab[i]
 * 
 * Default step sizes:
 * - Step 1: Fine adjustment for dim lighting
 * - Step 4-8: Moderate adjustment for mid-range
 * - Step 16: Fast adjustment at maximum brightness
 */
static const uint8_t stepsTab[OT_BTN_LEVELS_COUNT] = {1, 4, 6, 8, 16};

/**
 * @brief Calculate new brightness value based on current dim level
 * 
 * @param[in] dimLevel Current brightness level (0-255) or direction indicator
 * 
 * @return New brightness value after applying appropriate step adjustment
 * @retval 0 if brightness drops below OT_BTN_MIN_BRIGHTNESS (turns off)
 * @retval OT_BTN_MAX_BRIGHTNESS if brightness exceeds maximum
 * 
 * @details
 * This function implements the adaptive brightness algorithm:
 * 1. Identifies current brightness range using threshTab[]
 * 2. Selects step size from stepsTab[]
 * 3. Applies adjustment in dimming or brightening direction
 * 4. Ensures result stays within valid range
 * 
 * @note Function handles both dimming (decreasing) and brightening (increasing) operations
 */
uint8_t ad_btn_dim_GetNewValue(uint32_t dimLevel);

#endif /* AD_BTN_DIMCONTROL_H_ */

/**
 * @}
 */
