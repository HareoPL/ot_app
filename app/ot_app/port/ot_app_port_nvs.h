/**
 * @file ot_app_port_nvs.h
 * @brief Abstraction layer for persistent data storage (Flash/EEPROM).
 * @details see more information in section: @ref ot_app_port_nvs
 * 
 * @defgroup ot_app_port_nvs NVS Storage Port
 * @ingroup ot_app
 * @brief Abstraction layer for persistent data storage (Flash/EEPROM).
 * @details
 * @{
 * This module provides a unified interface for saving and retrieving configuration data 
 * that must survive a power cycle (e.g., paired devices list, network credentials).
 * **Platform Independence:**
 * - On **ESP32**, it maps to the NVS (Non-Volatile Storage) partition API.
 * - On **STM32**, it typically maps to Flash Page emulation or external EEPROM.
 * **Key Concept:** Data is accessed via a simplified numeric `keyId` (uint8_t), which the implementation 
 * translates into platform-specific keys or memory addresses.
 * 
 * @version 0.1
 * @date 24-10-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */

#ifndef OT_APP_NVS_H_
#define OT_APP_NVS_H_

#include "stdint.h"

#define OT_APP_NVS_OK               (-1)
#define OT_APP_NVS_ERROR            (-2)
#define OT_APP_NVS_IS               (-3)
#define OT_APP_NVS_IS_NOT           (-4)
#define OT_APP_NVS_IS_NO_SPACE      (-5)

/**
 * @brief Saves a string to non-volatile storage.
 * @details Writes a null-terminated string associated with a specific numeric key ID.
 * If data already exists for this key, it is overwritten.
 * @param inData        [in] Pointer to the null-terminated string to save.
 * @param keyId         [in] Unique identifier (0-255) for the data entry.
 * @return int8_t       @ref OT_APP_NVS_OK on success, or @ref OT_APP_NVS_ERROR / @ref OT_APP_NVS_IS_NO_SPACE.
 */
int8_t ot_app_nvs_saveString(const char *inData, const uint8_t keyId);

/**
 * @brief Reads a string from non-volatile storage.
 * @details Retrieves data associated with the numeric key ID into the provided buffer.
 * Ensures the output string is null-terminated.
 * @param[out] outBuff      Destination buffer.
 * @param[in]  outBuffSize  Size of the destination buffer.
 * @param[in]  keyId        Unique identifier of the data to read.
 * @return int8_t           @ref OT_APP_NVS_OK on success, or @ref OT_APP_NVS_IS_NOT if key not found.
 */
int8_t ot_app_nvs_readString(char *outBuff, uint8_t outBuffSize, const uint8_t keyId);

/**
 * @brief Initializes the underlying NVS platform driver.
 * @details Performs necessary hardware or middleware setup (e.g., `nvs_flash_init()` on ESP32).
 * Should be called once at system startup before any read/write operations.
 * @return int8_t @ref OT_APP_NVS_OK on success.
 */
int8_t ot_app_nvs_init(void);

/**
 * @brief Runs a simple read/write verification test.
 * @details Writes a set of test strings to keys 0-2, reads them back, verifies content, 
 * and then deletes/clears them. Results are printed to the log.
 * Used for debugging storage integration.
 */
void ot_app_nvs_test();


#endif  /* OT_APP_NVS_H_ */

/**
 * @}
 */