/**
 * @file ad_btn_assign.h * 
 * @brief Handles physical button interactions and maps them to application events.
 * @details see more information in section: @ref device_btn_assign 
 * 
 * @defgroup device_btn_assign Button Assignment
 * @ingroup device_button
 * @brief Handles physical button interactions and maps them to application events.
 * @details
 * @{
 * This module is responsible for the low-level handling of physical buttons using the OneButton library.
 * It manages GPIO pin configuration, pull-up resistor setup, and callback registration for various 
 * interaction types (click, double click, long press).
 * It decouples the physical button layer from the CoAP logic by passing events to higher layers 
 * via standardized click types.
 * @version 0.1
 * @date 28-10-2025
 */
#ifndef AD_BTN_ASSIGN_H_
#define AD_BTN_ASSIGN_H_

#include "ot_app_drv.h"

#define OT_BTN_OB_ONE_CLICK     1
#define OT_BTN_OB_LONG_PRESS    2

void ad_btn_task(void);
int8_t ad_btn_init(ot_app_devDrv_t *drvPtr);
int8_t ad_btn_assignDevice(otapp_pair_Device_t *newDevice);

#endif  /* AD_BTN_ASSIGN_H_ */

/**
 * @}
 */