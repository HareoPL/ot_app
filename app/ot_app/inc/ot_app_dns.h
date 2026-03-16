/**
 * @file ot_app_dns.h
 * @brief Handles network service browsing, hostname resolution, and initiates the device pairing sequence. 
 * @details see more information in section: @ref ot_app_dns 
 * 
 * @defgroup ot_app_dns DNS Service Discovery
 * @ingroup ot_app
 * @brief Handles network service browsing, hostname resolution, and initiates the device pairing sequence. 
 * @details
 * @{
 * This module acts as the entry point for finding other devices on the Thread network.
 * It utilizes the OpenThread DNS Client API to browse for specific service types (e.g., `_coap._udp`).
 *  **Workflow:**
 * 1. **Browse:** Scans the network for services matches the provided service name.
 * 2. **Resolve:** For every discovered service instance, it resolves the hostname to an IPv6 address.
 * 3. **Queue:** Once resolved, the device information (Name + IP) is added to the pairing queue via `otapp_pair_addToQueue`.
 *  @section dns_logs Discovery & Pairing Log Sequence
 * The following log output demonstrates a successful discovery cycle where the DNS module finds devices,
 * resolves them, and passes them to the Pairing and CoAP modules for verification:
 *  @code
 * $$ ot_app_dns   --> DNS client browse. Checked reported services:
 * $$ ot_app_dns   --> DNS browse response for _coap._udp.default.service.arpa.
 * $$ ot_app_dns   --> Device full name (label): device1_2_588c81fffe3029f0
 * $$ ot_app_dns   -->
 * $$ ot_app_dns   --> Device full name (label): device1_3_588c81fffe3035a4
 * $$ ot_app_dns   -->
 * $$ ot_app_dns   --> Device full name (label): device1_2_588c81fffe301ea4
 * $$ ot_app_dns   -->
 * $$ ot_app_dns   --> DNS: Add item to queue
 * $$ ot_app_pair  --> Pairing device: device1_2_588c81fffe3029f0
 * $$ ot_app_pair  -->       = current device, or NOT allowed
 * $$ ot_app_dns   --> DNS: Add item to queue
 * $$ ot_app_pair  --> Pairing device: device1_3_588c81fffe3035a4
 * $$ ot_app_coap  --> CoAP sentGetByte to .well-known/core
 * $$ ot_app_coap  --> CoAP sent WELL KNOWN URI
 * $$ ot_app_pair  -->       success paired on index 0
 * $$ ot_app_dns   --> DNS: Add item to queue
 * $$ ot_app_pair  --> Pairing device: device1_2_588c81fffe301ea4
 * $$ ot_app_pair  -->       = current device, or NOT allowed
 * $$ ot_app_pair  --> responseHandlerUriWellKnown IN
 * $$ ot_app_coap  --> CoAP sent SubscribeRequest
 * $$ ot_app_coap  --> CoAP sent SubscribeRequest
 * $$ ot_app_coap  --> CoAP sent SubscribeRequest
 * $$ ad_button    --> Dev Button detect DEVICE! device1_3_588c81fffe3035a4
 * $$ ad_button    -->        uri 0: light/on_off
 * $$ ad_button    -->        uri 1: light/dimm
 * $$ ad_button    -->        uri 2: light/rgb
 * @endcode
 * 
 * @version 0.1
 * @date 05-09-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */

#ifndef OTAPP_DNS_H_
#define OTAPP_DNS_H_

#include "openthread/instance.h"
#include "hro_utils.h"
#include "stdint.h"

/** @name Return Codes */
///@{
#define OTAPP_DNS_OK        (-1) ///< Operation successful
#define OTAPP_DNS_ERROR     (-2) ///< Generic error (e.g., invalid arguments or buffer allocation fail)
///@}

/**
 * @brief Initiates a DNS-SD browse operation for a specific service.
 *  @details This function starts the browsing process. When services are found, 
 * the internal callback chain is triggered:
 * `otapp_dnsClientBrowseResponseCallback` -> `otapp_dnsClientResolve` -> `otapp_dnsPairDevice`.
 *  This ultimately pushes discovered devices into the pairing queue.
 *  @param[in] instance     Pointer to the OpenThread instance structure.
 * @param[in] serviceName  The service to browse for (e.g., "_coap._udp.default.service.arpa").
 *  @return int8_t 
 * - @ref OTAPP_DNS_OK on success.
 * - @ref OTAPP_DNS_ERROR if arguments are NULL or the OT stack returns an error.
 */
int8_t otapp_dnsClientBrowse(otInstance *instance, const char *serviceName);

#endif  /* OTAPP_DNS_H_ */

/**
 * @}
 */