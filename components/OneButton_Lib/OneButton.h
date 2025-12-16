/**
 * @file OneButton.h
 * @author Jan Łukaszewicz
 * @brief Platform-portable button detection library with multi-click and long-press support
 * @version 1.0
 * @date Nov 19, 2022
 * 
 * @defgroup onebutton_libs OneButton library
 * @ingroup libs
 * @{
 * @section onebutton_overview Overview
 * 
 * OneButton is a button event detection library originally inspired by Arduino OneButton,
 * ported to bare-metal embedded systems (STM32, ESP32).
 * 
 * **Features:**
 * - Single-click detection
 * - Double-click detection
 * - Long-press detection with start/stop events
 * - Hardware debouncing
 * - Configurable timing parameters
 * - Non-blocking, poll-based operation
 * - Multi-button support
 * 
 * @section onebutton_platform Platform Selection
 * 
 * **Select your target platform by defining ONE of the following:**
 * - `STM_PLATFORM` - For STM32 microcontrollers with HAL
 * - `ESP_PLATFORM` - For ESP32 microcontrollers with ESP-IDF
 * 
 * **Example:**
 * @code{.c}
 * // For STM32:
 * #define STM_PLATFORM
 * 
 * // For ESP32:
 * #define ESP_PLATFORM
 * @endcode
 * 
 * @warning Define ONLY ONE platform macro at a time
 * 
 * @section onebutton_platforms Platform-Specific Configuration
 * 
 * @subsection stm32_config STM32 Platform (HAL-based)
 * 
 * **Macros:**
 * - `OB_READ_PIN()` → `HAL_GPIO_ReadPin(Btn->GpioPort, Btn->GpioPin)`
 * - `OB_GET_TICK()` → `HAL_GetTick()` (1ms SysTick)
 * - `OB_BUTTON_PRESSED` → `GPIO_PIN_RESET` (0 - active low)
 * - `OB_BUTTON_NOT_PRESSED` → `GPIO_PIN_SET` (1 - pull-up)
 * 
 * **Initialization:**
 * @code{.c}
 * OneButton_t myButton;
 * OneButtonInit(&myButton, GPIOA, GPIO_PIN_0);  // PA0 with GPIO port
 * @endcode
 * 
 * **GPIO Configuration Requirements:**
 * @code{.c}
 * GPIO_InitTypeDef GPIO_InitStruct = {0};
 * GPIO_InitStruct.Pin = GPIO_PIN_0;
 * GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 * GPIO_InitStruct.Pull = GPIO_PULLUP;  // For active-low buttons
 * HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 * @endcode
 * 
 * @subsection esp32_config ESP32 Platform (ESP-IDF)
 * 
 * **Required Headers:**
 * - `xtimers.h` - Custom timer wrapper
 * - `ot_app.h` - Application header
 * 
 * **Macros:**
 * - `OB_READ_PIN()` → `gpio_get_level(Btn->GpioPin)`
 * - `OB_GET_TICK()` → `xTim_getTick()` (custom millisecond timer)
 * - `OB_BUTTON_PRESSED` → `0` (active low)
 * - `OB_BUTTON_NOT_PRESSED` → `1` (pull-up)
 * 
 * **Initialization:**
 * @code{.c}
 * OneButton_t myButton;
 * OneButtonInit(&myButton, GPIO_NUM_9);  // GPIO 9 (no port needed)
 * @endcode
 * 
 * **GPIO Configuration Requirements:**
 * @code{.c}
 * gpio_config_t io_conf = {
 *     .pin_bit_mask = (1ULL << GPIO_NUM_9),
 *     .mode = GPIO_MODE_INPUT,
 *     .pull_up_en = GPIO_PULLUP_ENABLE,  // Enable pull-up for active-low
 *     .pull_down_en = GPIO_PULLDOWN_DISABLE,
 *     .intr_type = GPIO_INTR_DISABLE
 * };
 * gpio_config(&io_conf);
 * @endcode
 * 
 * @section onebutton_usage Usage Example
 * 
 * @code{.c}
 * #include "OneButton.h"
 * 
 * // Define button instance
 * OneButton_t button1;
 * 
 * // Callback functions
 * void onSingleClick(uint16_t btnNum) {
 *     printf("Button %d: Single click\n", btnNum);
 * }
 * 
 * void onDoubleClick(uint16_t btnNum) {
 *     printf("Button %d: Double click\n", btnNum);
 * }
 * 
 * void onLongPressStart(uint16_t btnNum) {
 *     printf("Button %d: Long press started\n", btnNum);
 * }
 * 
 * void onLongPressStop(uint16_t btnNum) {
 *     printf("Button %d: Long press stopped\n", btnNum);
 * }
 * 
 * void setup() {
 *     // Initialize button
 *     #ifdef STM_PLATFORM
 *         OneButtonInit(&button1, GPIOA, GPIO_PIN_0);
 *     #endif
 *     #ifdef ESP_PLATFORM
 *         OneButtonInit(&button1, GPIO_NUM_9);
 *     #endif
 *     
 *     // Optional: Configure custom timing
 *     OneButtonSetTimerDebounce(&button1, 10);        // 10ms debounce
 *     OneButtonSetTimerDoubleClick(&button1, 350);    // 350ms double-click window
 *     OneButtonSetTimerLongPressStart(&button1, 700); // 700ms for long-press
 *     OneButtonSetTimerLongPressTick(&button1, 500);  // 500ms long-press repeat
 *     
 *     // Register callbacks
 *     OneButtonCallbackOneClick(&button1, onSingleClick);
 *     OneButtonCallbackDoubleClick(&button1, onDoubleClick);
 *     OneButtonCallbackLongPressStart(&button1, onLongPressStart);
 *     OneButtonCallbackLongPressStop(&button1, onLongPressStop);
 * }
 * 
 * void loop() {
 *     OneButtonTask(&button1);  // Call periodically (every 10-50ms recommended)
 * }
 * @endcode
 * 
 * @section onebutton_porting Porting to New Platforms
 * 
 * To add support for a new platform:
 * 
 * 1. **Add platform definition** at top of OneButton.h:
 *    @code{.c}
 *    #define OB_MYPLATFORM
 *    @endcode
 * 
 * 2. **Add platform-specific macros block**:
 *    @code{.c}
 *    #ifdef OB_MYPLATFORM
 *    #define OB_READ_PIN()            my_gpio_read(Btn->GpioPin)
 *    #define OB_GET_TICK()            my_get_millis()
 *    #define OB_BUTTON_PRESSED        0
 *    #define OB_BUTTON_NOT_PRESSED    1
 *    #endif
 *    @endcode
 * 
 * 3. **Update OneButton_t structure** if needed (e.g., add port field)
 * 
 * 4. **Add platform-specific initialization** in OneButton.c:
 *    @code{.c}
 *    #ifdef OB_MYPLATFORM
 *    void OneButtonInit(OneButton_t *Btn, uint16_t GpioPin) {
 *        // Initialization code
 *    }
 *    #endif
 *    @endcode
 * 
 * @see OneButton.c for implementation details
 * @see ad_button.h for integration with OpenThread application
 */

