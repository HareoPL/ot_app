/**
 * @file ot_app_msg_tlv.h
 *
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief TLV (Type-Length-Value) message serialization/deserialization API.
 *
 * @version 0.1
 * @date 29-01-2026
 *
 * @defgroup ot_app_msg_tlv TLV Message Buffer
 * @ingroup ot_app
 * @{
 *
 * @section msg_tlv_description Overview
 *
 * The TLV Message Buffer module provides serialization and deserialization of
 * Type-Length-Value (TLV) data structures in a **fixed-size user-provided** byte buffer.
 * Designed for constrained environments like OpenThread/CoAP payloads where dynamic
 * allocation is avoided.
 *
 * **Key features:**
 * - Append unique TLV blocks (`keyAdd()`): key (u16) + length (u16) + value
 * - Extract value by key (`keyGet()`): linear search with optional value copy
 * - Query free space (`freeBufSpaceGet()`): remaining capacity calculation
 * - **2-byte reserved header** tracks total used bytes (writtenBytes counter)
 * - **Packed 4-byte TLV header** (no padding): `uint16_t key; uint16_t length;`
 *
 * **Buffer layout (exact byte-by-byte format):**
 * ```
 * [0:1] writtenBytes (u16) | [2:5] TLV1: [key u16][len u16][value[len]] | [N:] TLV2... | [free space]
 * ```
 * **Minimum size for 1 key: 2 + 4 + valueLength bytes**
 *
 * **Example for valueLength=4 (TOTAL: 10 bytes minimum):**
 * 
 * | Bytes  | Content          | Size | Example       | Description              |
 * |--------|------------------|------|---------------|--------------------------|
 * | `[0:1]`| writtenBytes     | 2B   | `0x000A` (10) | **TLV data counter**     |
 * | `[2:3]`| **key**          | 2B   | `0xAAA1`      | TLV identifier           |
 * | `[4:5]`| **length**       | 2B   | `0x0004` (4)  | Value bytes count        |
 * | `[6:9]`| **value**        | 4B   | `[1,2,3,4]`   | Actual data              |
 * | **↓**  | **TOTAL**        | **10B**|             | **Minimum buffer size**  |
 *
 * **Error handling:**
 * - Strict validation: null pointers, buffer overflow, invalid sizes
 * - Duplicate key detection during append
 * - Empty/invalid buffer states
 *
 * **Typical lifecycle:**
 * - Allocate fixed buffer (e.g. noinit/heap/stack)
 * - Append keys: `otapp_msg_tlv_keyAdd(buffer, size, key, len, value)`
 * - Query: `otapp_msg_tlv_freeBufSpaceGet(buffer, size, &free)`
 * - Extract: `otapp_msg_tlv_keyGet(buffer, size, key, &len, value)`
 *
 * **Integration:**
 * - CoAP options, OpenThread discovery payloads, device state serialization
 * - Portable: ESP32/STM32 (little-endian, packed structs)
 * - Thread-safe with external mutex (user-provided buffer)
 *
/**
 * @section msg_tlv_example Usage Example
 *
 * This example demonstrates how to gather device URI resources, serialize them into 
 * TLV format using a thread-safe system buffer, and transmit them as a CoAP response.
 *
 * @code{.c}
 * #include "ot_app_msg_tlv.h"
 * #include "ot_app_buffer.h"
 * #include "ot_app_drv.h"
 *
 * // Example CoAP handler implementation that sends a TLV-encoded URI list as a response.
 * void ad_temp_uri_well_knownCoreHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo)
 * {
 *      ot_app_devDrv_t *devDrv_ = otapp_getDevDrvInstance();
 *      otapp_coap_uri_t *urisList = NULL;
 *      uint8_t *buffer;
 *      uint16_t bufferSize;
 *      int8_t result;
 *
 *      if (request && devDrv_)
 *      {
 *          // 1. Retrieve the URI list defined in the device driver
 *          urisList = devDrv_->uriGetList_clb();
 *          if(urisList == NULL) return;
 *
 *          // 2. Acquire access to the thread-safe global buffer
 *          buffer = otapp_buffer_get_withMutex();
 *          otapp_buffer_clear();
 *          bufferSize = otapp_buffer_getSize();
 *
 *          // 3. Serialize URI data into TLV format
 *          result = otapp_pair_uriResourcesCreate(urisList, devDrv_->uriGetListSize, buffer, &bufferSize);
 *          if(result == OTAPP_PAIR_OK) 
 *          {
 *              // 4. Send the populated TLV buffer as a CoAP response
 *              otapp_coap_sendResponse(request, aMessageInfo, buffer, bufferSize);
 *          }
 *
 *          // 5. Release the buffer mutex
 *          otapp_buffer_releaseMutex();
 *      }
 * }
 *
 * // Helper function to create a TLV structure from a URI list.
 * int8_t otapp_pair_uriResourcesCreate(otapp_coap_uri_t *uri, uint8_t uriSize, uint8_t *bufferOut, uint16_t *bufferSizeInOut)
 * {   
 *      if(uri == NULL || bufferOut == NULL || bufferSizeInOut == NULL || uriSize == 0) return OTAPP_PAIR_ERROR;
 *
 *      const uint16_t patternKey = 0xAA00;
 *
 *      // Add TLV block containing the number of available URIs
 *      otapp_msg_tlv_keyAdd(bufferOut, *bufferSizeInOut, patternKey, sizeof(uriSize), &uriSize);
 *
 *      // Iterate through the list and append device types and URI paths as TLV blocks
 *      for (size_t i = 0; i < uriSize; i++) 
 *      {
 *          // Add device type using an incremented key
 *          otapp_msg_tlv_keyAdd(bufferOut, *bufferSizeInOut, patternKey + i + 1, sizeof(uri[i].devType), (uint8_t*)&uri[i].devType);
 *          
 *          // Add URI path string as the subsequent TLV block
 *          otapp_msg_tlv_keyAdd(bufferOut, *bufferSizeInOut, patternKey + i + 2, strlen(uri[i].resource.mUriPath), (uint8_t*)uri[i].resource.mUriPath);
 *      }
 *
 *      // Retrieve the final count of written bytes from the buffer header
 *      if(otapp_msg_tlv_writenBytesGet(bufferOut, *bufferSizeInOut, bufferSizeInOut) == OT_APP_MSG_TLV_ERROR) 
 *      {
 *          return OTAPP_PAIR_ERROR;
 *      }
 *      return OTAPP_PAIR_OK;
 * }
 * @endcode

 * @include ot_app_msg_tlv_example.c
 */

