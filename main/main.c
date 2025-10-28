/**
 * @file main.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 14-04-2025
 * 
 * @copyright The MIT License (MIT) Copyright (c) 2025 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * 
 */

#include "main.h"
#include "ws2812b_fx.h"
#include "ws2812b_drv_RMT.h"
#include "xtimers.h"
#include "spiffs.h"
#include "wifi.h"
#include "web_app.h"
#include "ot_app.h"
#include "OneButton.h"

#include "ad_temp.h"

static const char *TAG = "main";

#define BUTTON_GPIO GPIO_NUM_4
OneButton_t btn_coapSendGetTest;

// only for test coap get and put sent
void otapp_gpioInit()
{ 
    
    gpio_reset_pin(BUTTON_GPIO); // reset to default state
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT); //set gpio as input

    // set pull-up
    gpio_pullup_en(BUTTON_GPIO);

    OneButtonInit(&btn_coapSendGetTest, GPIO_NUM_4);
    OneButtonCallbackOneClick(&btn_coapSendGetTest, otapp_coapSendtoTestGet);
    OneButtonCallbackDoubleClick(&btn_coapSendGetTest, otapp_coapSendtoTestPut);
    // OneButtonCallbackLongPressStart(&BlueKey,ToggleLed); // the function will repeat itself until you release the button
    // OneButtonCallbackLongPressStop(&BlueKey,TurnOffLed);

}

void app_main(void)
{
    ESP_UNUSED(TAG);

    otapp_gpioInit(); // only for test coap get sent
    ad_tempInit();
    otapp_init();
    
    // wifi_initSTA();
    // wifi_initAP();
    // web_app_startWebServer();
    
  
    xTim_Init();                                // init freeRTOS soft timers
    // xTim_printTaskListEnable();                 // enable freeRTOS task list

    ws2812b_if_init();                          // init interface - RMT
    WS2812BFX_Init(ws2812b_if_getDrvRMT(), 1);  // init ws leds
    
    WS2812BFX_SetSpeed(0, 100);	                // Speed of segment 0
    WS2812BFX_SetColorRGB(0, 0, 0, 5);	        // Set color 0
    WS2812BFX_SetMode(0, FX_MODE_COLOR_WIPE);	// Set mode segment 0
    WS2812BFX_Start(0);	                        // Start segment 0

       
    
    while (1) 
    {
        WS2812BFX_Callback();	                // FX effects calllback        

        ot_app_drv_task();

        // UTILS_RTOS_CHECK_FREE_STACK();
        vTaskDelay(pdMS_TO_TICKS(1));           // this has to be here for refresch watchdog
    }
}

/****************************************************
 *  freeRtos hooks
 */
// void vApplicationTickHook(void) // calling from IRQ
// {
    
// }

void vApplicationIdleHook(void) // the lowest freeRTOS priority
{

}