#ifndef INC_ONEBUTTON_H_
#define INC_ONEBUTTON_H_

#include "driver/gpio.h" // for esp


// #include "main.h"

/**
 * @defgroup onebutton_platform_select Platform Selection
 * @brief Define ONE of these macros to select target platform
 * @{
 */

/**
 * @brief Enable STM32 platform support (HAL-based)
 */
// #define STM_PLATFORM

/**
 * @brief Enable ESP32 platform support (ESP-IDF)
 */
// #define ESP_PLATFORM

/**
 * @}
 */

/**
 * @defgroup onebutton_platform_macros Platform-Specific Macros
 * @brief GPIO and timing macros adapted per platform
 * @{
 */

#ifdef STM_PLATFORM

/**
 * @brief Read GPIO pin state (STM32 HAL)
 * @return GPIO_PinState (GPIO_PIN_RESET or GPIO_PIN_SET)
 */
#define OB_READ_PIN()            HAL_GPIO_ReadPin(Btn->GpioPort, Btn->GpioPin)

/**
 * @brief Get system tick in milliseconds (STM32 HAL)
 * @return uint32_t System time in milliseconds
 */
#define OB_GET_TICK()            HAL_GetTick()

/**
 * @brief Logic level when button is pressed (STM32 - active low with pull-up)
 */
#define OB_BUTTON_PRESSED        GPIO_PIN_RESET

/**
 * @brief Logic level when button is not pressed (STM32 - pull-up high)
 */
#define OB_BUTTON_NOT_PRESSED    GPIO_PIN_SET

#endif // STM_PLATFORM

#ifdef ESP_PLATFORM

#include "xtimers.h"
#include "ot_app.h"

/**
 * @brief Read GPIO pin state (ESP32 ESP-IDF)
 * @return int GPIO level (0 or 1)
 */
#define OB_READ_PIN()            gpio_get_level(Btn->GpioPin)

/**
 * @brief Get system tick in milliseconds (ESP32 custom timer)
 * @return uint32_t System time in milliseconds
 * @note Uses custom xTim_getTick() wrapper around ESP-IDF timer
 */
#define OB_GET_TICK()            xTim_getTick()

