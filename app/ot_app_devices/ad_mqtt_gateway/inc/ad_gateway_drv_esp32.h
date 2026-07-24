/**
 * @file ad_gateway_drv_esp32.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Network communication driver for ESP32-based gateways using SIM800 modem.
 * @version 0.4
 * @date 06-05-2026
 * @copyright The MIT License (MIT) Copyright (c) 2026 
 * 
 * @details See more information in section: @ref device_gateway_drv_esp32
 * 
 * @defgroup device_gateway_drv_esp32 ESP32 Gateway Driver
 * @ingroup device_mqtt_gateway
 * @brief Handles coexistence of OpenThread, Cellular (PPP via SIM800), and MQTT.
 * @details
 * 
 * ## 1. Overview
 * The `ad_gateway_drv_esp32` module is a robust network communication driver for ESP32-based gateways. It implements the `ad_gw_drv_t` interface and handles the complex coexistence of a Thread Mesh network (OpenThread) and a Cellular connection (PPP via SIM800 modem) alongside an MQTT client. 
 * 
 * Features a highly asynchronous, non-blocking architecture with built-in auto-recovery mechanisms for dealing with hardware resets, cellular coverage loss, and MQTT broker disconnections.
 * 
 * ---
 * 
 * ## 2. Core Mechanisms
 * 
 * * **Default Network Interface Override:** OpenThread might hijack the default routing. To ensure MQTT traffic is routed through the cellular network, the driver performs a hard-override of the default interface (`esp_netif_set_default_netif`) the moment the `IP_EVENT_PPP_GOT_IP` event is triggered.
 * * **MQTT-Driven Auto-Recovery:** The driver actively monitors the MQTT state. If the client throws an `MQTT_EVENT_DISCONNECTED` event, it immediately triggers the `modem_connect()` logic to re-establish the physical cellular tunnel.
 * * **Graceful Shutdown:** To prevent auto-recovery from fighting intentional device sleep modes, a `modem_isShuttingDown` flag is used. It gracefully disconnects MQTT, deregisters from the cell tower (`AT+COPS=2`), and powers down the modem (`AT+CPOWD=1`).
 * * **Hardware Watchdog (Exponential Backoff):** If the DCE physical device fails to boot, the hardware initialization task will cut off power, multiply the waiting delay (e.g., 20s -> 40s -> 80s), and attempt a cold boot infinitely to prevent the device from bricking in remote locations.
 * 
 * ---
 * 
 * ## 3. System Execution Flow & Real-World Logs
 * 
 * The driver divides the modem lifecycle into specialized FreeRTOS tasks to prevent the main application thread from blocking.
 * 
 * ### A. Hardware Initialization & Exponential Backoff
 * The `modem_hardwareInitTask` initializes UART and actively polls the modem with `esp_modem_sync()`. If the modem does not respond within `MODEM_HW_INIT_TASK_RETRIES_MAX` attempts, the task cuts power, waits using an exponential backoff algorithm, and retries.
 * 
 * **Execution Log (Hardware Boot Struggle & Success):**
 * ```text
 * $$ ad_gateway_drv_esp32  &&--> Enable modem power...
 * $$ ad_gateway_drv_esp32  &&--> modem is not responding... Attempt: 15/15
 * $$ ad_gateway_drv_esp32  &&--> ERROR modem did not get up. Hardware ERROR
 * $$ ad_gateway_drv_esp32  &&--> Disable modem power...
 * $$ ad_gateway_drv_esp32  &&--> Waiting 20000 ms before next boot attempt...
 * $$ ad_gateway_drv_esp32  &&--> Enable modem power...
 * $$ ad_gateway_drv_esp32  &&--> modem is not responding... Attempt: 15/15
 * $$ ad_gateway_drv_esp32  &&--> ERROR modem did not get up. Hardware ERROR
 * $$ ad_gateway_drv_esp32  &&--> Disable modem power...
 * $$ ad_gateway_drv_esp32  &&--> Waiting 40000 ms before next boot attempt...
 * $$ ad_gateway_drv_esp32  &&--> Enable modem power...
 * $$ ad_gateway_drv_esp32  &&--> hardware woken up. Lanch modem_conn_task (PPP)
 * $$ ad_gateway_drv_esp32  &&--> Logged in to the network! (Status: 1)
 * $$ ad_mqtt_gateway       &&--> got ip addr
 * $$ ad_gateway_drv_esp32  &&--> IP: 10.114.144.240
 * $$ ad_mqtt_gateway       &&--> MQTT connected with broker
 * ```
 * 
 * ### B. Disconnection & Auto-Recovery Sequence
 * Triggered automatically if LwIP detects a lost IP or if the MQTT client loses its TCP connection. The recovery task executes a "Soft Reset" (`+++`), drops old sessions (`ATH`), and restarts network registration.
 * 
 * **Execution Log (Connection Drop & Recovery):**
 * ```text
 * E (133866) transport_base: tcp_read error, errno=Software caused connection abort
 * $$ ad_mqtt_gateway       &&--> MQTT error
 * $$ ad_gateway_drv_esp32  &&--> modem connect task is running
 * $$ ad_mqtt_gateway       &&--> MQTT disconnected
 * $$ ad_gateway_drv_esp32  &&--> Whait when SIM CARD will connect to network ...
 * $$ ad_gateway_drv_esp32  &&--> Logged in to the network! (Status: 1)
 * $$ ad_gateway_drv_esp32  &&--> switch to DATA mode(PPP)...
 * $$ ad_mqtt_gateway       &&--> got ip addr
 * $$ ad_gateway_drv_esp32  &&--> IP: 10.77.128.254
 * $$ ad_mqtt_gateway       &&--> MQTT connected with broker
 * ```
 * 
 * ---
 * 
 * ## 4. API Reference (`ad_gw_drv_t`)
 * 
 * The singleton driver interface contains the following methods:
 * 
 * | Module | Function | Description |
 * | :--- | :--- | :--- |
 * | **System** | `init()` | Bootstraps the driver, creates PPP netif, and launches the HW polling task. |
 * | **Modem** | `connect()` | Safely creates `modem_conn_task` if not already running or shutting down. |
 * | **Modem** | `disconnect()` | Executes the Graceful Shutdown sequence. |
 * | **MQTT** | `publish(...)` | Wrapper for `esp_mqtt_client_publish`. |
 * | **MQTT** | `subscribeSingle(...)`| Wrapper for `esp_mqtt_client_subscribe_single`. |
 * | **MQTT** | `unSubscribeSingle(...)`| Wrapper for `esp_mqtt_client_unsubscribe`. |
 * 
 * ### `void ad_gw_drv_register_event_cb(ad_gw_drv_event_cb_t callback)`
 * Registers an upper-level application callback to listen for driver state changes.
 * Dispatched events include:
 * * `AD_GW_DRV_GOT_IP`: Tunnel established, override applied, MQTT started.
 * * `AD_GW_DRV_LOST_IP`: PPP tunnel dropped.
 * * `AD_GW_DRV_MQTT_CONNECTED`: Connected to remote broker.
 * * `AD_GW_DRV_MQTT_DISCONNECTED`: Connection dropped (triggers auto-recovery).
 * * `AD_GW_DRV_MQTT_ERROR`: General transport/protocol error.
 * 
 * ---
 * 
 * ## 5. Configuration Macros
 * 
 * The module can be adapted to different hardware revisions by adjusting the macros defined below. They control modem boot timeouts, UART pins, APN settings, and MQTT broker details.
 * 
 * @{
 */

