/**
 * @file ad_btn_dimControl.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief 
 * @version 0.1
 * @date 06-11-2025
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

#include "ad_btn_dimControl.h"

#define OT_BTN_DIR_UP   1
#define OT_BTN_DIR_DOWN 0


//////////////////////
// do not edit below
typedef struct {
    const uint8_t *thresh;
    const uint8_t *steps;
    uint8_t dir : 1;
}ad_btn_dim_t;

static ad_btn_dim_t dim = {
    .thresh = threshTab,
    .steps  = stepsTab,
    .dir = OT_BTN_DIR_UP, 
};

static uint8_t ad_btn_dim_getStep(uint8_t brightness) 
{
    for (int i = 0; i < OT_BTN_LEVELS_COUNT; i++) 
    {
        if (brightness <= dim.thresh[i]) 
        {
            return dim.steps[i];
        }
    }
    return 1;  // default step
}

uint8_t ad_btn_dim_GetNewValue(uint32_t dimLevel) 
{    
    uint8_t step = ad_btn_dim_getStep(dimLevel);
     
    if(dim.dir == OT_BTN_DIR_UP)
    {
        if (dimLevel + step > OT_BTN_MAX_BRIGHTNESS)
        {
            dimLevel = OT_BTN_MAX_BRIGHTNESS;
            dim.dir = OT_BTN_DIR_DOWN;
        }
        else
        {
            dimLevel += step;
        }        
    }
    else // dir down
    {
        if (dimLevel < OT_BTN_MIN_BRIGHTNESS)
        {
            dimLevel = 0;
            dim.dir = OT_BTN_DIR_UP;
        }
        else
        {
            dimLevel -= step;
        }        
    }
   
    return dimLevel;
}