/**
 * @brief Logic level when button is pressed (ESP32 - active low with pull-up)
 */
#define OB_BUTTON_PRESSED        0

/**
 * @brief Logic level when button is not pressed (ESP32 - pull-up high)
 */
#define OB_BUTTON_NOT_PRESSED    1

#endif // ESP_PLATFORM

/**
 * @}
 */

/**
 * @defgroup onebutton_callbacks Callback Function Types
 * @brief Function pointer types for button event callbacks
 * @{
 */

/**
 * @brief Callback function type for single-click event
 * @param btnNum Button GPIO pin number passed to callback
 * 
 * @note Current signature: void callback(uint16_t btnNum)
 * @note Previous signature: void callback(void) - commented out
 * @note If you need different signature, modify typedef but keep the name
 */
typedef void(*CallBackFunOneClick_t)(uint16_t btnNum);

/**
 * @brief Callback function type for double-click event
 * @param btnNum Button GPIO pin number passed to callback
 */
typedef void(*CallBackFunDoubleClick_t)(uint16_t btnNum);

/**
 * @brief Callback function type for long-press start event
 * @param btnNum Button GPIO pin number passed to callback
 */
typedef void(*CallBackFunLongPressStart_t)(uint16_t btnNum);

/**
 * @brief Callback function type for long-press stop event
 * @param btnNum Button GPIO pin number passed to callback
 */
typedef void(*CallBackFunLongPressStop_t)(uint16_t btnNum);

/**
 * @}
 */

/**
 * @defgroup onebutton_types State Machine Types
 * @brief Enumerations and structures for button state management
 * @{
 */

/**
 * @brief Button state machine states
 * 
 * @details
 * State machine flow:
 * ```
 * IDLE (waiting)
 *   ↓ (button pressed)
 * DEBOUNCE (wait for stable state)
 *   ↓ (debounce time elapsed & still pressed)
 * DECIDE (determine click type)
 *   ↓ (press duration < long-press threshold & button released)
 *   ├─→ ONECLICK (if no second click) → IDLE
 *   ├─→ DOUBLECLICK (if second click detected) → IDLE
 *   └─→ LONGPRESS (if held > threshold) → IDLE (when released)
 * ```
 */
typedef enum
{
    IDLE = 0,         ///< Waiting for button press
    DEBOUNCE,         ///< Debouncing button press
    DECIDE,           ///< Deciding between click types
    ONECLICK,         ///< Single-click detected
    DOUBLECLICK,      ///< Double-click detected
    LONGPRESS         ///< Long-press detected
} ButtonState;

/**
 * @brief Button instance structure
 * 
 * @details
 * Contains all state, timing, and callback information for a single button.
 * Each physical button requires its own OneButton_t instance.
 * 
 * **Platform-specific fields:**
 * - STM32: Contains GpioPort (GPIO_TypeDef*) and GpioPin (uint16_t)
 * - ESP32: Contains only GpioPin (uint16_t)
 */
typedef struct
{
    ButtonState State;                          ///< Current state machine state
    
#ifdef STM_PLATFORM
    GPIO_TypeDef* GpioPort;                     ///< [STM32 only] GPIO port (e.g., GPIOA, GPIOB)
#endif
    
    uint16_t GpioPin;                           ///< GPIO pin number
    uint8_t CoundClick;                         ///< Click counter for multi-click detection
    uint32_t LastTick;                          ///< Last time button state changed (milliseconds)
    
    uint32_t TimerDebounce;                     ///< Debounce time in milliseconds (default: 10ms)
    uint32_t TimerDoubleClick;                  ///< Double-click window in milliseconds (default: 350ms)
    uint32_t TimerLongPressStart;               ///< Time to trigger long-press start (default: 700ms)
    uint32_t TimerLongPressTick;                ///< Repeat interval during long-press (default: 500ms)
    
    CallBackFunOneClick_t OneClick;             ///< Single-click callback function pointer
    CallBackFunDoubleClick_t DoubleClick;       ///< Double-click callback function pointer
    CallBackFunLongPressStart_t LongPressStart; ///< Long-press start callback function pointer
    CallBackFunLongPressStop_t LongPressStop;   ///< Long-press stop callback function pointer
} OneButton_t;

/**
 * @}
 */

/**
 * @defgroup onebutton_functions Public API Functions
 * @brief Initialization, configuration, and task functions
 * @{
 */

/**
 * @brief Initialize OneButton instance
 * 
 * @details
 * Initializes button structure with default timing values:
 * - State: IDLE
 * - Click counter: 0
 * - Debounce time: 10ms
 * - Double-click window: 350ms
 * - Long-press start: 700ms
 * - Long-press tick: 500ms
 * - All callbacks: NULL
 * 
 * **Platform-specific signatures:**
 */