#ifndef AD_GATEWAY_DRV_ESP32_H_
#define AD_GATEWAY_DRV_ESP32_H_

#include "ad_gateway_drv.h"

/* -------------------------------------------------------------------------
 * MODEM TIMEOUTS & TASKS
 * ------------------------------------------------------------------------- */
#define MODEM_CONNECT_TASK_RETRIES_MAX  60      ///< Max seconds connect task waits for network attachment.
#define MODEM_HW_INIT_TASK_RETRIES_MAX  15      ///< Max seconds hardware init task waits for modem boot.

/* -------------------------------------------------------------------------
 * MODEM HARDWARE CONFIGURATION
 * ------------------------------------------------------------------------- */
#define MODEM_MODEL                     ESP_MODEM_DCE_SIM800 ///< Selected DCE model from esp_modem library.
#define MODEM_TXD_PIN                   (GPIO_NUM_6)         ///< UART TX pin connected to the modem.
#define MODEM_RXD_PIN                   (GPIO_NUM_7)         ///< UART RX pin connected to the modem.
#define MODEM_UART_PORT                 (UART_NUM_1)         ///< UART peripheral port used for communication.
#define MODEM_APN                       ("internet")         ///< Cellular network Access Point Name.

/* -------------------------------------------------------------------------
 * MQTT CONFIGURATION
 * ------------------------------------------------------------------------- */
// #define MQTT_BROKER                     ("mqtt://35.157.202.98") ///< Target MQTT broker URI (e.g., broker.hivemq.com).
#define MQTT_BROKER                     ("mqtt://3.124.246.66") ///< Target MQTT broker URI (e.g., broker.hivemq.com).
#define MQTT_KEEPALIVE                  60                       ///< MQTT ping keep-alive interval in seconds.
#define MQTT_TIMEOUT_MS                 10000                    ///< Network timeout for MQTT operations in milliseconds.
#define MQTT_BUFFER_TX_RX_SIZE          1024                     ///< Size of the allocated MQTT TX/RX heap buffer.

/**
 * @brief Returns the singleton instance of the gateway driver interface.
 * 
 * @details Populates the driver interface with hardware-specific function pointers 
 * for the ESP32 (init, modem connection control, and MQTT publishing/subscribing).
 * 
 * @return ad_gw_drv_t* Pointer to the initialized `ad_gw_drv_t` struct.
 */
ad_gw_drv_t *ad_gateway_drv_getEsp32(void);

#endif  /* AD_GATEWAY_DRV_ESP32_H_ */

/** @} */