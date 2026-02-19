/**
 * @file ws2812b_fx.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief WS2812B lighting effects library - Ported from Arduino WS2812FX
 * @version 0.1
 * @date 15-04-2025
 * 
 * Library based on
 * https://github.com/lamik/WS2812B_STM32_HAL
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
 * @defgroup ws2812b_fx WS2812B Effects Library
 * @ingroup ws2812b_if
 * @{
 * 
 * @section ws2812b_fx_desc Description
 * 
 * Rich lighting effects library for WS2812B LEDs ported from Arduino WS2812FX.
 * Provides 58 predefined effects with segment support for independent control
 * of different LED strip sections.
 * 
 * **Key Features:**
 * - 58 predefined lighting effects
 * - Multi-segment support (independent strip sections)
 * - Adjustable speed per segment
 * - Reversible effects
 * - HSV and RGB color space support
 * - Configurable segment size
 * 
 * **Credits:**
 * - Original STM32 port: https://github.com/lamik/WS2812B_STM32_HAL by Mateusz Salamon
 * - Arduino WS2812FX: https://github.com/kitesurfer1404/WS2812FX by kitesurfer1404
 * 
 * @section ws2812b_fx_usage Basic Usage
 * 
 * @code{.c}
 * #include "ws2812b_fx.h"
 * #include "ws2812b_drv_RMT.h"
 * 
 * void app_main() {
 *     // Initialize hardware
 *     ws2812b_if_init();
 *     const ws2812b_drv_t *drv = ws2812b_if_getDrvRMT();
 *     
 *     // Initialize effects library with 1 segment
 *     WS2812BFX_Init(drv, 1);
 *     
 *     // Configure segment 0 (entire strip)
 *     WS2812BFX_SetSegmentSize(0, 0, WS2812B_IF_LEDS - 1);
 *     WS2812BFX_SetMode(0, FX_MODE_RAINBOW_CYCLE);
 *     WS2812BFX_SetSpeed(0, 500);
 *     WS2812BFX_Start(0);
 *     
 *     // Main loop
 *     while(1) {
 *         WS2812BFX_Callback();  // Process effects
 *         vTaskDelay(pdMS_TO_TICKS(10));
 *     }
 * }
 * @endcode
 * 
 * @section ws2812b_fx_segments Segment Support
 * 
 * Segments allow independent control of different LED strip sections:
 * 
 * @code{.c}
 * // Initialize with 2 segments
 * WS2812BFX_Init(drv, 2);
 * 
 * // Segment 0: LEDs 0-24 (rainbow effect)
 * WS2812BFX_SetSegmentSize(0, 0, 24);
 * WS2812BFX_SetMode(0, FX_MODE_RAINBOW_CYCLE);
 * WS2812BFX_Start(0);
 * 
 * // Segment 1: LEDs 25-49 (fire effect)
 * WS2812BFX_SetSegmentSize(1, 25, 49);
 * WS2812BFX_SetMode(1, FX_MODE_FIRE_FLICKER);
 * WS2812BFX_Start(1);
 * @endcode
 */

#ifndef WS2812B_FX_H_
#define WS2812B_FX_H_

#include "main.h"
#include "ws2812b_if.h"

/**
 * @defgroup ws2812b_fx_constants Constants and Macros
 * @brief Configuration constants and predefined colors
 * @{
 */

/**
 * @brief Default effect color
 * @note Currently set to invalid value (needs correction)
 */
#define DEFAULT_COLOR 0x00FF000000

/**
 * @brief Number of configurable colors per effect
 * @details Each effect can use up to 3 independent colors
 */
#define NUM_COLORS 3

/**
 * @brief Minimum effect speed value
 * @details Slowest possible effect speed (10 = very slow)
 */
#define SPEED_MIN 10

/**
 * @brief Default effect speed
 * @details Medium speed suitable for most effects
 */
#define DEFAULT_SPEED 150

/**
 * @brief Maximum effect speed value
 * @details Fastest possible effect speed (65535 = very fast)
 */
#define SPEED_MAX 65535

/**
 * @brief Total number of available effect modes
 * @details Library provides 58 different effects
 */
#define MODE_COUNT 58

/**
 * @brief Default effect mode
 * @details FX_MODE_STATIC (solid color)
 */
#define DEFAULT_MODE 0

/**
 * @brief Fade rate for fade effects
 * @details Controls fade speed (higher = faster fade)
 */
#define FADE_RATE 2

/**
 * @defgroup ws2812b_fx_colors Predefined Colors
 * @brief Common color constants in 0x00RRGGBB format
 * @{
 */

#define RED     (uint32_t)0xFF0000   ///< Pure red
#define GREEN   (uint32_t)0x00FF00   ///< Pure green
#define BLUE    (uint32_t)0x0000FF   ///< Pure blue
#define WHITE   (uint32_t)0xFFFFFF   ///< Pure white (all channels max)
#define BLACK   (uint32_t)0x000000   ///< Black (LEDs off)
#define YELLOW  (uint32_t)0xFFFF00   ///< Yellow (red + green)
#define CYAN    (uint32_t)0x00FFFF   ///< Cyan (green + blue)
#define MAGENTA (uint32_t)0xFF00FF   ///< Magenta (red + blue)
#define PURPLE  (uint32_t)0x400080   ///< Purple (dark magenta)
#define ORANGE  (uint32_t)0xFF3000   ///< Orange (red + some green)
#define PINK    (uint32_t)0xFF1493   ///< Pink (hot pink shade)

