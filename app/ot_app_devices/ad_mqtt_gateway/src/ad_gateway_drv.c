/**
 * @file ad_gateway_drv.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 04-05-2026
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2026 
 */

#include "ad_gateway_drv.h"
#include "hro_utils.h"

#define TAG "ad_gateway_drv.c "

__attribute__((weak)) void ad_gw_drv_register_event_cb(ad_gw_drv_event_cb_t callback)
{
    // Print a highly visible error to the serial monitor
    HRO_PRINTF(TAG, "=======================================================\n");
    HRO_PRINTF(TAG, "=======================================================\n");
    HRO_PRINTF(TAG,"ARCHITECTURE ERROR: Default WEAK function called!\n");
    HRO_PRINTF(TAG,"Implement function: ad_gw_drv_register_event_cb()\n.");
    HRO_PRINTF(TAG,"=======================================================\n");
    HRO_PRINTF(TAG,"=======================================================\n");
}