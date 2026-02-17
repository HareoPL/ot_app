/**
 * @file ot_app_deviceName.h
 * @brief Handles generation and parsing of unique device identifiers for discovery and pairing.
 * @details ee more information in section: @ref ot_app_deviceName 
 * 
 * @defgroup ot_app_deviceName Device Naming
 * @ingroup ot_app
 * @brief Handles generation and parsing of unique device identifiers for discovery and pairing.
 * @details
 * @{
 * This module enforces a strict naming convention for OpenThread devices to facilitate 
 * automatic discovery, logical grouping, and type identification.
 * **Naming Format:** `GroupName_DeviceType_EUI64`
 * **Example:** `kitchen_3_588c81fffe301ea4`
 * - **GroupName** (`kitchen`): User-defined string used for grouping. Devices check this to decide if they should pair.
 * - **DeviceType** (`3`): Numeric identifier from @ref otapp_deviceType_t (e.g., 3 = LIGHTING).
 * - **EUI64** (`588c...`): Unique 64-bit hardware identifier ensuring name uniqueness on the network.
 * This structure allows devices to filter traffic (e.g., "I am a Switch in group 'kitchen', I look for Lights in group 'kitchen'").
 * 
 * @version 0.1
 * @date 26-08-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */

#ifndef OT_APP_DEVICENAME_H_
#define OT_APP_DEVICENAME_H_

#include "hro_utils.h"

#ifndef UNIT_TEST
    #include "ot_app.h"
#else
    #include "mock_ot_app.h" 
    #include "mock_ip6.h"    
    #include "mock_mocks.h"     
#endif

/** @name Comparison Results */
///@{
#define OTAPP_DEVICENAME_IS                 (1)  ///< Names/Groups match
#define OTAPP_DEVICENAME_IS_NOT             (2)  ///< Names/Groups do not match
#define OTAPP_DEVICENAME_OK                 (-1) ///< Operation successful
///@}

/** @name Error Codes */
///@{
#define OTAPP_DEVICENAME_ERROR              (-2) ///< Generic error (null ptr, invalid format)
#define OTAPP_DEVICENAME_TOO_LONG           (-3) ///< String exceeds buffer limits
#define OTAPP_DEVICENAME_TOO_SHORT          (-4) ///< String is too short to be valid
#define OTAPP_DEVICENAME_BUFFER_TOO_SMALL   (-5) ///< Destination buffer insufficient
#define OTAPP_DEVICENAME_CALL_DEVICE_NAME_SET_FN   (-6) ///< Name not initialized yet
///@}

/** @name Buffer Limits & Configuration */
///@{
#define OTAPP_DEVICENAME_MAX_DEVICE_TYPE    OTAPP_END_OF_DEVICE_TYPE

#ifndef OTAPP_DEVICENAME_FULL_SIZE
    #define OTAPP_DEVICENAME_FULL_SIZE OTAPP_DNS_SRV_LABEL_SIZE ///< Max size for full label (32 bytes)
#endif

#define OTAPP_DEVICENAME_SIZE               (OTAPP_DEVICENAME_FULL_SIZE - 22) ///< Max user GroupName length (~10 chars)
#define OTAPP_DEVICENAME_MIN_SIZE           (OTAPP_DEVICENAME_FULL_SIZE - OTAPP_DEVICENAME_SIZE + 1) ///< Min required size for metadata
#define OTAPP_DEVICENAME_MIN_ADD_DOMAIN_BUFFER_SIZE           (2 * OTAPP_DEVICENAME_FULL_SIZE) ///< Buffer safety margin for DNS domain
///@}

/**
 * @brief Sets the device Group Name and Type, generating the Full Name.
 * @details Constructs the full unique identifier string using the provided user name, 
 * device type, and the hardware EUI64 address obtained from the OpenThread stack.
 * @param deviceName    [in] User-defined group name (e.g., "kitchen"). Max ~10 chars.
 * @param deviceType    [in] Device functional type (@ref otapp_deviceType_t).
 * @return int8_t       @ref OTAPP_DEVICENAME_OK on success, or error code.
 */
int8_t otapp_deviceNameSet(const char *deviceName, const otapp_deviceType_t deviceType);

