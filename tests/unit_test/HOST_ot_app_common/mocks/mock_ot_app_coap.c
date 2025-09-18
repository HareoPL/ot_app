
#include "mock_ot_app_coap.h"
#include "mock_ot_app.h"

DEFINE_FFF_GLOBALS;  




DEFINE_FAKE_VALUE_FUNC(const char *, otapp_coap_getUriNameFromDefault, otapp_coap_uriIndex_t);
DEFINE_FAKE_VOID_FUNC3(otapp_coapSendGetUri_Well_known, const otIp6Address *, otCoapResponseHandler, void *);

void otapp_ip6AddressPrint(const otIp6Address *aAddress)
{
   UNUSED(aAddress);
}
