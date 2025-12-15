
#include "mock_ot_app_coap.h"
#include "mock_ot_app.h"

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


static otCoapResource otapp_coap_resource[] = {
    {".well-known/core", NULL, NULL, NULL},
    {"test", NULL, NULL, NULL},
    {"device/led", NULL, NULL, NULL}, 
    {"paring_services", NULL, NULL, NULL}

};
#define OTAPP_COAP_RESOURCE_SIZE (sizeof(otapp_coap_resource) / sizeof(otapp_coap_resource[0]))


const char *otapp_coap_getUriName(otapp_coap_uriTableIndex_t uriIndex)
{
    return otapp_coap_resource[uriIndex - 1].mUriPath;
} 

void otapp_printIp6Address(const otIp6Address *aAddress)
{
   UNUSED(aAddress);
}
