/**
 * @file ot_app_coap_uri.h
 * @brief Callback functions handling incoming CoAP requests for specific URI paths.
 * @details see more information in section: @ref ot_app_coap_handlers 
 * 
 * @defgroup ot_app_coap_handlers CoAP Resource Handlers
 * @ingroup ot_app
 * @brief Callback functions handling incoming CoAP requests for specific URI paths.
 * @details
 * @{
 * This module defines the "Business Logic" of the CoAP server. Each function corresponds 
 * to a specific resource (URI) exposed by the device. These handlers are registered 
 * in the main CoAP module and invoked automatically by the OpenThread stack when 
 * a matching request arrives.
 * **Key Handlers:**
 * - **LED Control:** Handles requests to switch lights/LEDs on or off.
 * - **Pairing Services:** Handles device discovery and exchange of capabilities (TLV).
 * - **Subscription Notify:** Client-side handler for processing incoming Observe notifications.
 * 
 * @version 0.1
 * @date 24-07-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */
#ifndef OT_APP_COAP_URI_TEST_H_
#define OT_APP_COAP_URI_TEST_H_

#ifdef UNIT_TEST
    #include "mock_ot_app_coap.h"
#else
    #include "ot_app_coap.h"
#endif

/**
 * @brief Handler for a generic test resource.
 * @details Used for verifying connectivity and basic CoAP functionality (Ping/Pong).
 * Typically responds with a simple ACK or status payload.
 * @param[in] aContext      User context pointer (unused).
 * @param[in] request       Pointer to the incoming CoAP request message.
 * @param[in] aMessageInfo  Pointer to message metadata (source IP, port, etc.).
 */
void otapp_coap_uri_testHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo);

/**
 * @brief Handler for LED control resource (e.g., "light/on_off").
 * @details Processes PUT/POST requests to change the state of the device's LED or relay.
 * Parses the payload (e.g., "1" or "0") and calls the hardware driver to execute the action.
 * @param[in] aContext      User context pointer.
 * @param[in] request       Pointer to the incoming CoAP request message.
 * @param[in] aMessageInfo  Pointer to message metadata.
 */
void otapp_coap_uri_ledControlHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo);

/**
 * @brief Handler for Pairing and Discovery services (e.g., "pairing/service").
 * @details This is the core endpoint for the pairing process. 
 * - **GET:** Returns the device's capabilities and supported URIs encoded in TLV format.
 * - **POST:** May be used to initiate a handshake or confirm pairing using `otapp_pair_uriParseMessage`.
 * @param[in] aContext      User context pointer.
 * @param[in] request       Pointer to the incoming CoAP request message.
 * @param[in] aMessageInfo  Pointer to message metadata.
 */
void otapp_coap_uri_paringServicesHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo);

/**
 * @brief Client-side Handler for incoming Observe notifications.
 * @details This function is NOT a server resource (URI) handler in the traditional sense. 
 * It is the callback registered when this device subscribes to a remote resource. 
 * It is called when a remote device sends a notification (e.g., "Light status changed").
 * **Logic:**
 * 1. Extracts the CoAP Token from the message.
 * 2. Matches the Token using `oac_uri_obs` to identify the sender device.
 * 3. Updates the internal state mirroring the remote device via `oac_uri_obs_parseMessageFromNotify`.
 * @param[in] aContext      User context pointer.
 * @param[in] request       Pointer to the incoming notification message.
 * @param[in] aMessageInfo  Pointer to message metadata.
 */
void otapp_coap_uri_subscribedHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo);


void ad_temp_uri_well_knownCoreHandle(void *aContext, otMessage *request, const otMessageInfo *aMessageInfo);

#endif  /* OT_APP_COAP_URI_TEST_H_ */

/**
 * @}
 */