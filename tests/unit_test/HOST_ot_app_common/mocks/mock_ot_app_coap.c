
#include "mock_ot_app_coap.h"
#include "mock_ot_app.h"

DEFINE_FFF_GLOBALS;  




DEFINE_FAKE_VALUE_FUNC(const char *, otapp_coap_getUriNameFromDefault, otapp_coap_uriIndex_t);


void otapp_ip6AddressPrint(const otIp6Address *aAddress)
{
   UNUSED(aAddress);
}
