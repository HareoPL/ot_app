/**
 * @file ot_app.h
 * @brief Core types, global configuration macros, and framework initialization routines.
 * @details see more information in section: @ref ot_app_core 
 * 
 * @defgroup ot_app_core Core Definitions & Init
 * @ingroup ot_app
 * @brief Core types, global configuration macros, and framework initialization routines.
 * @details
 * @{
 * This module defines the fundamental structures used across the framework, 
 * including the supported device types (@ref otapp_deviceType_t) and global 
 * accessors for OpenThread instances and network addresses.
 *
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @version 0.1
 * @date 14-07-2025
 * @copyright © 2025 MIT @ref prj_license 
 */

#ifndef THREAD_OT_APP_H_
#define THREAD_OT_APP_H_

#include "hro_utils.h"

#ifdef UNIT_TEST
    #include "mock_ot_app_coap.h"
    #include "mock_ip6.h"
#else
    #include "ot_app_coap.h"
    #include "openthread/dns_client.h"
#endif

/** @name Logging Configuration */
///@{
#define OTAPP_LOG_ENABLE

#ifdef OTAPP_LOG_ENABLE
    #define OTAPP_PRINTF HRO_PRINTF 
#else
    // logs disable
    #define OTAPP_PRINTF(fmt, ...) ((void)0)
#endif
///@}

/** @name Network Configuration */
///@{
#define OTAPP_CCA_THRESHOLD         (-70)   ///< Clear Channel Assessment threshold in dBm
#define OTAPP_UDP_PORT              12345   ///< Default UDP port for application traffic
///@}

/** @name Return Codes */
///@{
#define OTAPP_OK        (-1) ///< Operation successful
#define OTAPP_ERROR     (-2) ///< Generic error
///@}

/** @name Buffer & Limits definitions */
///@{
#define OTAPP_CHAR_BUFFER_SIZE      1024    ///< Size for temporary string buffers
#define OTAPP_PAIRED_DEVICES_MAX    10      ///< Max number of devices to save from DNS query results
#define OTAPP_PAIRED_URI_MAX        3       ///< Max number of URIs per paired device
#define OTAPP_URI_MAX_NAME_LENGHT   24      ///< Max length of URI string name (e.g. "light/dimm")

#define OTAPP_DNS_SRV_NAME_SIZE     64      ///< OT_DNS_MAX_NAME_SIZE full service name: "_coap._udp.default.service.arpa." 
#define OTAPP_DNS_SRV_LABEL_SIZE    32      ///< OT_DNS_MAX_LABEL_SIZE host name: "device1_1_588c81fffe301ea4"
#define OTAPP_DNS_SRV_TXT_SIZE      512     ///< DNS TXT record buffer size
#define OTAPP_DEVICE_NAME_FULL_SIZE OTAPP_DNS_SRV_LABEL_SIZE
///@}

/** @name Driver task config */
///@{

/** * @brief Stack size (in words) for the internal driver polling task. 
 */
#define OTAPP_DRV_TASK_STACK        (3*1024)

/** * @brief Priority for the internal driver polling task. 
 * @details Should be relatively low to allow the OpenThread network stack 
 * and higher priority events to process efficiently.
 */
#define OTAPP_DRV_TASK_PRIORITY     6
///@}

typedef struct ot_app_devDrv_t ot_app_devDrv_t; // forward declaration

/**
 * @brief Supported Device Types.
 * Identifies the functional role of the device within the OpenThread network.
 * This enum is used during the discovery phase to filter devices (e.g., a Switch looking for Lights).
 */
