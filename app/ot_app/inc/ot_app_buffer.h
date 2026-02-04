/**
 * @file ot_app_buffer.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Thread-safe shared buffer management for the OpenThread Application Framework.
 * @version 0.1
 * @date 03-02-2026
 * @addtogroup ot_app
 * @{
 * @defgroup ot_app_buffer Shared Buffer Management
 * @brief Thread-safe global buffer pool for transient data processing (e.g., CoAP payloads, TLV serialization).
 * * @details
 * This module provides a single, statically allocated buffer of @ref OT_APP_BUFFER_SIZE bytes.
 * It is designed to be shared across different application modules to save RAM in constrained 
 * environments. Access is synchronized using an RTOS Mutex to prevent data races.
 * * **Usage Pattern:**
 * 1. Acquire the buffer and mutex: `otapp_buffer_get_withMutex()`
 * 2. Process data (clear, write TLV, etc.)
 * 3. Release the mutex: `otapp_buffer_releaseMutex()`
 * @{
 */

#ifndef OT_APP_BUFFER_H_
#define OT_APP_BUFFER_H_

#include "stdint.h"

/**
 * @brief Total size of the shared application buffer in bytes.
 */
#define OT_APP_BUFFER_SIZE 1024 

/**
 * @brief Acquires the shared buffer and locks its associated mutex.
 * * @details This function blocks the calling thread until the buffer becomes available.
 * Every successful call MUST be followed by a call to @ref otapp_buffer_releaseMutex.
 * * @return uint8_t* Pointer to the start of the 1024-byte buffer.
 */
uint8_t *otapp_buffer_get_withMutex(void);

/**
 * @brief Releases the mutex for the shared buffer.
 * * @details Call this function immediately after finishing work with the buffer to 
 * allow other tasks to access it.
 */
void otapp_buffer_releaseMutex(void);

/**
 * @brief Returns the total capacity of the shared buffer.
 * * @return uint16_t Buffer size in bytes (@ref OT_APP_BUFFER_SIZE).
 */
uint16_t otapp_buffer_getSize(void);

/**
 * @brief Wipes the entire buffer content with zeros.
 * * @note This function should only be called after acquiring the mutex.
 */
void otapp_buffer_clear(void);

/**
 * @brief Initializes the buffer module and creates the RTOS mutex.
 * * @note This is called internally during the framework initialization.
 */
void otapp_buffer_init(void);

#endif /* OT_APP_BUFFER_H_ */

/**
 * @}
 * @}
 */
