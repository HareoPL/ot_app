/**
 * @file spiffs.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief SPIFFS filesystem wrapper for ESP-IDF with simplified API
 * @version 0.1
 * @date 17-04-2025
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
 * @defgroup spiffs SPIFFS Filesystem Wrapper
 * @ingroup libs
 * @{
 * 
 * @section spiffs_overview Overview
 * 
 * SPIFFS (SPI Flash File System) wrapper library for ESP32 microcontrollers.
 * Provides simplified interface to ESP-IDF's SPIFFS implementation with automatic
 * initialization, mounting, and error handling.
 * 
 * **Key Features:**
 * - ✅ Simplified mount/unmount API
 * - ✅ Automatic filesystem check on startup
 * - ✅ Partition size information retrieval
 * - ✅ Auto-format on mount failure
 * - ✅ POSIX-compatible file operations (fopen, fwrite, fread, etc.)
 * - ✅ Multiple concurrent file access
 * - ✅ Wear-leveling friendly
 * - ✅ FreeRTOS task integration
 * 
 * @section spiffs_usage Basic Usage
 * 
 * **Simple File Write Example:**
 * @code{.c}
 * #include "spiffs.h"
 * #include <stdio.h>
 * 
 * void app_main() {
 *     // Mount SPIFFS filesystem
 *     esp_err_t ret = spiffs_mount();
 *     if (ret != ESP_OK) {
 *         printf("Failed to mount SPIFFS\n");
 *         return;
 *     }
 *     
 *     // Write to file using standard C functions
 *     FILE* f = fopen("/spiffs/data.txt", "w");
 *     if (f != NULL) {
 *         fprintf(f, "Hello SPIFFS!\n");
 *         fclose(f);
 *     }
 *     
 *     // Unmount when done
 *     spiffs_unmount();
 * }
 * @endcode
 * 
 * **Read File Example:**
 * @code{.c}
 * void read_file_example() {
 *     spiffs_mount();
 *     
 *     FILE* f = fopen("/spiffs/data.txt", "r");
 *     if (f != NULL) {
 *         char line[128];
 *         while (fgets(line, sizeof(line), f)) {
 *             printf("%s", line);
 *         }
 *         fclose(f);
 *     }
 *     
 *     spiffs_unmount();
 * }
 * @endcode
 * 
 * **Get Partition Info Example:**
 * @code{.c}
 * void partition_info_example() {
 *     spiffs_mount();
 *     
 *     size_t total = 0, used = 0;
 *     if (spiffs_partitionSizeInfo(&total, &used) == ESP_OK) {
 *         printf("SPIFFS: %d KB total, %d KB used\n", 
 *                total / 1024, used / 1024);
 *         printf("Free space: %d KB\n", (total - used) / 1024);
 *     }
 *     
 *     spiffs_unmount();
 * }
 * @endcode
 * 
 * @section spiffs_config Configuration
 * 
 * SPIFFS configuration is defined in `spiffs.c`:
 * 
 * @code{.c}
 * static esp_vfs_spiffs_conf_t spiffsConf = {
 *     .base_path = "/spiffs",              // Mount point
 *     .partition_label = NULL,             // Use default partition
 *     .max_files = 5,                      // Max concurrent open files
 *     .format_if_mount_failed = true      // Auto-format on mount failure
 * };
 * @endcode
 * 
 * **Configuration Options:**
 * - `base_path`: VFS mount point (default: "/spiffs")
 * - `partition_label`: NULL for default, or specific partition name
 * - `max_files`: Maximum number of files that can be open simultaneously
 * - `format_if_mount_failed`: Auto-format filesystem if mount fails
 * 
 * @section spiffs_check Filesystem Check
 * 
 * **Automatic Check on Startup:**
 * 
 * Define `SPIFFS_CHECK_ON_START` to enable automatic filesystem check during mount:
 * @code{.c}
 * #define SPIFFS_CHECK_ON_START
 * @endcode
 * 
 * This performs integrity check and repairs filesystem if needed.
 * Useful after power loss or unexpected reset.
 * 
 * @section spiffs_operations File Operations
 * 
 * All standard POSIX file operations are supported:
 * 
 * | Operation | Function | Description |
 * |-----------|----------|-------------|
 * | **Open** | `fopen()` | Open file for reading/writing |
 * | **Close** | `fclose()` | Close file handle |
 * | **Read** | `fread()`, `fgets()` | Read data from file |
 * | **Write** | `fwrite()`, `fprintf()` | Write data to file |
 * | **Seek** | `fseek()`, `ftell()` | Navigate within file |
 * | **Delete** | `unlink()` | Delete file |
 * | **Rename** | `rename()` | Rename/move file |
 * | **Stat** | `stat()` | Get file information |
 * 
 * @section spiffs_test Test Function
 * 
 * The library includes a test function demonstrating basic operations:
 * 
 * @code{.c}
 * spiffs_test();  // Creates FreeRTOS task that tests SPIFFS
 * @endcode
 * 
 * **Test sequence:**
 * 1. Mount filesystem
 * 2. Get partition info
 * 3. Write test file
 * 4. Rename file
 * 5. Read file
 * 6. Unmount filesystem
 * 
 * @section spiffs_partition Partition Configuration
 * 
 * SPIFFS partition must be defined in partition table CSV file:
 * 
 * @code{.csv}
 *   # Name,    Type, SubType, Offset,   Size,      Flags
 *   nvs,       data, nvs,     0x9000,   0x6000,    
 *   otadata,   data, ota,     0xf000,   0x2000,    
 *   phy_init,  data, phy,     0x11000,  0x1000,    
 *   factory,   app,  factory, 0x20000,  0x1E0000,  
 *   ota_0,     app,  ota_0,   0x200000, 0x200000,  
 *   ota_1,     app,  ota_1,   0x400000, 0x200000,  
 *   storage,    data, spiffs, 0x600000, 0x200000,  
 * @endcode
 * 
 * @note SPIFFS partition size should be multiple of 4KB
 * @note Minimum recommended size: 256KB
 * 
 * @warning Do not unmount while files are open
 * @warning SPIFFS is not suitable for wear-intensive applications (use FatFS instead)
 * 
 * @see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html
 */