typedef enum {
    OTAPP_NO_DEVICE_TYPE = 0,   ///< Undefined device type
    
    /* Controllers / Inputs */
    OTAPP_CONTROL_PANEL = 1 ,   ///< Central control panel or gateway
    OTAPP_SWITCH,               ///< Physical button or switch

    /* Actuators / Outputs */
    OTAPP_LIGHTING,             ///< Generic lighting device
    OTAPP_LIGHTING_ON_OFF,      ///< Simple On/Off light
    OTAPP_LIGHTING_DIMM,        ///< Dimmable light
    OTAPP_LIGHTING_RGB,         ///< Color changing light (RGB)
    
    /* HVAC */
    OTAPP_THERMOSTAT,                   ///< Generic thermostat
    OTAPP_THERMOSTAT_SET_TEMP,          ///< Thermostat: Set point control
    OTAPP_THERMOSTAT_READ_SET_TEMP,     ///< Thermostat: Read set point
    OTAPP_THERMOSTAT_READ_CURRENT_TEMP, ///< Thermostat: Read ambient temp
    
    /* Sensors & Security */
    OTAPP_SENSOR,               ///< Generic sensor (output only)
    OTAPP_DOOR_LOCK,            ///< Smart door lock
    OTAPP_MOTION_DETECTOR,      ///< PIR or microwave motion sensor
    OTAPP_REMOTE_CONTROL,       ///< Handheld remote
    OTAPP_ENERGY_METER,         ///< Power consumption meter
    OTAPP_SMART_PLUG,           ///< Smart socket/plug
    OTAPP_ENVIRONMENT_SENSOR,   ///< Environmental sensor (Humidity, Air Quality, etc.)
    OTAPP_DOOR_SENSOR,          ///< Reed switch / door open sensor
    OTAPP_ALARM,                ///< Siren or alarm system

    OTAPP_END_OF_DEVICE_TYPE    ///< Sentinel value
} otapp_deviceType_t;

/**
 * @brief Retrieves the singleton instance of the Device Driver.
 * @return ot_app_devDrv_t* Pointer to the driver instance containing hardware callbacks and config.
 */
ot_app_devDrv_t *otapp_getDevDrvInstance(void);

/**
 * @brief Retrieves the global OpenThread instance pointer.
 * @return otInstance* Pointer to the OpenThread stack instance.
 */
otInstance *otapp_getOpenThreadInstancePtr(void);

/**
 * @brief Gets the framework-defined multicast address.
 * Used for group communication or discovery.
 * @return const otIp6Address* Pointer to the multicast IPv6 address.
 */
const otIp6Address *otapp_multicastAddressGet(void);

/**
 * @brief Gets the current Mesh-Local EID (IPv6 address) of this device.
 * @return const otIp6Address* Pointer to the IPv6 address.
 */
const otIp6Address *otapp_ip6AddressGet(void);


/**
 * @brief Prints an IPv6 address to the log output.
 * @param aAddress Pointer to the IPv6 address to print.
 */
void otapp_ip6AddressPrint(const otIp6Address *aAddress);

/**
 * @brief Refreshes and returns the current Mesh-Local EID.
 * Updates the internal cache of the device's IP address from the OpenThread stack.
 * @return const otIp6Address* Pointer to the refreshed IPv6 address.
 */
const otIp6Address *otapp_ip6AddressRefresh(void);

/**
 * @brief Gets the Factory Assigned IEEE EUI-64 address.
 * @return const otExtAddress* Pointer to the hardware MAC address.
 */
const otExtAddress *otapp_macAddrGet(void);

/**
 * @brief Main initialization of the OpenThread Application Framework.
 * @details Performs the following sequence:
 * 1. Initializes the driver singleton instance.
 * 2. Initializes platform-specific ports (RTOS/OpenThread).
 * 3. Sets up shared buffers and pairing modules.
 * 4. **Spawns the internal Driver Task (Conditional)**: Automatically creates a FreeRTOS 
 * task (`otapp_drv_task`) **ONLY IF** the application developer has assigned a valid 
 * function pointer to the `drv->task` callback during device initialization. 
 * @return int8_t @ref OTAPP_OK on success.
 */
int8_t otapp_init(void);

/**
 * @brief Initializes the network layer components.
 * Sets up the operational dataset, device name, CoAP services, and SRP client.
 * Typically called within the OpenThread task context.
 */
void otapp_network_init(void);

/**
 * @brief Applies the Operational Dataset TLV to the OpenThread stack.
 * Configures network parameters like Channel, PANID, Network Key, etc.
 */
void otapp_setDataset_tlv(void);

#endif  /* THREAD_OT_APP_H_ */

/**
 * @}
 */
