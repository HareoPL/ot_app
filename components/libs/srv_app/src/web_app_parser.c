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
#include "ws2812b_fx.h"


static const char *TAG = "web_app_parser";
static wbp_parser_t *wbp_parser = NULL;

void wbp_parseData(void)
{
    if (NULL != wbp_parser && wbp_parser->semaphoreHandle)
    {
        xSemaphoreGive(wbp_parser->semaphoreHandle);
    }
}

static wbp_state_t wbp_getNestedValue(wbp_parser_t *parser, const char *key1, const char *key2, const char *key3, void *out, uint8_t numOfDataInByte) 
{
    if((parser->root == NULL) || (out == NULL) || (NULL == key1))
    { 
        return WBP_ERROR;
    }

    if(!(numOfDataInByte == 1 || numOfDataInByte == 2 || numOfDataInByte == 4))
    { 
        return WBP_ERROR;
    }

    parser->obj = cJSON_GetObjectItem(parser->root, key1);
    
    if(NULL != key2)
    {
        parser->obj = cJSON_GetObjectItem(parser->obj, key2);
        if(NULL ==  parser->obj)
        {
            return WBP_ERROR;
        }
    } 

    if(NULL != key3)
    {
        parser->obj = cJSON_GetObjectItem(parser->obj, key3);
        if(NULL ==  parser->obj)
        {
            return WBP_ERROR;
        }
    } 
        if(!cJSON_IsNumber(parser->obj)) 
        {
            return WBP_ERROR;
        }

        switch (numOfDataInByte)
        {
        case 1:
            *(uint8_t*)out = parser->obj->valueint;
            break;
        case 2:
            *(uint16_t*)out = parser->obj->valueint;
            break;
        case 4:
            *(uint32_t*)out = parser->obj->valueint;
            break;   
        }
       
    return WBP_OK;
}

static void wap_setWS2812(wbp_parser_t *parser)
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t modeFX = 0;
    uint16_t speed = 0;
    wbp_state_t state = 0;

    state += wbp_getNestedValue(parser, "color", "rgb", "r", (uint8_t*)&r, 1);
    state += wbp_getNestedValue(parser, "color", "rgb", "g", (uint8_t*)&g, 1);
    state += wbp_getNestedValue(parser, "color", "rgb", "b", (uint8_t*)&b, 1);

    state += wbp_getNestedValue(parser, "speed", 0, 0, (uint16_t*)&speed, 2);

    state += wbp_getNestedValue(parser, "modeFX", 0, 0, (uint8_t*)&modeFX, 1);
    
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

void wap_task(void *parseBuffer)
{
    while (1) 
    {
        if(xSemaphoreTake(wbp_parser->semaphoreHandle, portMAX_DELAY) == pdTRUE) 
        {
            // parse JSON
            wbp_parser->root = cJSON_Parse((char*)parseBuffer);

            if (wbp_parser->root == NULL) 
            {
                ESP_LOGE(TAG, "parse error JSON");
            }
            else
            {
                #ifdef DEBUG
                    char *json_str = cJSON_Print(wbp_parser->root);
                    if(NULL != json_str)
                    {
                        ESP_LOGI(TAG, "receive JSON:\n%s\n", json_str);
                        free(json_str);
                    }
                #endif

                wap_setWS2812(wbp_parser);
                
                UTILS_RTOS_CHECK_FREE_STACK();

                // memory free
                cJSON_Delete(wbp_parser->root);
            }
        }
    }
}


static wbp_parser_t *wbp_create(void)
{
    /* Allocate memory for wbp_parser_t */
    wbp_parser_t *wbp = calloc(1, sizeof(wbp_parser_t));
    if (NULL == wbp) 
    {
        ESP_LOGE(TAG, "Failed to allocate memory for wbp_parser_t");
        return NULL;
    }

   return wbp;
}

static void wbp_delete(wbp_parser_t *wbp)
{
    if(wbp->semaphoreHandle)
    {
        vSemaphoreDelete(wbp->semaphoreHandle);
    } 
    if(wbp->taskHandle)
    {
        vTaskDelete(wbp->taskHandle);
    }
    free(wbp);
}

wbp_state_t wbp_initParser(const wbp_parserConfig_t *config)
{
    if((NULL == config) || (NULL == config->buffer))
    {
        return WBP_ERROR;
    }

    if(NULL != wbp_parser)
    {
        wbp_delete(wbp_parser);
        wbp_parser = NULL;
    }

    wbp_parser = wbp_create();
    if(NULL == wbp_parser)
    {
        return WBP_ERROR_CALLOC;
    }

    wbp_parser->semaphoreHandle = xSemaphoreCreateBinary();
    if(NULL == wbp_parser->semaphoreHandle)
    {
        wbp_delete(wbp_parser);
        wbp_parser = NULL;

        return WBP_ERROR;
    }

    if(pdPASS != xTaskCreate(wap_task, "parserTask", config->taskStackSize, (char*)config->buffer, config->taskPriority, &wbp_parser->taskHandle))
    {
        wbp_delete(wbp_parser);
        wbp_parser = NULL;

        return WBP_ERROR;
    }

    return WBP_OK;
}