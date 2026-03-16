/**
 * @file ot_app_port_openthread.h
 * @brief Abstraction layer unifying access to the OpenThread instance across different platforms.
 * @details see more information in section: @ref ot_app_port_ot
 * 
 * @defgroup ot_app_port_ot OpenThread Port
 * @ingroup ot_app
 * @brief Abstraction layer unifying access to the OpenThread instance across different platforms.
 * @details
 * @{
 * This module provides a uniform macro to retrieve the `otInstance` pointer, 
 * shielding the core application from platform-specific API differences.
 * **Supported Platforms:**
 * - **ESP_PLATFORM (ESP32):** Maps to `esp_openthread_get_instance()`.
 * - **STM_PLATFORM (STM32):** Maps to `APP_THREAD_GetInstance()`.
 * **Usage:**
 * The core application calls `otapp_port_openthread_get_instance()` without needing to know 
 * which hardware is currently running.
 * 
 * @version 0.1
 * @date 24-10-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */
#ifndef OT_APP_PORT_OPENTHREAD_H_
#define OT_APP_PORT_OPENTHREAD_H_

#ifdef ESP_PLATFORM
    #include "esp_openthread.h"

    /**
     * @brief Retrieves the OpenThread instance pointer (ESP32 implementation).
     * @return otInstance* Pointer to the OpenThread context.
     */
    #define otapp_port_openthread_get_instance() esp_openthread_get_instance()

#elif defined(STM_PLATFORM)
    #include "app_thread.h"

    /**
     * @brief Retrieves the OpenThread instance pointer (STM32 implementation).
     * @return otInstance* Pointer to the OpenThread context.
     */
    #define otapp_port_openthread_get_instance() APP_THREAD_GetInstance()

#else
    #error "Unsupported platform. Define ESP_PLATFORM or STM_PLATFORM"
#endif

#endif  /* OT_APP_PORT_OPENTHREAD_H_ */

/**
 * @}
 */