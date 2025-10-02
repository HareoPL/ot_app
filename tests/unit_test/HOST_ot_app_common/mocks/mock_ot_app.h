#ifndef MOCK_OT_APP_H_
#define MOCK_OT_APP_H_

#include "mock_ot_app_coap.h" 
#include <stdio.h>
#include <string.h>


#define OTAPP_PAIRED_DEVICES_MAX    10  // max number of devices to save them from DNS query
#define OTAPP_PAIRED_URI_MAX        5   // max number of uris to save 
#define OTAPP_URI_MAX_NAME_LENGHT   32  // max lenght of uri string name

#define OTAPP_DNS_SRV_NAME_SIZE     64 // OT_DNS_MAX_NAME_SIZE full service name: "_coap._udp.default.service.arpa." 
#define OTAPP_DNS_SRV_LABEL_SIZE    32 // OT_DNS_MAX_LABEL_SIZE host name: "device1"
#define OTAPP_DNS_SRV_TXT_SIZE      512
#define OTAPP_DEVICE_NAME_FULL_SIZE        OTAPP_DNS_SRV_LABEL_SIZE


#define OTAPP_PAIR_DEVICES_MAX    OTAPP_PAIRED_DEVICES_MAX // max number of devices to save them from DNS query
#define OTAPP_PAIR_URI_MAX        OTAPP_PAIRED_URI_MAX 
#define OTAPP_PAIR_NAME_SIZE      OTAPP_DEVICE_NAME_FULL_SIZE 
#define OTAPP_PAIR_URI_INIT       OTAPP_URI_END_OF_INDEX

typedef enum {
    OTAPP_NO_DEVICE_TYPE = 0,
    
    OTAPP_CONTROL_PANEL = 1 ,
    OTAPP_SWITCH,

    OTAPP_LIGHTING,
    OTAPP_LIGHTING_ON_OFF,
    OTAPP_LIGHTING_DIMM,
    OTAPP_LIGHTING_RGB,
    
    OTAPP_THERMOSTAT,
    OTAPP_THERMOSTAT_SET_TEMP,
    OTAPP_THERMOSTAT_READ_SET_TEMP,
    OTAPP_THERMOSTAT_READ_CURRENT_TEMP,
    
    OTAPP_SENSOR,               // only out data
    OTAPP_DOOR_LOCK,            // only out data
    OTAPP_MOTION_DETECTOR,      // only out data
    OTAPP_REMOTE_CONTROL,
    OTAPP_ENERGY_METER,
    OTAPP_SMART_PLUG,
    OTAPP_ENVIRONMENT_SENSOR,
    OTAPP_DOOR_SENSOR,
    OTAPP_ALARM,

    OTAPP_END_OF_DEVICE_TYPE
}otapp_deviceType_t;

typedef struct ot_app_devDrv_t ot_app_devDrv_t; // forward declaration

#endif  /* MOCK_OT_APP_H_ */
