/**
 * @file ot_app_srp_client.h
 * @brief Manages Service Registration Protocol (SRP) for network discoverability.
 * @details see more information in section: @ref ot_app_srp 
 * 
 * @defgroup ot_app_srp SRP Client Wrapper
 * @ingroup ot_app
 * @brief Manages Service Registration Protocol (SRP) for network discoverability.
 * @details
 * @{
 * This module handles the registration of the device's Host Name, IP Address, and Services 
 * on the Thread Network's SRP Server (usually the Border Router).
 * **Purpose:**
 * Without SRP registration, other devices cannot resolve this device's name to an IP address 
 * using DNS-SD (@ref ot_app_dns), effectively making this device invisible for pairing.
 * **Key Functions:**
 * - Registers the device hostname (e.g., `device_type_EUI64`).
 * - Registers the `_coap._udp` service.
 * - Manages Lease Renewals automatically via OpenThread Auto-Start mode.
 * @version 0.1
 * @date 06-09-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */
#ifndef OT_APP_SRP_CLIENT_H_
#define OT_APP_SRP_CLIENT_H_

#include "hro_utils.h"
#include "openthread/instance.h"


#define OTAPP_DNS_LEASE_TASK_DELAY  300  // in secounds = 5m
#define OTAPP_DNS_LEASE_TIME        7200 // in secounds = 2h
#define OTAPP_DNS_LEASE_GUARD       (4 * OTAPP_DNS_LEASE_TASK_DELAY) // 20 min before end the time lease
#define OTAPP_DNS_M_KEY_LEASE_TIME  86400


/**
 * @brief Updates the registered Host Address on the SRP Server.
 * @details This function should be called when the device's Mesh-Local EID (RLOC/ML-EID) changes.
 * It ensures the DNS server points to the current, valid IPv6 address of this device.
 * @param instance Pointer to the OpenThread instance.
 */
void otapp_srpClientUpdateHostAddress(otInstance *instance);

/**
 * @brief Initializes the SRP Client and starts the registration process.
 * @details This function:
 * 1. Sets the device host name (from @ref ot_app_deviceName).
 * 2. Enables the OpenThread SRP Client Auto-Start mode.
 * 3. Registers the default services.
 * @return int8_t Result of the operation (typically 0/OK).
 */
void otapp_srpInit();

#endif  /* OT_APP_SRP_CLIENT_H_ */

/**
 * @}
 */