/**
 * @}
 */

/**
 * @}
 */

/**
 * @defgroup ws2812b_fx_types Type Definitions
 * @brief Enumerations and type definitions
 * @{
 */

/**
 * @brief Function return status codes
 */
typedef enum {
    FX_OK = 0,      ///< Operation successful
    FX_ERROR = 1    ///< Operation failed
} FX_STATUS;

/**
 * @brief Available lighting effect modes
 * 
 * @details
 * 58 different lighting effects organized by category:
 * - Static: Solid colors and gradual transitions
 * - Blink: Various blinking patterns
 * - Wipe: Progressive color fills
 * - Dynamic: Moving patterns
 * - Rainbow: Rainbow-based effects
 * - Chase: Theater chase and running patterns
 * - Twinkle: Random twinkling effects
 * - Sparkle: Sparkle and flash effects
 * - Scanner: Cylon/larson scanner effects
 * - Special: Holiday and themed effects
 */
typedef enum {
    FX_MODE_STATIC,                     ///< 0: Solid color (no animation)
    FX_MODE_WHITE_TO_COLOR,            ///< 1: Gradual fade from white to color
    FX_MODE_BLACK_TO_COLOR,            ///< 2: Gradual fade from black to color
    FX_MODE_BLINK,                     ///< 3: Simple on/off blink
    FX_MODE_BLINK_RAINBOW,             ///< 4: Blink through rainbow colors
    FX_MODE_STROBE,                    ///< 5: Fast strobe effect
    FX_MODE_STROBE_RAINBOW,            ///< 6: Rainbow colored strobe
    FX_MODE_BREATH,                    ///< 7: Smooth breathing effect
    FX_MODE_COLOR_WIPE,                ///< 8: Progressive color fill
    FX_MODE_COLOR_WIPE_INV,            ///< 9: Color wipe with inverse
    FX_MODE_COLOR_WIPE_REV,            ///< 10: Reverse color wipe
    FX_MODE_COLOR_WIPE_REV_INV,        ///< 11: Reverse color wipe inverse
    FX_MODE_COLOR_WIPE_RANDOM,         ///< 12: Random color wipe
    FX_MODE_COLOR_SWEEP_RANDOM,        ///< 13: Random color sweep
    FX_MODE_RANDOM_COLOR,              ///< 14: Random solid colors
    FX_MODE_SINGLE_DYNAMIC,            ///< 15: Single LED moving
    FX_MODE_MULTI_DYNAMIC,             ///< 16: Multiple LEDs moving
    FX_MODE_RAINBOW,                   ///< 17: Static rainbow
    FX_MODE_RAINBOW_CYCLE,             ///< 18: Rotating rainbow
    FX_MODE_FADE,                      ///< 19: Fade in/out
    FX_MODE_SCAN,                      ///< 20: Single scanner
    FX_MODE_DUAL_SCAN,                 ///< 21: Dual direction scanner
    FX_MODE_THEATER_CHASE,             ///< 22: Theater marquee chase
    FX_MODE_THEATER_CHASE_RAINBOW,     ///< 23: Rainbow theater chase
    FX_MODE_RUNNING_LIGHTS,            ///< 24: Wave pattern
    FX_MODE_TWINKLE,                   ///< 25: Random twinkling
    FX_MODE_TWINKLE_RANDOM,            ///< 26: Random color twinkles
    FX_MODE_TWINKLE_FADE,              ///< 27: Fading twinkles
    FX_MODE_TWINKLE_FADE_RANDOM,       ///< 28: Random fading twinkles
    FX_MODE_SPARKLE,                   ///< 29: Random white sparkles
    FX_MODE_FLASH_SPARKLE,             ///< 30: Flash with sparkles
    FX_MODE_HYPER_SPARKLE,             ///< 31: Intense sparkle effect
    FX_MODE_MULTI_STROBE,              ///< 32: Multi-color strobe
    FX_MODE_CHASE_WHITE,               ///< 33: White dot chase
    FX_MODE_CHASE_COLOR,               ///< 34: Colored dot chase
    FX_MODE_CHASE_RANDOM,              ///< 35: Random color chase
    FX_MODE_CHASE_RAINBOW,             ///< 36: Rainbow chase
    FX_MODE_CHASE_FLASH,               ///< 37: Chase with flash
    FX_MODE_CHASE_FLASH_RANDOM,        ///< 38: Random chase flash
    FX_MODE_CHASE_RAINBOW_WHITE,       ///< 39: Rainbow chase with white
    FX_MODE_CHASE_BLACKOUT,            ///< 40: Chase with blackout
    FX_MODE_CHASE_BLACKOUT_RAINBOW,    ///< 41: Rainbow chase blackout
    FX_MODE_RUNNING_COLOR,             ///< 42: Single color running
    FX_MODE_RUNNING_RED_BLUE,          ///< 43: Red and blue running
    FX_MODE_RUNNING_RANDOM,            ///< 44: Random colors running
    FX_MODE_LARSON_SCANNER,            ///< 45: Cylon/K.I.T.T. scanner
    FX_MODE_COMET,                     ///< 46: Comet with tail
    FX_MODE_FIREWORKS,                 ///< 47: Fireworks effect
    FX_MODE_FIREWORKS_RANDOM,          ///< 48: Random fireworks
    FX_MODE_MERRY_CHRISTMAS,           ///< 49: Christmas theme
    FX_MODE_FIRE_FLICKER,              ///< 50: Realistic fire
    FX_MODE_FIRE_FLICKER_SOFT,         ///< 51: Soft fire flicker
    FX_MODE_FIRE_FLICKER_INTENSE,      ///< 52: Intense fire flicker
    FX_MODE_CIRCUS_COMBUSTUS,          ///< 53: Circus colors
    FX_MODE_HALLOWEEN,                 ///< 54: Halloween theme
    FX_MODE_BICOLOR_CHASE,             ///< 55: Two-color chase
    FX_MODE_TRICOLOR_CHASE,            ///< 56: Three-color chase
    FX_MODE_ICU,                       ///< 57: Emergency lights (ICU/ambulance)
} fx_mode;