#ifndef SPIFFS_H_
#define SPIFFS_H_

#include "main.h"

/**
 * @defgroup spiffs_config Configuration Macros
 * @brief Compile-time configuration options
 * @{
 */

/**
 * @brief Enable filesystem check on startup
 * 
 * @details
 * When defined, performs automatic SPIFFS integrity check during mount.
 * 
 * **Check Operations:**
 * - Validates filesystem structure
 * - Repairs corrupted metadata
 * - Cleans unreferenced pages
 * - Fixes inconsistent state after power loss
 * 
 * **Performance Impact:**
 * - Adds ~200-500ms to mount time (depends on partition size)
 * - Only runs on first mount after power-on
 * 
 * **Recommendations:**
 * - Enable for battery-powered devices
 * - Enable for devices with frequent power loss
 * - Disable for performance-critical applications
 * 
 * @note See https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
 */
#define SPIFFS_CHECK_ON_START

/**
 * @}
 */

/**
 * @defgroup spiffs_functions Public API Functions
 * @brief SPIFFS filesystem control functions
 * @{
 */

/**
 * @brief Mount SPIFFS filesystem
 * 
 * @return esp_err_t 
 *         - ESP_OK: Success
 *         - ESP_FAIL: Failed to mount or format
 *         - ESP_ERR_NOT_FOUND: SPIFFS partition not found
 *         - ESP_ERR_INVALID_STATE: Already mounted
 * 
 * @details
 * Initializes and mounts SPIFFS filesystem to "/spiffs" mount point.
 * 
 * **Mount Process:**
 * 1. Check if already mounted (prevents double mount)
 * 2. Register SPIFFS with ESP-IDF VFS
 * 3. Attempt to mount partition
 * 4. If mount fails and auto-format enabled, format partition
 * 5. If SPIFFS_CHECK_ON_START defined, run filesystem check
 * 6. Mark as mounted
 * 
 * **Configuration Used:**
 * - Base path: "/spiffs"
 * - Partition label: NULL (default partition)
 * - Max files: 5
 * - Auto-format on failure: true
 * 
 * **Auto-Format Behavior:**
 * - Triggered when filesystem is corrupted
 * - Triggered when partition is unformatted
 * - Erases all existing data
 * - Creates new filesystem
 * 
 * **Example:**
 * @code{.c}
 * esp_err_t ret = spiffs_mount();
 * if (ret == ESP_OK) {
 *     printf("SPIFFS mounted successfully\n");
 *     // Use filesystem...
 * } else if (ret == ESP_ERR_NOT_FOUND) {
 *     printf("SPIFFS partition not found in partition table\n");
 * } else {
 *     printf("Failed to mount SPIFFS: %s\n", esp_err_to_name(ret));
 * }
 * @endcode
 * 
 * @note Can be called multiple times safely (returns ESP_OK if already mounted)
 * @note Blocks until mount completes (typically 50-500ms)
 * @warning Ensure partition table includes SPIFFS partition
 * 
 * @see spiffs_unmount() to unmount filesystem
 * @see spiffs_partitionSizeInfo() to get partition information
 */
esp_err_t spiffs_mount(void);

/**
 * @brief Unmount SPIFFS filesystem
 * 
 * @return esp_err_t 
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_STATE: Not mounted
 *         - ESP_FAIL: Unmount failed
 * 
 * @details
 * Unmounts SPIFFS filesystem and unregisters from VFS.
 * 
 * **Unmount Process:**
 * 1. Flush all pending writes
 * 2. Close any open file handles (may cause data loss if files are open)
 * 3. Unregister from ESP-IDF VFS
 * 4. Free allocated resources
 * 5. Mark as unmounted
 * 
 * **Important Notes:**
 * - All open files are forcibly closed
 * - Uncommitted data may be lost
 * - File handles become invalid
 * 
 * **Example:**
 * @code{.c}
 * // Proper cleanup sequence
 * FILE* f = fopen("/spiffs/data.txt", "w");
 * if (f != NULL) {
 *     fprintf(f, "data");
 *     fclose(f);  // Close file BEFORE unmount
 * }
 * 
 * esp_err_t ret = spiffs_unmount();
 * if (ret == ESP_OK) {
 *     printf("SPIFFS unmounted successfully\n");
 * }
 * @endcode
 * 
 * @warning Close all files before calling unmount
 * @warning File paths like "/spiffs/..." become invalid after unmount
 * @note Can be called multiple times safely
 * 
 * @see spiffs_mount() to mount filesystem
 */