#ifdef STM_PLATFORM
/**
 * @brief Initialize button (STM32 version)
 * @param[in,out] Btn Pointer to OneButton_t structure to initialize
 * @param[in] GpioPort GPIO port (e.g., GPIOA, GPIOB)
 * @param[in] GpioPin GPIO pin number (e.g., GPIO_PIN_0)
 */
void OneButtonInit(OneButton_t *Btn, GPIO_TypeDef *GpioPort, uint16_t GpioPin);
#endif

#ifdef ESP_PLATFORM
/**
 * @brief Initialize button (ESP32 version)
 * @param[in,out] Btn Pointer to OneButton_t structure to initialize
 * @param[in] GpioPin GPIO pin number (e.g., GPIO_NUM_9)
 */
void OneButtonInit(OneButton_t *Btn, uint16_t GpioPin);
#endif

/**
 * @brief Process button state machine (call periodically)
 * 
 * @param[in,out] Btn Pointer to OneButton_t structure
 * 
 * @details
 * This function implements the button state machine and should be called
 * periodically from the main loop or RTOS task.
 * 
 * **Recommended call frequency:** Every 10-50ms
 * 
 * **Processing steps:**
 * 1. Read current GPIO pin state
 * 2. Update state machine based on timing and transitions
 * 3. Invoke appropriate callbacks when events are detected
 * 
 * @note Non-blocking - returns immediately after processing
 * @warning Call frequently enough to detect button presses reliably
 */
void OneButtonTask(OneButton_t *Btn);

/**
 * @}
 */

/**
 * @defgroup onebutton_timers Timer Configuration Functions
 * @brief Functions to configure timing parameters
 * @{
 */

/**
 * @brief Set debounce time
 * @param[in,out] Btn Pointer to OneButton_t structure
 * @param[in] DebounceTime Debounce time in milliseconds
 * @note Default: 10ms
 */
void OneButtonSetTimerDebounce(OneButton_t *Btn, uint32_t DebounceTime);

/**
 * @brief Set double-click detection window
 * @param[in,out] Btn Pointer to OneButton_t structure
 * @param[in] DoubleClickTime Maximum time between clicks for double-click (milliseconds)
 * @note Default: 350ms
 */
void OneButtonSetTimerDoubleClick(OneButton_t *Btn, uint32_t DoubleClickTime);

/**
 * @brief Set long-press start time
 * @param[in,out] Btn Pointer to OneButton_t structure
 * @param[in] LongPressStartTime Time to hold button before long-press starts (milliseconds)
 * @note Default: 700ms
 */
void OneButtonSetTimerLongPressStart(OneButton_t *Btn, uint32_t LongPressStartTime);

/**
 * @brief Set long-press tick interval
 * @param[in,out] Btn Pointer to OneButton_t structure
 * @param[in] LongPressTickTime Repeat interval during long-press (milliseconds)
 * @note Default: 500ms
 */
void OneButtonSetTimerLongPressTick(OneButton_t *Btn, uint32_t LongPressTickTime);

/**
 * @}
 */

/**
 * @defgroup onebutton_register Callback Registration Functions
 * @brief Functions to register event callback handlers
 * @{
 */

/**
 * @brief Register single-click callback
 * @param[in,out] Btn Pointer to OneButton_t structure
 * @param[in] OneClickCallback Function pointer to single-click handler (or NULL to unregister)
 */
void OneButtonCallbackOneClick(OneButton_t *Btn, CallBackFunOneClick_t OneClickCallback);

/**
 * @brief Register double-click callback
 * @param[in,out] Btn Pointer to OneButton_t structure
 * @param[in] DoubleClickCallback Function pointer to double-click handler (or NULL to unregister)
 */
void OneButtonCallbackDoubleClick(OneButton_t *Btn, CallBackFunDoubleClick_t DoubleClickCallback);

/**
 * @brief Register long-press start callback
 * @param[in,out] Btn Pointer to OneButton_t structure
 * @param[in] LongPressStartCallback Function pointer to long-press start handler (or NULL to unregister)
 */
void OneButtonCallbackLongPressStart(OneButton_t *Btn, CallBackFunLongPressStart_t LongPressStartCallback);

/**
 * @brief Register long-press stop callback
 * @param[in,out] Btn Pointer to OneButton_t structure
 * @param[in] LongPressStopCallback Function pointer to long-press stop handler (or NULL to unregister)
 */
void OneButtonCallbackLongPressStop(OneButton_t *Btn, CallBackFunLongPressStop_t LongPressStopCallback);

/**
 * @}
 */

 /**
 * @}
 */

#endif /* INC_ONEBUTTON_H_ */
