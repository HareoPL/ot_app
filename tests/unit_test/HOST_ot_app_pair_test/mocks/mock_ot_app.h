#ifndef MOCK_OT_APP_H_
#define MOCK_OT_APP_H_

#include "mock_ot_app_coap.h" 
#include <stdio.h>
#include <string.h>


#define OTAPP_PAIRED_DEVICES_MAX    10 // max number of devices to save them from DNS query
#define OTAPP_PAIRED_URI_MAX        20 

#define OTAPP_DNS_SRV_NAME_SIZE     64 // OT_DNS_MAX_NAME_SIZE full service name: "_coap._udp.default.service.arpa." 
#define OTAPP_DNS_SRV_LABEL_SIZE    32 // OT_DNS_MAX_LABEL_SIZE host name: "device1"
#define OTAPP_DNS_SRV_TXT_SIZE      512
#define OTAPP_DEVICE_NAME_SIZE        OTAPP_DNS_SRV_LABEL_SIZE


#define OTAPP_PAIR_DEVICES_MAX    OTAPP_PAIRED_DEVICES_MAX // max number of devices to save them from DNS query
#define OTAPP_PAIR_URI_MAX        OTAPP_PAIRED_URI_MAX 
#define OTAPP_PAIR_NAME_SIZE      OTAPP_DEVICE_NAME_SIZE 
#define OTAPP_PAIR_NO_URI         0
#define OTAPP_PAIR_URI_INIT       OTAPP_URI_END_OF_INDEX



#endif  /* MOCK_OT_APP_H_ */
