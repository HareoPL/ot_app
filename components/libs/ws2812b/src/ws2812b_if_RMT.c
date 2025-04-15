/**
 * @file ws2812b_if_RMT.c
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
#include "ws2812b_if_RMT.h"
#include "driver/rmt_tx.h"
// for test
static uint8_t ws2812b_if_state;
static uint8_t ws2812b_if_ledNumber;

static ws2812b_color wsLedBuffer[WS2812B_IF_LEDS];

static rmt_channel_handle_t tx_chan = NULL;
static rmt_encoder_handle_t encoder  = NULL;
static rmt_transmit_config_t transmit_cfg = {.loop_count = 0, .flags.eot_level = 0};

uint8_t* ws2812b_if_getPixels(void);
void ws2812b_if_refresh(void);
uint32_t ws2812b_if_getColor(int16_t DiodeID);
void ws2812b_if_setOneDiodeRGB(int16_t DiodeID, uint8_t R, uint8_t G, uint8_t B);
void ws2812b_if_setOneDiode(int16_t DiodeID, uint32_t color);
uint16_t ws2812b_if_getNumberOfLeds(void);
void ws2812b_if_setDiodeColorStruct(int16_t DiodeID, ws2812b_color colorStruct);

// ws2812b_drv_t ws2812b_if_RMT_Drv = {

// };

void ws2812b_if_setDiodeColorStruct(int16_t DiodeID, ws2812b_color colorStruct)
{
    wsLedBuffer[DiodeID] = colorStruct;
}

uint16_t ws2812b_if_getNumberOfLeds(void)
{
    return WS2812B_IF_LEDS;
}

void ws2812b_if_setOneDiode(int16_t DiodeID, uint32_t color)
{
    if(DiodeID >= WS2812B_IF_LEDS || DiodeID < 0) return; // Escape if we try to set diode out of chain.

    wsLedBuffer[DiodeID].red = ((color >> 16) & 0x000000FF);
	wsLedBuffer[DiodeID].green = ((color >> 8) & 0x000000FF);
	wsLedBuffer[DiodeID].blue = (color & 0x000000FF);
}

void ws2812b_if_setOneDiodeRGB(int16_t DiodeID, uint8_t R, uint8_t G, uint8_t B)
{
	uint32_t color = 0;

	color |= (R << 16);
	color |= (G << 8);
	color |= (B);

	ws2812b_if_setOneDiode(DiodeID, color);
}

uint32_t ws2812b_if_getColor(int16_t DiodeID)
{
	uint32_t color = 0;
	
	color |= (wsLedBuffer[DiodeID].red << 16);
	color |= (wsLedBuffer[DiodeID].green << 8);
	color |= (wsLedBuffer[DiodeID].blue);
	return color;
}
uint8_t* ws2812b_if_getPixels(void)
{
    return (uint8_t*)wsLedBuffer;
}

void ws2812b_if_refresh(void)
{
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, encoder, wsLedBuffer, sizeof(wsLedBuffer), &transmit_cfg));
    // ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_chan, portMAX_DELAY));
    
}

void ws2812b_if_setAllDiodes(uint32_t color)
{
	for(int8_t i = 0; i < WS2812B_IF_LEDS; i++)
    {
        ws2812b_if_setOneDiode(i, color);
    }
}

void ws2812b_if_simpleTest(void)
{
	static uint32_t color;
   switch (ws2812b_if_state)
   {
   case 0:
	  	color = WS2812B_IF_RED; 
	   break;
   case 1:
   		color = WS2812B_IF_GREEN; 
	   break;
   case 2:
   		color = WS2812B_IF_BLUE; 
	   break;
   
   default:
	   break;
   }

   ws2812b_if_setAllDiodes(WS2812B_IF_BLACK); 
   ws2812b_if_setOneDiode(ws2812b_if_ledNumber, color);
   ws2812b_if_refresh();

   ws2812b_if_ledNumber++;

   if(ws2812b_if_ledNumber >= WS2812B_IF_LEDS)
   {
	   ws2812b_if_ledNumber = 0;
	   ws2812b_if_state++;
   }

   if(ws2812b_if_state >= 3)
   {
	   ws2812b_if_state = 0;
   }
}

void ws2812b_if_init(void)
{
    rmt_bytes_encoder_config_t bytes_encoder_cfg = 
    {
        .bit0 = {.duration0 = RMT_TIME_03_US, .level0 = 1, .duration1 = RMT_TIME_09_US, .level1 = 0}, // 0.3µs HIGH + 0.9µs LOW
        .bit1 = {.duration0 = RMT_TIME_09_US, .level0 = 1, .duration1 = RMT_TIME_03_US, .level1 = 0}, // 0.9µs HIGH + 0.3µs LOW
    };
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_cfg, &encoder));

    rmt_tx_channel_config_t tx_chan_config = 
    {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = RMT_LED_STRIP_GPIO_NUM,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));

    ESP_ERROR_CHECK(rmt_enable(tx_chan));


    for (int led = 0; led < WS2812B_IF_LEDS; led++) 
    {
        wsLedBuffer[0].blue = 0;
        wsLedBuffer[0].green = 0;
        wsLedBuffer[0].red = 5;
    }

}


// color correction
static const uint8_t _sineTable[256] = {
	128,131,134,137,140,143,146,149,152,155,158,162,165,167,170,173,
	176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215,
	218,220,222,224,226,228,230,232,234,235,237,238,240,241,243,244,
	245,246,248,249,250,250,251,252,253,253,254,254,254,255,255,255,
	255,255,255,255,254,254,254,253,253,252,251,250,250,249,248,246,
	245,244,243,241,240,238,237,235,234,232,230,228,226,224,222,220,
	218,215,213,211,208,206,203,201,198,196,193,190,188,185,182,179,
	176,173,170,167,165,162,158,155,152,149,146,143,140,137,134,131,
	128,124,121,118,115,112,109,106,103,100, 97, 93, 90, 88, 85, 82,
	 79, 76, 73, 70, 67, 65, 62, 59, 57, 54, 52, 49, 47, 44, 42, 40,
	 37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 11,
	 10,  9,  7,  6,  5,  5,  4,  3,  2,  2,  1,  1,  1,  0,  0,  0,
	  0,  0,  0,  0,  1,  1,  1,  2,  2,  3,  4,  5,  5,  6,  7,  9,
	 10, 11, 12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35,
	 37, 40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70, 73, 76,
	 79, 82, 85, 88, 90, 93, 97,100,103,106,109,112,115,118,121,124};
  
  static const uint8_t _gammaTable[256] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
	  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
	  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
	 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
	 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
	 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
	 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
	 58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
	 76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
	 97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
	122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
	150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
	182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
	218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255};
  
  uint8_t WS2812B_sine8(uint8_t x)
  {
	  return _sineTable[x];
  }
  
  uint8_t WS2812B_gamma8(uint8_t x)
  {
	  return _gammaTable[x];
  }
  