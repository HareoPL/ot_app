#ifndef MOCK_OT_APP_COAP_H_
#define MOCK_OT_APP_COAP_H_

#include "mock_ip6.h"
#include "hro_utils.h"
#include <string.h>

#include "fff.h"

typedef enum {
    OTAPP_URI_NO_URI_INDEX = 0,
    
    OTAPP_URI_WELL_KNOWN_CORE,
    OTAPP_URI_TEST,
    OTAPP_URI_TEST_LED,
    OTAPP_URI_PARING_SERVICES,
    OTAPP_URI_MOCK_5,
    OTAPP_URI_MOCK_6,
    OTAPP_URI_MOCK_7,
    OTAPP_URI_MOCK_8,
    OTAPP_URI_MOCK_9,
    OTAPP_URI_MOCK_10,
    OTAPP_URI_MOCK_11,
    OTAPP_URI_MOCK_12,
    OTAPP_URI_MOCK_13,
    OTAPP_URI_MOCK_14,
    OTAPP_URI_MOCK_15,
    OTAPP_URI_MOCK_16,
    OTAPP_URI_MOCK_17,
    OTAPP_URI_MOCK_18,
    OTAPP_URI_MOCK_19,
    OTAPP_URI_MOCK_20,

    OTAPP_URI_END_OF_INDEX,
}otapp_coap_uriIndex_t;

// from coap.h

typedef void (*otCoapRequestHandler)();

typedef struct otCoapResource
{
    const char            *mUriPath; ///< The URI Path string
    otCoapRequestHandler   mHandler; ///< The callback for handling a received request
    void                  *mContext; ///< Application-specific context
    struct otCoapResource *mNext;    ///< The next CoAP resource in the list
} otCoapResource;

// end of coap.h

DECLARE_FAKE_VALUE_FUNC(const char *, otapp_coap_getUriNameFromDefault, otapp_coap_uriIndex_t);

void otapp_ip6AddressPrint(const otIp6Address *aAddress);

#endif  /* MOCK_OT_APP_COAP_H_ */
