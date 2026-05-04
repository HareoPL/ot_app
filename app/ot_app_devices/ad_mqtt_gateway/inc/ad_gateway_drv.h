#ifndef AD_GATEWAY_DRV_H_
#define AD_GATEWAY_DRV_H_

#include "stdint.h"

typedef enum {
    AD_GW_DRV_CONNECTED = 0,
    AD_GW_DRV_DISCONNECTED,
    AD_GW_DRV_GOT_IP,
    AD_GW_DRV_LOST_IP,

    AD_GW_DRV_MQTT_CONNECTED,
    AD_GW_DRV_MQTT_DISCONNECTED,
    AD_GW_DRV_MQTT_SUBSCRIBED,
    AD_GW_DRV_MQTT_UNSUBSCRIBED,
    AD_GW_DRV_MQTT_PUBLISHED,
    AD_GW_DRV_MQTT_DATA,    
    AD_GW_DRV_MQTT_ERROR,

    AD_GW_DRV_END_OF_EVENT
} ad_gw_drv_event_t;

typedef void (*ad_gw_drv_event_cb_t)(ad_gw_drv_event_t event, void *context);

typedef struct {
    int8_t (*init)(void);
    // int8_t (*init)(const char* APN, const char* mqttBrokerIpAddress);
    
    struct {
    int8_t (*connect)(void);
    int8_t (*connectCheck)(void);
    int8_t (*disconnect)(void);
    }modem;
    
    struct {
        // start
        // stop

        int8_t (*publish)(const char *topic, const uint8_t *payload, uint16_t len, uint8_t qos, uint8_t retain);
        int8_t (*subscribeSingle)(const char *topic, uint8_t qos);
        int8_t (*unSubscribeSingle)(const char *topic);   
    }mqtt;
}ad_gw_drv_t;

void ad_gw_drv_register_event_cb(ad_gw_drv_event_cb_t callback);

#endif  /* AD_GATEWAY_DRV_H_ */
