/**
 * @file ws2812b_if.h
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief driver for ws2812b
 * @version 0.1
 * @date 15-04-2025
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
#ifndef WS2812B_IF_H_
#define WS2812B_IF_H_

#include "stdint.h"

typedef struct
{
	uint8_t green, red, blue;
} ws2812b_color;

typedef struct 
{   
    void (*Refresh)(void);
    uint8_t  (*sine8)(uint8_t x);
    uint8_t* (*GetPixels)(void);
    uint32_t (*GetColor)(int16_t DiodeID);
    uint16_t (*GetNumberOfLeds)(void);

    void (*SetOneDiode)(int16_t DiodeID, uint32_t color);
    void (*SetOneDiodeRGB)(int16_t DiodeID, uint8_t R, uint8_t G, uint8_t B);
    void (*SetDiodeColorStruct)(int16_t DiodeID, ws2812b_color colorStruct);
}ws2812b_drv_t;

#endif  /* WS2812B_IF_H_ */
