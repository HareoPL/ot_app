/**
 * @file ot_app_buffer.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Thread-safe shared buffer management for the OpenThread Application Framework.
 * @version 0.1
 * @date 03-02-2026
 * @addtogroup ot_app
 * @{
 * @defgroup ot_app_buffer Shared Buffer Management
 * @brief Thread-safe global buffer pool for transient data processing.
 * @details
 * This module manages a statically allocated memory pool divided into specific slots (Keys).
 * It provides thread-safe access using FreeRTOS mutexes to prevent race conditions during 
 * read/write operations.
 * * **Key Features:**
 * - **Static Allocation:** No heap fragmentation (uses `HRO_SEC_NOINIT` section).
 * - **Thread Safety:** All API calls are protected by a mutex.
 * - **Zero-Copy Write:** `otapp_buf_getWriteOnly_ptr` allows direct memory access with a locking mechanism.
 * - **Key-Based Access:** Data is organized by predefined keys (e.g., `OTAPP_BUF_KEY_1`).
 *
 * @par Unit Test Verification
 * This module has been verified with a comprehensive suite of unit tests (`HOST_ot_app_buffer_test.c`) using the Unity framework:
 * - **Argument Validation:** Verified rejection of NULL pointers, zero-length requests, and invalid keys (Returns @ref OTAPP_BUF_ERROR).
 * - **Boundary Protection:** Confirmed `OTAPP_BUF_ERROR_OVERFLOW` is returned when attempting to write beyond the allocated slot size.
 * - **Locking Logic:** Verified that `otapp_buf_getWriteOnly_ptr` correctly locks the slot. Subsequent `otapp_buf_append` calls return @ref OTAPP_BUF_ERROR_WRITE_LOCK until the slot is explicitly unlocked via `otapp_buf_writeUnlock`.
 * - **Concurrency & Thread Safety:** Validated using a multi-threaded stress test ("Buffer Bomber"). 
 * The test simulates a Race Condition using `pthread` and confirms that the internal Mutex mechanism prevents data corruption when multiple threads write simultaneously.
 * @{
 */

#ifndef OT_APP_BUFFER_H_
#define OT_APP_BUFFER_H_

#include "stdint.h"

/** @name Return Codes */
///@{
#define OTAPP_BUF_OK                    (-1) ///< Operation successful
#define OTAPP_BUF_ERROR                 (-2) ///< Generic error (null ptr, init missing)
#define OTAPP_BUF_ERROR_OVERFLOW        (-3) ///< Buffer is full or data exceeds slot size
#define OTAPP_BUF_ERROR_KEY_NOT_FOUND   (-4) ///< The provided key does not exist in config
#define OTAPP_BUF_ERROR_WRITE_LOCK      (-5) ///< The slot is currently locked for direct writing
///@}

/** @name Buffer Configuration Keys */
///@{
/** @brief Key 1: General purpose buffer.
 * Used by otapp_pair_uriParseMessage() and otapp_pair_uriResourcesCreate().
 */
#define OTAPP_BUF_KEY_1         0x1001 

#ifdef UNIT_TEST
    #define OTAPP_BUF_KEY_1_SIZE    30000 
#else    
    #define OTAPP_BUF_KEY_1_SIZE    144
#endif

#define OTAPP_BUF_KEY_2         0x1002 ///< Auxiliary buffer slot 2
#define OTAPP_BUF_KEY_2_SIZE    64

#define OTAPP_BUF_KEY_3         0x1003 ///< Auxiliary buffer slot 3
#define OTAPP_BUF_KEY_3_SIZE    64
///@}

/**
 * @brief Buffer configuration structure.
 */
typedef struct {
    uint16_t key;  ///< Unique identifier for the buffer slot
    uint16_t size; ///< Size in bytes allocated for this slot
} bufferConfig_t;

/* Single Definition Guard: 
 * This ensures the configuration array is instantiated in memory exactly once 
 * (inside the source file that defines OTAPP_BUF_INIT_CONGIG_IMPL). 
 */
#ifdef OTAPP_BUF_INIT_CONGIG_IMPL
    const bufferConfig_t otapp_buf_init_config[] = {
        { .key = OTAPP_BUF_KEY_1, .size = OTAPP_BUF_KEY_1_SIZE }, 
        { .key = OTAPP_BUF_KEY_2, .size = OTAPP_BUF_KEY_2_SIZE }, 
        { .key = OTAPP_BUF_KEY_3, .size = OTAPP_BUF_KEY_3_SIZE }  
    };
