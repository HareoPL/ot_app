/**
 * @file web_app_parser.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 25-04-2025
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
#include "web_app_parser.h"

#include "web_app.h"
#include "cJSON.h"
#include "ws2812b_fx.h"

#define WBPARSER_TASK_STACK_SIZE (1024 * 3) 
#define WBPARSER_TASK_PRIORITY   (1)

static const char *TAG = "web_app_parser";

static char *wap_buffer;

static cJSON *wap_root = NULL;
static cJSON *wap_obj = NULL;

TaskHandle_t taskHandle_wbParser = NULL;
SemaphoreHandle_t semaphore_wbParser = NULL;

void wbp_parseData(void)
{
    xSemaphoreGive(semaphore_wbParser);
}

static wbp_state_t wbp_getNestedValue(const cJSON *root, const char *key1, const char *key2, const char *key3, void *out, uint8_t numOfDataInByte) 
{
    wbp_state_t state = 0;

    if((root == NULL) && (out == NULL))
    { 
        return WBP_ERROR;
    }
    if((numOfDataInByte < 1) && (numOfDataInByte > 4) && (numOfDataInByte == 3))
    { 
        return WBP_ERROR;
    }

    wap_obj = cJSON_GetObjectItem(root, key1);
    
    if(key2)
    {
        wap_obj = cJSON_GetObjectItem(wap_obj, key2);
    } 
    if(key3)
    {
        wap_obj = cJSON_GetObjectItem(wap_obj, key3);
    } 

    if(wap_obj != NULL)
    {
        switch (numOfDataInByte)
        {
        case 1:
            *(uint8_t*)out = wap_obj->valueint;
            break;
        case 2:
            *(uint16_t*)out = wap_obj->valueint;
            break;
        case 4:
            *(uint32_t*)out = wap_obj->valueint;
            break;   
        }
       
        state = WBP_OK;
    }
    else
    {
        state = WBP_ERROR; 
    }

    return state;
}

static void wap_setWS2812(const cJSON *root)
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t modeFX = 0;
    uint16_t speed = 0;
    wbp_state_t state = 0;

    state += wbp_getNestedValue(root, "color", "rgb", "r", (uint8_t*)&r, 1);
    state += wbp_getNestedValue(root, "color", "rgb", "g", (uint8_t*)&g, 1);
    state += wbp_getNestedValue(root, "color", "rgb", "b", (uint8_t*)&b, 1);

    state += wbp_getNestedValue(root, "speed", 0, 0, (uint16_t*)&speed, 2);

    state += wbp_getNestedValue(root, "modeFX", 0, 0, (uint8_t*)&modeFX, 1);
    
    if(state == WBP_OK)
    {
        WS2812BFX_SetColorRGB(0, r, g, b);
        WS2812BFX_SetColorRGB(0, r, g, b);
        WS2812BFX_SetColorRGB(0, r, g, b);
        WS2812BFX_SetSpeed(0, speed);	   
        WS2812BFX_SetMode(0, modeFX);
    }
    else
    {
        ESP_LOGE(TAG, "incorrect WS2812B data in JSON");
    }
}

void wap_task(void *pvParameter)
{
    
    while (1) 
    {
        if(xSemaphoreTake(semaphore_wbParser, portMAX_DELAY) == pdTRUE) 
        {
            // parse JSON
            wap_root = cJSON_Parse(wap_buffer);

            if (wap_root == NULL) 
            {
                ESP_LOGE(TAG, "parse error JSON");
            }
            else
            {
                ESP_LOGI(TAG, "receive JSON:\n%s\n", cJSON_Print(wap_root));

                wap_setWS2812(wap_root);
                
                UTILS_RTOS_CHECK_FREE_STACK();

                // memory free
                cJSON_Delete(wap_root);
            }
        }
    }
}

void wbp_initParser(void)
{
    semaphore_wbParser = xSemaphoreCreateBinary();

    xTaskCreate(wap_task, "parserTask", WBPARSER_TASK_STACK_SIZE, NULL, WBPARSER_TASK_PRIORITY, &taskHandle_wbParser);

    wap_buffer = web_app_getBuffer();

}