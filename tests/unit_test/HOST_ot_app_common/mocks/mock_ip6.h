#ifndef MOCK_IP6_H_
#define MOCK_IP6_H_

#include "hro_utils.h"

#define OT_IP6_PREFIX_SIZE 8                             ///< Size of an IPv6 prefix (bytes)
#define OT_IP6_PREFIX_BITSIZE (OT_IP6_PREFIX_SIZE * 8)   ///< Size of an IPv6 prefix (bits)
#define OT_IP6_IID_SIZE 8                                ///< Size of an IPv6 Interface Identifier (bytes)
#define OT_IP6_ADDRESS_SIZE 16                           ///< Size of an IPv6 address (bytes)
#define OT_IP6_ADDRESS_BITSIZE (OT_IP6_ADDRESS_SIZE * 8) ///< Size of an IPv6 address (bits)
#define OT_IP6_HEADER_SIZE 40                            ///< Size of an IPv6 header (bytes)
#define OT_IP6_HEADER_PROTO_OFFSET 6                     ///< Offset of the proto field in the IPv6 header (bytes)

#define OT_TOOL_PACKED_BEGIN
#define OT_TOOL_PACKED_FIELD __attribute__((packed))
#define OT_TOOL_PACKED_END __attribute__((packed))
#define OT_TOOL_WEAK __attribute__((weak))

typedef struct otExtAddress otExtAddress;
typedef struct otInstance otInstance;
/**
 * @struct otIp6InterfaceIdentifier
 *
 * Represents the Interface Identifier of an IPv6 address.
 */
OT_TOOL_PACKED_BEGIN
struct otIp6InterfaceIdentifier
{
    union OT_TOOL_PACKED_FIELD
    {
        uint8_t  m8[OT_IP6_IID_SIZE];                     ///< 8-bit fields
        uint16_t m16[OT_IP6_IID_SIZE / sizeof(uint16_t)]; ///< 16-bit fields
        uint32_t m32[OT_IP6_IID_SIZE / sizeof(uint32_t)]; ///< 32-bit fields
    } mFields;                                            ///< The Interface Identifier accessor fields
} OT_TOOL_PACKED_END;

/**
 * Represents the Interface Identifier of an IPv6 address.
 */
typedef struct otIp6InterfaceIdentifier otIp6InterfaceIdentifier;

/**
 * @struct otIp6NetworkPrefix
 *
 * Represents the Network Prefix of an IPv6 address (most significant 64 bits of the address).
 */
OT_TOOL_PACKED_BEGIN
struct otIp6NetworkPrefix
{
    uint8_t m8[OT_IP6_PREFIX_SIZE]; ///< The Network Prefix.
} OT_TOOL_PACKED_END;

/**
 * Represents the Network Prefix of an IPv6 address (most significant 64 bits of the address).
 */
typedef struct otIp6NetworkPrefix otIp6NetworkPrefix;

/**
 * @struct otIp6Address
 *
 * Represents an IPv6 address.
 */

 struct otIp6AddressComponents
{
    otIp6NetworkPrefix       mNetworkPrefix; ///< The Network Prefix (most significant 64 bits of the address)
    otIp6InterfaceIdentifier mIid;           ///< The Interface Identifier (least significant 64 bits of the address)
} OT_TOOL_PACKED_END;

/**
 * Represents the components of an IPv6 address.
 */
typedef struct otIp6AddressComponents otIp6AddressComponents;

OT_TOOL_PACKED_BEGIN
struct otIp6Address
{
    union OT_TOOL_PACKED_FIELD
    {
        uint8_t                m8[OT_IP6_ADDRESS_SIZE];                     ///< 8-bit fields
        uint16_t               m16[OT_IP6_ADDRESS_SIZE / sizeof(uint16_t)]; ///< 16-bit fields
        uint32_t               m32[OT_IP6_ADDRESS_SIZE / sizeof(uint32_t)]; ///< 32-bit fields
        otIp6AddressComponents mComponents;                                 ///< IPv6 address components
    } mFields;                                                              ///< IPv6 accessor fields
} OT_TOOL_PACKED_END;

/**
 * Represents an IPv6 address.
 */
typedef struct otIp6Address otIp6Address;

typedef struct otMessage
{
}otMessage;
typedef struct otMessageInfo
{   
} otMessageInfo;

#endif  /* MOCK_IP6_H_ */