#endif

/** @brief Total size of the buffer pool */
#define OTAPP_BUF_SIZE      (OTAPP_BUF_KEY_1_SIZE + OTAPP_BUF_KEY_2_SIZE + OTAPP_BUF_KEY_3_SIZE)

/**
 * @brief Appends data to the buffer slot associated with the key.
 * @param[in] key      Buffer slot identifier.
 * @param[in] new_data Pointer to the data to be copied.
 * @param[in] len      Length of data to copy.
 * @return int8_t      @ref OTAPP_BUF_OK on success, or error code (e.g. @ref OTAPP_BUF_ERROR_OVERFLOW).
 * @note **Thread Safe (Mutex Protected):**
 * This function acquires the mutex to protect the `current_len` increment and memory copy 
 * from race conditions. The critical section covers the bounds check and memory update.
 * @warning Returns @ref OTAPP_BUF_ERROR_WRITE_LOCK if the slot is currently locked by `otapp_buf_getWriteOnly_ptr`.
 */
int8_t otapp_buf_append(uint16_t key, const uint8_t* new_data, uint16_t len);

/**
 * @brief Retrieves data from a buffer slot into a user-provided buffer.
 * @param[in]  key       Buffer slot identifier.
 * @param[out] bufOut    Destination buffer.
 * @param[in]  bufSize   Size of the destination buffer.
 * @param[out] lenBufOut Pointer to store the actual number of bytes read.
 * @return int8_t @ref OTAPP_BUF_OK on success.
 */
int8_t otapp_buf_getData(uint16_t key, uint8_t* bufOut, uint16_t bufSize, uint16_t *lenBufOut);


/**
 * @brief Gets a read-only pointer to the buffer slot's payload.
 * @param[in]  key          Buffer slot identifier.
 * @param[out] bufSize_out  Pointer to store the size of valid data in the slot.
 * @return const uint8_t* Pointer to the data, or NULL on error.
 * @note This function acquires the mutex briefly to read the current length, 
 * but returns a direct pointer to static memory.
 */
const uint8_t *otapp_buf_getReadOnly_ptr(uint16_t key, uint16_t *bufSize_out);

/**
 * @brief Requests a write-only pointer for direct memory access (Zero-Copy).
 * @details This function locks the slot for writing. after correctly returning the ptr to the memory slot, subsequent calls to `otapp_buf_append` 
 * or `otapp_buf_getWriteOnly_ptr` for this key will fail/block until @ref otapp_buf_writeUnlock is called.
 * @param[in] key           Buffer slot identifier.
 * @param[in] required_size Number of bytes intended to be written (sets `current_len`).
 * @return uint8_t* Pointer to the buffer slot, or NULL if locked/full.
 * @warning **Blocking Behavior:** If successful, this function explicitly **LOCKS** the slot. 
 * You **MUST** call @ref otapp_buf_writeUnlock(key) after finishing the write operation.
 */
uint8_t* otapp_buf_getWriteOnly_ptr(uint16_t key, uint16_t required_size);

/**
 * @brief Clears the data in a specific buffer slot.
 * @param key Buffer slot identifier.
 * @return int8_t @ref OTAPP_BUF_OK on success.
 */
int8_t otapp_buff_clear(uint16_t key);

/**
 * @brief Returns the maximum capacity of a slot.
 * @param key Buffer slot identifier.
 * @return uint16_t Max size in bytes.
 */
uint16_t otapp_buf_getMaxSize(uint16_t key);

/**
 * @brief Returns the current length of data stored in a slot.
 * @param key Buffer slot identifier.
 * @return uint16_t Current usage in bytes.
 */
uint16_t otapp_buf_getCurrentLenSize(uint16_t key);

/**
 * @brief Unlocks a slot previously locked by `otapp_buf_getWriteOnly_ptr`.
 * @param key Buffer slot identifier.
 * @return int8_t @ref OTAPP_BUF_OK on success.
 */
int8_t otapp_buf_writeUnlock(uint16_t key);

/**
 * @brief Initializes the buffer module and creates the RTOS mutex.
 * @note This is called internally during the framework initialization.
 */
void otapp_buffer_init(void);

#endif /* OT_APP_BUFFER_H_ */

/**
 * @}
 * @}
 */