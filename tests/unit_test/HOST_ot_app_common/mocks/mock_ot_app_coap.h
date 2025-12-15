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

typedef enum otCoapOptionType
{
    OT_COAP_OPTION_IF_MATCH       = 1,  ///< If-Match
    OT_COAP_OPTION_URI_HOST       = 3,  ///< Uri-Host
    OT_COAP_OPTION_E_TAG          = 4,  ///< ETag
    OT_COAP_OPTION_IF_NONE_MATCH  = 5,  ///< If-None-Match
    OT_COAP_OPTION_OBSERVE        = 6,  ///< Observe [RFC7641]
    OT_COAP_OPTION_URI_PORT       = 7,  ///< Uri-Port
    OT_COAP_OPTION_LOCATION_PATH  = 8,  ///< Location-Path
    OT_COAP_OPTION_URI_PATH       = 11, ///< Uri-Path
    OT_COAP_OPTION_CONTENT_FORMAT = 12, ///< Content-Format
    OT_COAP_OPTION_MAX_AGE        = 14, ///< Max-Age
    OT_COAP_OPTION_URI_QUERY      = 15, ///< Uri-Query
    OT_COAP_OPTION_ACCEPT         = 17, ///< Accept
    OT_COAP_OPTION_LOCATION_QUERY = 20, ///< Location-Query
    OT_COAP_OPTION_BLOCK2         = 23, ///< Block2 (RFC7959)
    OT_COAP_OPTION_BLOCK1         = 27, ///< Block1 (RFC7959)
    OT_COAP_OPTION_SIZE2          = 28, ///< Size2 (RFC7959)
    OT_COAP_OPTION_PROXY_URI      = 35, ///< Proxy-Uri
    OT_COAP_OPTION_PROXY_SCHEME   = 39, ///< Proxy-Scheme
    OT_COAP_OPTION_SIZE1          = 60, ///< Size1
} otCoapOptionType;
typedef enum otError
{
    OT_ERROR_GENERIC = 255,
}otError;

typedef void (*otCoapRequestHandler)();
typedef void (*otCoapResponseHandler)(void                *aContext,
                                      otMessage           *aMessage,
                                      const otMessageInfo *aMessageInfo,
                                      otError              aResult);
typedef struct otCoapResource
{
    const char            *mUriPath; ///< The URI Path string
    otCoapRequestHandler   mHandler; ///< The callback for handling a received request
    void                  *mContext; ///< Application-specific context
    struct otCoapResource *mNext;    ///< The next CoAP resource in the list
} otCoapResource;

typedef struct otCoapOption
{
    uint16_t mNumber; ///< Option Number
    uint16_t mLength; ///< Option Length
} otCoapOption;
typedef struct otCoapOptionIterator
{
    const otMessage *mMessage;          ///< CoAP message
    otCoapOption     mOption;           ///< CoAP message option
    uint16_t         mNextOptionOffset; ///< Byte offset of next option
} otCoapOptionIterator;


DECLARE_FAKE_VALUE_FUNC2(otError, otCoapOptionIteratorInit, otCoapOptionIterator *, const otMessage *);
DECLARE_FAKE_VALUE_FUNC2(const otCoapOption *, otCoapOptionIteratorGetFirstOptionMatching, otCoapOptionIterator *, uint16_t );
DECLARE_FAKE_VALUE_FUNC2(otError, otCoapOptionIteratorGetOptionValue, otCoapOptionIterator *, void *);
DECLARE_FAKE_VALUE_FUNC1(const uint8_t *, otCoapMessageGetToken, const otMessage *);
// end of coap.h

DECLARE_FAKE_VALUE_FUNC(const char *, otapp_coap_getUriNameFromDefault, otapp_coap_uriIndex_t);
DECLARE_FAKE_VOID_FUNC3(otapp_coapSendGetUri_Well_known, const otIp6Address *, otCoapResponseHandler, void *);
DECLARE_FAKE_VOID_FUNC3(otapp_coapSendPutUri_subscribed_uris, const otIp6Address *,  const uint8_t *, uint16_t);
DECLARE_FAKE_VOID_FUNC3(otapp_coapSendSubscribeRequest, const otIp6Address *,  const char *, uint8_t *);
DECLARE_FAKE_VOID_FUNC3(otapp_coapSendSubscribeRequestUpdate, const otIp6Address *,  const char *, uint8_t *);
// DECLARE_FAKE_VALUE_FUNC3(int8_t, otapp_coapReadPayload, otMessage *, uint8_t *, uint16_t);


void otapp_ip6AddressPrint(const otIp6Address *aAddress);

#endif  /* MOCK_OT_APP_COAP_H_ */
