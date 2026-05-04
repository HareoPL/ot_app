/**
 * @file ad_gateway.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Main application logic for the IoT Gateway (OpenThread <-> GSM/MQTT).
 * @version 1.0
 * @date 08-05-2026
 * @copyright The MIT License (MIT) Copyright (c) 2026
 * @defgroup device_mqtt_gateway MQTT GSM Gateway
 * @ingroup devices
 * @brief Edge gateway integrating local OpenThread Mesh network with the Cloud (MQTT) via GSM.
 * @details
 * ## 1. System Architecture & Role
 * The `ad_gateway` module acts as the central Edge Device. Its primary responsibility is to 
 * collect information from the internal OpenThread network from all end-devices:
 * - **Actuators:** Devices performing actions (e.g., light switches, roller shutter controllers).
 * - **Sensors:** Devices reporting environmental states (e.g., temperature, window sensors, rain sensors).
 * This module translates local protocols (CoAP/URI) into the global IoT protocol (MQTT), 
 * enabling bi-directional communication and remote control of OpenThread devices from the internet.
 * ---
 * ## 2. Topic Mapping (OpenThread URI -> MQTT Topic)
 * The gateway automatically maps the paths (URIs) of discovered devices to standardized MQTT topics. 
 * The MQTT topic format is derived from the device label and the specific URI of the functionality.
 * **Example from system logs (Device Discovery):**
 * ```text
 * $$ ad_mqtt_gateway  &&--> Detect DEVICE! device1_3_588c81fffe3035a4
 * $$ ad_mqtt_gateway  &&-->        uri 0: light/on_off
 * $$ ad_mqtt_gateway  &&-->        uri 1: light/dimm
 * $$ ad_mqtt_gateway  &&-->        uri 2: light/rgb
 * ```
 * **Generated MQTT Topics:**
 * Based on the discovery, the gateway subscribes to and publishes on:
 * - `device1/light/on_off`
 * - `device1/light/dimm`
 * - `device1/light/rgb`
 * This mapping allows external cloud services to control end-devices by publishing to these topics 
 * and monitor device states by subscribing to them.
 * ---
 * ## 3. Connectivity & "Fight for Life" Mechanism
 * As a remote cellular device, the modem must maintain connectivity under harsh conditions, 
 * including signal loss, hardware freezes, and ISP socket drops. 
 * - **Connection Process:** The initialization is fully asynchronous. It handles PPP negotiation, 
 * IP assignment, and routing overrides to ensure global reachability.
 * For hardware-level details, see: @ref device_gateway_drv_esp32 "ESP32 Gateway Driver".
 * - **The "Fight for Life" Algorithm:** When the hardware becomes unresponsive or network 
 * registration fails, the system performs a hardware power cycle. To prevent being blacklisted 
 * by the ISP (anti-DDoS/flood protection), the driver implements an **Exponential Backoff** algorithm, doubling the wait time between consecutive reset attempts.
 * See implementation details: @ref device_gateway_drv_esp32.
 * - **Auto-Recovery:** TCP socket failures trigger a recovery sequence including soft resets (`+++`), 
 * session termination (`ATH`), and immediate tunnel re-establishment with topic re-subscription.
 * See recovery flow: @ref device_gateway_drv_esp32.
 * @{
 */

#ifndef AD_MQTT_GATEWAY_H_
#define AD_MQTT_GATEWAY_H_

#include "ad_gateway_drv.h"

/**
 * @brief Initializes the gateway module.
 * @details Bridges the abstract hardware driver (`ad_gw_drv_t`) with the OpenThread framework. 
 * Sets up MQTT queues, registers device discovery callbacks, and triggers the hardware boot sequence.
 * @param gateway Pointer to the hardware-specific driver implementation (e.g., from `ad_gateway_drv_getEsp32()`).
 */
void ad_gatewayInit(ad_gw_drv_t *gateway);

#endif  /* AD_MQTT_GATEWAY_H_ */

/** @} */