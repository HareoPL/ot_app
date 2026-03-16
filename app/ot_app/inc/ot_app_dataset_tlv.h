/** 
 * @file ot_app_dataset_tlv.h
 * @brief Pre-configured Operational Dataset TLVs for initializing the Thread network.
 * @details see more information in section: @ref ot_app_dataset
 * 
 * @defgroup ot_app_dataset Dataset Management
 * @ingroup ot_app
 * @brief Pre-configured Operational Dataset TLVs for initializing the Thread network.
 * @details
 * @{
 * This module provides static definitions for the OpenThread Operational Dataset using the 
 * Type-Length-Value (TLV) format. This dataset includes critical network parameters required 
 * for a device to attach to or form a specific Thread network.
 * **Dataset Components:**
 * The binary blob `otapp_dataset_tlv` typically contains:
 * - **Active Timestamp:** Timestamp of the dataset generation.
 * - **Channel:** The IEEE 802.15.4 channel to operate on.
 * - **PAN ID:** Personal Area Network Identifier.
 * - **Network Key (Master Key):** 16-byte key for encryption.
 * - **Network Name:** Human-readable string identifying the network.
 * - **Extended PAN ID:** 64-bit unique identifier.
 * - **Mesh-Local Prefix:** IPv6 prefix for mesh-local addresses.
 * **Usage:**
 * This constant is used by @ref otapp_setDataset_tlv during the initialization phase 
 * (`otapp_network_init`) to provision the OpenThread stack if no dataset is currently active.
 * 
 * @version 0.1
 * @date 31-07-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */

#ifndef OT_APP_DATASET_TLV_H_
#define OT_APP_DATASET_TLV_H_

#include "stdint.h"
#include "openthread/dataset.h"

/**
 * @brief Default Operational Dataset encoded as TLV byte array.
 * @details This structure holds the serialized dataset that defines the network credentials.
 * It is applied to the OpenThread stack via `otDatasetSetActiveTlvs()`.
 * **Contents:**
 * - `.mTlvs`: The raw byte array containing the TLV sequence.
 * - `.mLength`: The total length of the valid data in the byte array (111 bytes).
 * @note This dataset corresponds to a specific pre-defined network configuration. 
 * Changing these bytes will change the network the device attempts to join.
 */
const otOperationalDatasetTlvs otapp_dataset_tlv = {
    .mTlvs = {
        0x0e, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x13, 0x4a,
        0x03, 0x00, 0x00, 0x10, 0x35, 0x06, 0x00, 0x04, 0x00, 0x1f, 0xff, 0xe0, 0x02, 0x08, 0x1c, 0xc8,
        0x1a, 0x73, 0xcd, 0x4c, 0x61, 0xfa, 0x07, 0x08, 0xfd, 0xef, 0x86, 0x2f, 0xed, 0x61, 0x7a, 0x32,
        0x05, 0x10, 0x29, 0x47, 0x54, 0xd4, 0x8c, 0x17, 0x3d, 0x2c, 0x01, 0x65, 0x34, 0x8f, 0xc7, 0x4e,
        0xb9, 0x9f, 0x03, 0x0f, 0x4f, 0x70, 0x65, 0x6e, 0x54, 0x68, 0x72, 0x65, 0x61, 0x64, 0x2d, 0x62,
        0x37, 0x66, 0x33, 0x01, 0x02, 0xb7, 0xf3, 0x04, 0x10, 0xef, 0x6c, 0x20, 0x54, 0xdc, 0x92, 0x26,
        0xdc, 0x78, 0xe7, 0x6f, 0x57, 0x54, 0x9f, 0xc1, 0xbd, 0x0c, 0x04, 0x02, 0xa0, 0xf7, 0xf8
    },
    .mLength = 111,
};

#endif  /* OT_APP_DATASET_TLV_H_ */

/**
 * @}
 */