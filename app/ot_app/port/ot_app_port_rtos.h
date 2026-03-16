/**
 * @file ot_app_port_rtos.h
 * @brief Unified inclusion of FreeRTOS headers for different platforms.
 * @details see more information in section: @ref ot_app_port_rtos 
 * 
 * @defgroup ot_app_port_rtos RTOS Port
 * @ingroup ot_app
 * @brief Unified inclusion of FreeRTOS headers for different platforms.
 * @details
 * @{
 * This module acts as a bridge to include the correct FreeRTOS header files depending on the target SDK.
 * Different platforms structure their include directories differently:
 * - **ESP32 (ESP-IDF):** Uses the `freertos/` prefix (e.g., `freertos/FreeRTOS.h`).
 * - **STM32 (Standard):** Typically uses direct inclusion (e.g., `FreeRTOS.h`).
 * * By including this file, the core application can use standard FreeRTOS types (like `TaskHandle_t`, `QueueHandle_t`)
 * without cluttering the code with `#ifdef` directives everywhere.
 * 
 * @version 0.1
 * @date 24-10-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */

#ifndef OT_APP_PORT_RTOS_H_
#define OT_APP_PORT_RTOS_H_


#ifdef ESP_PLATFORM
/* Espressif ESP-IDF specific includes */
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "freertos/queue.h" 
    #include "freertos/semphr.h"

#elif defined(STM_PLATFORM)
/* STM32 / Standard FreeRTOS specific includes */
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h" 
	#include "semphr.h"

#else
    // #error "Unsupported platform. Define ESP_PLATFORM or STM_PLATFORM"
#endif

#endif  /* OT_APP_PORT_RTOS_H_ */

/**
 * @}
 */