/**
 * @file web_app_parser.h
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
#ifndef WEB_APP_PARSER_H_
#define WEB_APP_PARSER_H_

#include "cJSON.h"

#define WBPARSER_DEFAULT_CONFIG() \
{ \
    .taskStackSize     = (1024 * 3), \
    .taskPriority      = tskIDLE_PRIORITY+1,\
}
typedef enum
{
    WBP_OK = 0,
    WBP_ERROR,
    WBP_ERROR_CALLOC,

}wbp_state_t;

typedef struct
{
    cJSON *root;
    cJSON *obj;
    
    TaskHandle_t taskHandle;
    SemaphoreHandle_t semaphoreHandle;
}wbp_parser_t;

typedef struct 
{
    uint32_t taskStackSize;
    uint32_t taskPriority;
    char *buffer;
}wbp_parserConfig_t;

void wbp_parseData(void);
wbp_state_t wbp_initParser(const wbp_parserConfig_t *config);

#endif  /* WEB_APP_PARSER_H_ */