/**
 * @brief Retrieves the current device's Full Name string.
 * @return const char* Pointer to the internal static buffer containing the name 
 * (e.g., "kitchen_3_588c81fffe301ea4").
 */
const char *otapp_deviceNameFullGet(void);

/**
 * @brief Checks if the provided full name string is identical to this device's name.
 * @details Used to prevent a device from pairing with itself during discovery.
 * @param deviceNameFull [in] String to compare.
 * @return int8_t        @ref OTAPP_DEVICENAME_IS (match), @ref OTAPP_DEVICENAME_IS_NOT (no match).
 */
int8_t otapp_deviceNameFullIsSame(const char *deviceNameFull);

/**
 * @brief Checks if the Group Name (prefix) matches this device's Group Name.
 * @details Parses the input string to extract the prefix (before the first '_')
 * and compares it with the local device group. This determines if devices belong to the same logical group.
 * @param deviceNameFull [in] Full name string of the remote device.
 * @param stringLength   [in] Length of the string to check.
 * @return int8_t        @ref OTAPP_DEVICENAME_IS (groups match), @ref OTAPP_DEVICENAME_IS_NOT (different groups).
 */
int8_t otapp_deviceNameIsSame(const char *deviceNameFull, uint8_t stringLength);

/**
 * @brief Extracts the Device Type ID from a Full Name string.
 * @details Parses the middle section of the name string (`Name_TYPE_EUI`).
 * @param deviceNameFull [in] Full name string.
 * @param stringLength   [in] Length of the string.
 * @return int16_t       Device Type ID (@ref otapp_deviceType_t) or error code.
 */
int16_t otapp_deviceNameGetDevId(const char *deviceNameFull, uint8_t stringLength);

/**
 * @brief Appends the OpenThread default service domain to a device name.
 * @details Transforms "device_name" into "device_name.default.service.arpa." for DNS-SD queries.
 * @param[in,out] deviceFullName Buffer containing the name. Result is written back here.
 * @param[in]     bufLength      Size of the buffer (must be large enough).
 * @return int8_t @ref OTAPP_DEVICENAME_OK on success.
 */
int8_t otapp_deviceNameFullAddDomain(char *deviceFullName, uint16_t bufLength);

/**
 * @brief Strips the domain suffix to extract the pure Device Full Name.
 * @details Transforms "device_name.default.service.arpa." back into "device_name".
 * Modifies the input buffer by inserting a null terminator.
 * @param[in,out] hostName Buffer containing the full DNS hostname.
 * @return int8_t @ref OTAPP_DEVICENAME_OK on success.
 */
int8_t otapp_hostNameToDeviceNameFull(char *hostName);

/**
 * @brief Determines if a discovered device is a valid candidate for pairing.
 * @details Checks two conditions:
 * 1. The device is NOT itself (@ref otapp_deviceNameFullIsSame returns IS_NOT).
 * 2. The device belongs to the same Group (@ref otapp_deviceNameIsSame returns IS).
 * @param deviceFullName Full name of the discovered device.
 * @return int8_t @ref OTAPP_DEVICENAME_IS if matching, otherwise IS_NOT or ERROR.
 */
int8_t otapp_deviceNameIsMatching(const char *deviceFullName);

/**
 * @brief Clears the internal device name buffer.
 */
void otapp_deviceNameDelete(void);

/**
 * @brief Extracts the EUI-64 suffix from a Device Full Name.
 * @details Locates the last underscore and returns a pointer to the EUI hex string part.
 * @param deviceNameFull    [in] Full name string.
 * @param stringLength      [in] Length of the string.
 * @param outEuiChrPtr      [out] Pointer to the start of the EUI string within the input buffer.
 * @return int8_t           @ref OTAPP_DEVICENAME_OK or error.
 */
int8_t otapp_deviceNameFullToEUI(const char *deviceNameFull, uint8_t stringLength, char **outEuiChrPtr);

/**
 * @brief Compares the EUI-64 in a name string with a raw EUI string.
 * @param deviceNameFull    [in] Full device name string.
 * @param eui               [in] Hex string of the EUI to compare against.
 * @return int8_t           @ref OTAPP_DEVICENAME_IS if match.
 */
int8_t otapp_deviceNameEuiIsSame(const char *deviceNameFull, const char *eui);

#endif  /* OT_APP_DEVICENAME_H_ */

/**
 * @}
 */