#ifndef OT_APP_MSG_TLV_H_
#define OT_APP_MSG_TLV_H_

#define OT_APP_MSG_TLV_OK                   (-1) ///< Operation successful
#define OT_APP_MSG_TLV_ERROR                (-2) ///< Generic error (null ptr, invalid param)
#define OT_APP_MSG_TLV_ERROR_NO_SPACE       (-3) ///< no space for new TLV
#define OT_APP_MSG_TLV_KEY_EXIST            (-4) ///< Key already exists (duplicate add)
#define OT_APP_MSG_TLV_KEY_NO_EXIST         (-5) ///< Key not found
#define OT_APP_MSG_TLV_EMPTY_BUFFER         (-5) ///< Buffer has no TLV data

#include "stdint.h"

/**
 * @brief Add new TLV block to buffer if key unique and space available.
 *
 * Appends a TLV entry: key (u16) + length (u16) + value bytes. Updates internal
 * writtenBytes counter. Validates space, duplicates, and parameters.
 *
 * @param buffer Pointer to buffer (user-allocated).
 * @param bufferSize Total buffer size in bytes. (must >= reserved 2B + TLV).
 * @param key Unique 16-bit key identifier.
 * @param valueLengthIn Value length in bytes (0 invalid).
 * @param valueIn Pointer to value data to copy.
 *
 * @return OT_APP_MSG_TLV_OK on success, error code otherwise.
 *
 * @note Buffer must have >= 2B + (x * TLV struct + keys valueLength)) bytes free.
 * @note Existing keys rejected (use update if needed).
 */
int8_t otapp_msg_tlv_keyAdd(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, const uint16_t valueLengthIn, uint8_t *valueIn);

/**
 * @brief Extract TLV value by key (optional copy to output).
 *
 * Linear scan from buffer start. Validates buffer integrity and key presence.
 * Copies value if pointers provided.
 *
 * @param buffer Pointer to TLV buffer.
 * @param bufferSize Total buffer size.
 * @param key 16-bit key to find.
 * @param valueLengthOut OUT: Value length (NULL to skip).
 * @param valueOut OUT: Value copy destination (NULL to skip).
 *
 * @return OT_APP_MSG_TLV_KEY_EXIST on found (with copy if requested),
 *         OT_APP_MSG_TLV_KEY_NO_EXIST if missing, error otherwise.
 */
int8_t otapp_msg_tlv_keyGet(uint8_t *buffer, const uint16_t bufferSize, const uint16_t key, uint16_t *valueLengthOut, uint8_t *valueOut);

/**
 * @brief Get remaining free space in TLV buffer.
 *
 * Calculates: bufferSize - reserved(2B) - writtenBytes.
 *
 * @param buffer Pointer to TLV buffer.
 * @param bufferSize Total size.
 * @param freeBufSpaceOut OUT: Free bytes available.
 *
 * @return OT_APP_MSG_TLV_OK or error.
 */
int8_t otapp_msg_tlv_freeBufSpaceGet(uint8_t *buffer, const uint16_t bufferSize, uint16_t *freeBufSpaceOut);

/**
 * @brief Get current written bytes count from buffer header.
 *
 * Reads 2-byte reserved header (little-endian u16).
 *
 * @param buffer Pointer to TLV buffer.
 * @param bufferSize Total size (>= 2B reserved).
 * @param writtenBytesOut OUT: Bytes used by TLV data.
 *
 * @return OT_APP_MSG_TLV_OK or error.
 */
int8_t otapp_msg_tlv_writenBytesGet(uint8_t *buffer, const uint16_t bufferSize, uint16_t *writtenBytesOut);

#endif  /* OT_APP_MSG_TLV_H_ */