esp_err_t spiffs_unmount(void);

/**
 * @brief Get SPIFFS partition size information
 * 
 * @param[out] totalSize Pointer to store total partition size in bytes
 * @param[out] usedSize Pointer to store used space in bytes
 * 
 * @return esp_err_t 
 *         - ESP_OK: Success
 *         - ESP_FAIL: Failed to get info (filesystem may be corrupted)
 *         - ESP_ERR_INVALID_ARG: NULL pointer passed
 * 
 * @details
 * Retrieves partition size information from mounted SPIFFS filesystem.
 * 
 * **Information Retrieved:**
 * - Total partition size (bytes)
 * - Used space (bytes)
 * - Free space = total - used
 * 
 * **Consistency Check:**
 * - Verifies that used <= total
 * - If inconsistent, runs filesystem check automatically
 * - Attempts repair if corruption detected
 * 
 * **On Error:**
 * - If getting info fails, attempts to format partition
 * - Returns error if format also fails
 * 
 * **Example:**
 * @code{.c}
 * size_t total = 0, used = 0;
 * 
 * if (spiffs_partitionSizeInfo(&total, &used) == ESP_OK) {
 *     size_t free = total - used;
 *     
 *     printf("SPIFFS Partition:\n");
 *     printf("  Total: %u bytes (%u KB)\n", total, total / 1024);
 *     printf("  Used:  %u bytes (%u KB)\n", used, used / 1024);
 *     printf("  Free:  %u bytes (%u KB)\n", free, free / 1024);
 *     printf("  Usage: %.1f%%\n", (float)used / total * 100);
 *     
 *     if (free < 10240) {  // Less than 10KB free
 *         printf("Warning: Low disk space!\n");
 *     }
 * } else {
 *     printf("Failed to get partition info\n");
 * }
 * @endcode
 * 
 * **Typical Size Reporting:**
 * - 1MB partition: ~960KB usable (overhead ~40KB)
 * - 512KB partition: ~490KB usable
 * - 256KB partition: ~245KB usable
 * 
 * @note SPIFFS has ~4% overhead for metadata
 * @note Filesystem must be mounted before calling
 * @warning May trigger filesystem check if inconsistency detected
 * 
 * @see spiffs_mount() to mount filesystem first
 */
esp_err_t spiffs_partitionSizeInfo(size_t *totalSize, size_t *usedSize);

/**
 * @brief Run SPIFFS test sequence in FreeRTOS task
 * 
 * @details
 * Creates a FreeRTOS task that performs comprehensive SPIFFS testing.
 * Useful for verifying filesystem functionality and debugging.
 * 
 * **Test Task Sequence:**
 * 1. Mount SPIFFS filesystem
 * 2. Get and log partition size information
 * 3. Run file operations test (write, rename, read)
 * 4. Unmount filesystem
 * 5. Delete task (one-time execution)
 * 
 * **Test Operations Performed:**
 * - Create file "/spiffs/hello.txt"
 * - Write text to file
 * - Rename to "/spiffs/foo.txt"
 * - Read file contents
 * - Verify data integrity
 * 
 * **Task Configuration:**
 * - Name: "spiffs"
 * - Stack: 8KB (1024 * 8 bytes)
 * - Priority: 5
 * - Execution: One-time (task deletes itself after completion)
 * 
 * **Console Output Example:**
 * @code{.text}
 * [spiffs_c] Initializing SPIFFS
 * [spiffs_c] Partition size: total: 983040, used: 4096
 * [spiffs_c] Opening file
 * [spiffs_c] File written
 * [spiffs_c] Renaming file
 * [spiffs_c] Reading file
 * [spiffs_c] Read from file: 'Hello World!'
 * [spiffs_c] SPIFFS unmounted
 * @endcode
 * 
 * **Usage Example:**
 * @code{.c}
 * void app_main() {
 *     // Start SPIFFS test
 *     spiffs_test();
 *     
 *     // Test runs asynchronously in separate task
 *     // Main loop continues
 *     while(1) {
 *         vTaskDelay(pdMS_TO_TICKS(1000));
 *     }
 * }
 * @endcode
 * 
 * @note Task deletes itself after completion
 * @note Suitable for development and debugging only
 * @note Creates test files that are not cleaned up
 * @warning Not suitable for production use (leaves test files)
 * 
 * @see spiffs_mount() for manual mounting
 * @see spiffs_partitionSizeInfo() for partition info
 */
void spiffs_test(void);

/**
 * @}
 */

#endif /* SPIFFS_H_ */

/**
 * @}
 */
