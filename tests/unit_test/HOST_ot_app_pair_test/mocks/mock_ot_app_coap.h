#ifndef MOCK_OT_APP_COAP_H_
#define MOCK_OT_APP_COAP_H_

#include "mock_ip6.h"
#include "hro_utils.h"
#include <string.h>

typedef enum {
    OTAPP_URI_NO_URI_INDEX = 0,
    
    OTAPP_URI_WELL_KNOWN_CORE,
    OTAPP_URI_TEST,
    OTAPP_URI_DEVICE_TEST,
    OTAPP_URI_PARING_SERVICES,

    OTAPP_URI_END_OF_INDEX,
}otapp_coap_uriTableIndex_t;

const char *otapp_coap_getUriName(otapp_coap_uriTableIndex_t uriIndex);
void otapp_printIp6Address(const otIp6Address *aAddress);

#endif  /* MOCK_OT_APP_COAP_H_ */