/**
 * @}
 */

FX_STATUS WS2812BFX_Init(const ws2812b_drv_t *drv, uint16_t Segments);
FX_STATUS WS2812BFX_SegmentIncrease(ws2812b_drv_t *drv);
FX_STATUS WS2812BFX_SegmentDecrease(ws2812b_drv_t *drv);
uint8_t WS2812BFX_GetSegmentsQuantity(void);

void WS2812BFX_SysTickCallback(void);
void WS2812BFX_Callback(void);

FX_STATUS WS2812BFX_ForceAllColor(uint16_t Segment, uint8_t r, uint8_t g, uint8_t b);

FX_STATUS WS2812BFX_Start(uint16_t Segment);
FX_STATUS WS2812BFX_Stop(uint16_t Segment);
FX_STATUS WS2812BFX_IsRunning(uint16_t Segment, uint8_t *Running);

FX_STATUS WS2812BFX_SetMode(uint16_t Segment, fx_mode Mode);
FX_STATUS WS2812BFX_GetMode(uint16_t Segment, fx_mode *Mode);
FX_STATUS WS2812BFX_NextMode(uint16_t Segment);
FX_STATUS WS2812BFX_PrevMode(uint16_t Segment);
FX_STATUS WS2812BFX_SetReverse(uint16_t Segment, uint8_t Reverse);
FX_STATUS WS2812BFX_GetReverse(uint16_t Segment, uint8_t *Reverse);

FX_STATUS WS2812BFX_SetSegmentSize(uint16_t Segment, uint16_t Start, uint16_t Stop);
FX_STATUS WS2812BFX_GetSegmentSize(uint16_t Segment, uint16_t *Start, uint16_t *Stop);
FX_STATUS WS2812BFX_SegmentIncreaseEnd(uint16_t Segment);
FX_STATUS WS2812BFX_SegmentDecreaseEnd(uint16_t Segment);
FX_STATUS WS2812BFX_SegmentIncreaseStart(uint16_t Segment);
FX_STATUS WS2812BFX_SegmentDecreaseStart(uint16_t Segment);

FX_STATUS WS2812BFX_SetSpeed(uint16_t Segment, uint16_t Speed);
FX_STATUS WS2812BFX_GetSpeed(uint16_t Segment, uint16_t *Speed);
FX_STATUS WS2812BFX_IncreaseSpeed(uint16_t Segment, uint16_t Speed);
FX_STATUS WS2812BFX_DecreaseSpeed(uint16_t Segment, uint16_t Speed);

void WS2812BFX_SetColorStruct(uint8_t id, ws2812b_color c);
void WS2812BFX_SetColorRGB(uint8_t id, uint8_t r, uint8_t g, uint8_t b);
FX_STATUS WS2812BFX_GetColorRGB(uint8_t id, uint8_t *r, uint8_t *g, uint8_t *b);
void WS2812BFX_SetColorHSV(uint8_t id, uint16_t h, uint8_t s, uint8_t v);
void WS2812BFX_SetColor(uint8_t id, uint32_t c);

FX_STATUS WS2812BFX_SetAll(uint16_t Segment, uint32_t c);
FX_STATUS WS2812BFX_SetAllRGB(uint16_t Segment, uint8_t r, uint8_t g, uint8_t b);

void WS2812BFX_RGBtoHSV(uint8_t r, uint8_t g, uint8_t b, uint16_t *h, uint8_t *s, uint8_t *v);
void WS2812BFX_HSVtoRGB(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b);


#endif  /* WS2812B_FX_H_ */
