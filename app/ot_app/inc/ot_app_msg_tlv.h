/**
 * @file ot_app_msg_tlv.h
 * @brief TLV (Type-Length-Value) message serialization/deserialization API.
 * @details see more information in section: @ref ot_app_msg_tlv
 * 
 * @defgroup ot_app_msg_tlv TLV Message Buffer
 * @ingroup ot_app
 * @brief TLV (Type-Length-Value) message serialization/deserialization API.
 * @details
 * @{
 *
 * @section msg_tlv_description Overview
 *
 * The TLV Message Buffer module provides serialization and deserialization of
 * Type-Length-Value (TLV) data structures in a **fixed-size user-provided** byte buffer.
 * Designed for constrained environments like OpenThread/CoAP payloads where dynamic
 * allocation is avoided.
 *
 * *Key features:**
 * - Append unique TLV blocks (`keyAdd()`): key (u16) + length (u16) + value
 * - Extract value by key (`keyGet()`): linear search with optional value copy
 * - Query free space (`freeBufSpaceGet()`): remaining capacity calculation
 * - **2-byte reserved header** tracks total used bytes (writtenBytes counter)
 * - **Packed 4-byte TLV header** (no padding): `uint16_t key; uint16_t length;`
 *
 * *Buffer layout (exact byte-by-byte format):**
 * ```
 * [0:1] writtenBytes (u16) | [2:5] TLV1: [key u16][len u16][value[len]] | [N:] TLV2... | [free space]
 * ```
 * *Minimum size for 1 key: 2 + 4 + valueLength bytes**
 *
 * *Example for valueLength=4 (TOTAL: 10 bytes minimum):**
 * 
 * | Bytes  | Content          | Size | Example       | Description              |
 * |--------|------------------|------|---------------|--------------------------|
 * | `[0:1]`| writtenBytes     | 2B   | `0x000A` (10) | **TLV data counter**     |
 * | `[2:3]`| **key**          | 2B   | `0xAAA1`      | TLV identifier           |
 * | `[4:5]`| **length**       | 2B   | `0x0004` (4)  | Value bytes count        |
 * | `[6:9]`| **value**        | 4B   | `[1,2,3,4]`   | Actual data              |
 * | **↓**  | **TOTAL**        | **10B**|             | **Minimum buffer size**  |
 *
 * *Error handling:**
 * - Strict validation: null pointers, buffer overflow, invalid sizes
 * - Duplicate key detection during append
 * - Empty/invalid buffer states
 *
 * *Typical lifecycle:**
 * - Allocate fixed buffer (e.g. noinit/heap/stack)
 * - Append keys: `otapp_msg_tlv_keyAdd(buffer, size, key, len, value)`
 * - Query: `otapp_msg_tlv_getBufferTotalFreeSpace(buffer, size, &free)`
 * - Extract: `otapp_msg_tlv_keyGet(buffer, size, key, &len, value)`
 *
 * *Integration:**
 * - CoAP options, OpenThread discovery payloads, device state serialization
 * - Portable: ESP32/STM32 (little-endian, packed structs)
 * - Thread-safe with external mutex (user-provided buffer)
 *
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
 *  int8_t result = 0;
 *   ot_app_devDrv_t *devDrv_ = otapp_getDevDrvInstance();
 *
 *   otapp_coap_uri_t *urisList = NULL;    
 *   ot_app_size_t uriListSize = 0;
 *
 *   uint8_t *buffer = NULL;
 *   uint16_t bufferSize = 0;
 *   
 *   if (request && devDrv_)
 *   {
 *       // Retrieve the URI list defined in the device driver
 *       urisList = devDrv_->uriGetList_clb();
 *       uriListSize = devDrv_->uriGetListSize;
 *       if(urisList == NULL || uriListSize == 0) return;
 *
 *       // Acquire access to the thread-safe global buffer
 *       bufferSize = otapp_pair_uriResourcesCalculateBufSize(urisList, uriListSize);
 *       buffer = otapp_buf_getWriteOnly_ptr(OTAPP_BUF_KEY_1, bufferSize);
 *       if(buffer == NULL || bufferSize == 0) 
 *       {
 *           otapp_buf_writeUnlock(OTAPP_BUF_KEY_1);
 *           OTAPP_PRINTF(TAG, "ERROR well-known/core: buffer = NULL"); 
 *           return;
 *       }         
 *
 *       // Serialize URI data into TLV format
 *       result = otapp_pair_uriResourcesCreate(urisList, uriListSize, buffer, &bufferSize);
 *       if(result == OTAPP_PAIR_OK)
 *       {
 *           // Send the populated TLV buffer as a CoAP response
 *           otapp_coap_sendResponse(request, aMessageInfo, buffer, bufferSize);
 *           OTAPP_PRINTF(TAG, "well-known/core: sent resources size: %d\n", bufferSize);
 *       }else
 *       {
 *           OTAPP_PRINTF(TAG, "ERROR well-known/core: uriResourcesCreate \n");
 *       }
 *
 *       // unlock the buffer
 *       otapp_buf_writeUnlock(OTAPP_BUF_KEY_1);
 *   }
 * }
 *
 * // Helper function to create a TLV structure from a URI list.
 * int8_t otapp_pair_uriResourcesCreate(otapp_coap_uri_t *uri, uint8_t uriSize, uint8_t *bufferOut, uint16_t *bufferSizeInOut)
 * {   
 *   if(uri == NULL || bufferOut == NULL || bufferSizeInOut == NULL || uriSize == 0 || uriSize > OTAPP_PAIR_URI_MAX)
 *   {
 *       return OTAPP_PAIR_ERROR;
 *   }
 *   uint16_t writtenBufSpace;
 *   // Add TLV block containing the number of available URIs
 *   otapp_msg_tlv_keyAdd(bufferOut, *bufferSizeInOut, OTAPP_PAIR_KEY_URIS_COUNT, sizeof(uriSize), &uriSize);
 *
 *   // Iterate through the list and append device types and URI paths as TLV blocks
 *   for (size_t i = 0; i < uriSize; i++) // quantity_of_uris | uri1_dt | uri1_path | uri2_dt | uri2_path | uri3_dt | uri3_path | ...
 *   {   
 *       // Add device type using an incremented key
 *       otapp_msg_tlv_keyAdd(bufferOut, *bufferSizeInOut, OTAPP_PAIR_KEY_PATTERN + 2*i + 1, sizeof(uri[i].devType), (uint8_t *)&uri[i].devType);
 *
 *       // Add URI path string as the subsequent TLV block
 *       otapp_msg_tlv_keyAdd(bufferOut, *bufferSizeInOut, OTAPP_PAIR_KEY_PATTERN + 2*i + 2, strlen(uri[i].resource.mUriPath), (uint8_t *)uri[i].resource.mUriPath);
 *   }
 *
 *   // Retrieve the final count of written bytes from the buffer header
 *   if(otapp_msg_tlv_getBufferTotalUsedSpace(bufferOut, *bufferSizeInOut, &writtenBufSpace) == OT_APP_MSG_TLV_ERROR) 
 *   {
 *       return OTAPP_PAIR_ERROR;
 *   }
 *   
 *   *bufferSizeInOut = writtenBufSpace;
 *
 *   return OTAPP_PAIR_OK;
 * }
 * @endcode

 * @include ot_app_msg_tlv_example.c
 *
 * @version 0.1
 * @date 29-01-2026
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
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
int8_t otapp_msg_tlv_getBufferTotalFreeSpace(const uint8_t *buffer, const uint16_t bufferSize, uint16_t *freeBufSpaceOut);


int8_t otapp_msg_tlv_getBufferTotalUsedSpace(const uint8_t *buffer, const uint16_t bufferSize, uint16_t *writtenBufSpaceOut);

uint16_t otapp_msg_tlv_calcualeBuffer(uint8_t keyDataLength, uint8_t cnt);

#endif  /* OT_APP_MSG_TLV_H_ */

/**
 * @}
 */