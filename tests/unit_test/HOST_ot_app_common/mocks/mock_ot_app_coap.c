
#include "mock_ot_app_coap.h"
#include "mock_ot_app.h"

DEFINE_FFF_GLOBALS;  




DEFINE_FAKE_VALUE_FUNC(const char *, otapp_coap_getUriNameFromDefault, otapp_coap_uriIndex_t);
DEFINE_FAKE_VOID_FUNC3(otapp_coapSendGetUri_Well_known, const otIp6Address *, otCoapResponseHandler, void *);
DEFINE_FAKE_VOID_FUNC3(otapp_coapSendPutUri_subscribed_uris, const otIp6Address *,  const uint8_t *, uint16_t);
DEFINE_FAKE_VOID_FUNC3(otapp_coapSendGetSubscribeRequest, const otIp6Address *,  const char *, uint8_t *);

void otapp_ip6AddressPrint(const otIp6Address *aAddress)
{
   UNUSED(aAddress);
}
