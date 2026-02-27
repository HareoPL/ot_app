/**
 * @file ws2812b_drv_tim_dma_stm32.c
 * @author Jan Łukaszewicz (pldevluk@gmail.com)
 * @brief Implementation of driver using timer and DMA for ws2812b
 * 
 * @version 0.1
 * @date 03-04-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */
#include "main.h"
#include "stm32wba65xx.h"
#include "ws2812b_drv_tim_dma_stm32.h"

typedef struct ws2812b_color
{
	uint8_t red, green, blue;
} ws2812b_color_t;

typedef struct ws2812b_buffer
{
	uint8_t dma[WS2812B_DMA_BUFFER_LENGTH];
	ws2812b_color ledColor[WS2812B_LEDS];

}ws2812b_buffer_t;

static void WS2812B_InitHW(void);

static void WS2812B_SetDiodeColorStruct(int16_t DiodeID, ws2812b_color colorStruct);
static void WS2812B_SetOneDiode(int16_t DiodeID, uint32_t color);
static void WS2812B_SetOneDiodeRGB(int16_t DiodeID, uint8_t R, uint8_t G, uint8_t B);
static void WS2812B_SetAllDiodes(uint32_t color);

static uint32_t WS2812B_GetColor(int16_t DiodeID);
static uint8_t* WS2812B_GetPixels(void);
static uint16_t WS2812B_GetNumberOfLeds(void);

static void WS2812B_Refresh(void);
static uint8_t WS2812B_sine8(uint8_t x);
static void WS2812B_Transfer_Start(void);

const ws2812b_drv_t ws2812b_drv_stm32 = {
	WS2812B_Refresh,
    WS2812B_sine8,
    WS2812B_GetPixels,
    WS2812B_GetColor,
    WS2812B_GetNumberOfLeds,

    WS2812B_SetOneDiode,
    WS2812B_SetOneDiodeRGB,
    WS2812B_SetDiodeColorStruct,

	WS2812B_InitHW,
};

static ws2812b_buffer_t ledBuffer;
static uint16_t CurrentLed;
volatile static uint8_t ws2812b_TransferComplete = 1;

// for test
static uint8_t ws2812b_state;
static uint8_t ws2812b_led;

/**
 * @brief Alternate Function (AF) table for ports PA15 and PA2 (AF15-AF25).
 * 
 * Table 24. Alternate functions (AF0 to AF7).
 * | Port    | ... | AF2<br>LPTIM1/2/<br>TIM1/2/3 | ... | 
 * | :------ | :---| :--------------------------- | :---| 
 * | **PA2** | ... | TIM3_CH1                     | ... | 
 * Source: STM32 datasheet DS14736,
 * 
 * APB1 (TIM3) clock: 32MHz
 * ws2812b clock: 800kHz
 * ARR = APB1 CLK / WSCLOCK - 1 = 32 000 000 / 800 000 - 1 = 39 ( - 1 TIM count from 0)
 */

void WS2812B_Timer_Init(void)
{	
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN; // enable timer 3
	while(!(RCC->APB1ENR1 & RCC_APB1ENR1_TIM3EN)); // Wait for clock propatation
	
	// 800 Khz period
	TIM3->PSC = WS2812B_TIMER_PRESCALER; 	// Prescaler for input frequency
	TIM3->EGR |= TIM_EGR_UG;				// Re-initialize the counter and generates an update of the registers.
	TIM3->ARR = WS2812B_TIME_PERIOD; 		// Auto-reload Register - Value to count in Timer

	// TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S);					// 00: CC1 channel is configured as output. When is as output this register is [alternate]. Reset value: 0x0000 0000 
	TIM3->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); 	// PWM Mode 1 on output
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE; 						// Enable pre-load for CC1

	TIM3->CCER |= TIM_CCER_CC1E; 							// Capture mode enabled / OC1 signal is output on the corresponding output pin
}

void WS2812B_PwmOutput_Init(void)
{
	// TIM3 CH1 on PA2 AHB2

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	while(!(RCC->AHB2ENR & RCC_AHB2ENR_GPIOAEN));

	GPIOA->MODER &= ~(GPIO_MODER_MODE2); 	// clear 
	GPIOA->MODER |= GPIO_MODER_MODE2_1; 	// set: 10: Alternate function mode
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2); 	// clear AF2
	GPIOA->AFR[0] |= (2 << GPIO_AFRL_AFSEL2_Pos); 	// set: AF2 for TIM3_CH1 on PA2
}

// GPDMA1_Channel2
void WS2812B_DMA_Init(void)
{
	// GPDMA_CxCR Reset value: 0x0000 0000
	uint32_t RegValue = 0;
	// DMA1 Channel 2 for TIM3 CH1 PWM
	RCC->AHB1ENR |= RCC_AHB1ENR_GPDMA1EN;
	while(!(RCC->AHB1ENR & RCC_AHB1ENR_GPDMA1EN));
	
	// set prioryty and TCIE irq
	// GPDMA_CxCR Reset value: 0x0000 0000, 
	RegValue |= DMA_CCR_PRIO_1;  // set DMA Priority Level Very High
	RegValue |= DMA_CCR_TCIE; 	 // transfer complete interrupt enable 
	GPDMA1_Channel2->CCR = RegValue;

	// data size and incremental
	// GPDMA_CxTR1 Reset value: 0x0000 0000, 
	RegValue = 0; 
	// RegValue &= ~(DMA_CTR1_SDW_LOG2);// 8-bit Memory Size. 00: byte. binary logarithm of the source data width of a burst in bytes. LEGACY: MSIZE
	RegValue |= (DMA_CTR1_DDW_LOG2_1); 	// 32-bit periperal size. 10: word (4 bytes). binary logarithm of the destination data width of a burst, in bytes. LEGACY: PSIZE
	RegValue |= DMA_CTR1_SINC; 			// Memmory incrementation. 1: contiguously incremented burst. source incrementing burst. LEACY: MINC
	// RegValue &= ~(DMA_CTR1_DINC);	// Peripheral incrementation. 0: fixed burst. destination incrementing burst. LEGACY: PINC
	// RegValue &= ~(DMA_CTR1_PAM); 	// Padding, right aligned. 8 bits(source, mem) to 32bits (destination, tim). 00: source data is transferred as right aligned, padded with 0s up to the destination data width 
	GPDMA1_Channel2->CTR1 = RegValue;

	// Request settings
	// GPDMA_CxTR2 Reset value: 0x0000 0000, 
	RegValue = 0;
 	RegValue |= (31 << DMA_CTR2_REQSEL_Pos); 	// set 31 for tim3_cc1_dma. GPDMA hardware request selection
 	RegValue |= DMA_CTR2_DREQ;					// 1: selected hardware request driven by a destination peripheral (request signal taken into account by the GPDMA transfer scheduler over the destination/write port)
												// DREQ = 1 (direction: RAM (source) -> Timer (destination)). LEGACY DIR
												// 0: selected hardware request driven by a source peripheral (request signal taken into account by the GPDMA transfer scheduler over the source/read port)
												// DREQ = 0 (direction: Timer (destination) -> RAM (source)). LEGACY DIR
												
 	// RegValue &= ~(DMA_CTR2_SWREQ);			// 0: no software request. software request
												// 1: software request for a memory-to-memory transfer. SWREQ = 1 (DIRECTION: RAM -> RAM)
 	// RegValue &= ~(DMA_CTR2_BREQ);			// 0: the selected hardware request is driven by a peripheral with a hardware request/acknowledge protocol at a burst level. Block hardware request
	GPDMA1_Channel2->CTR2 = RegValue;

	// single Mode
	// GPDMA_CxLLR Reset value: 0x0000 0000
	// GPDMA1_Channel2->CLLR = 0; // single mode 
	
	
	#if (WS2812B_LOW_RAM_ENABLE == 1)
	GPDMA1_Channel2->CCR |= DMA_CCR_HTIE; // half-transfer complete interrupt enable
	#endif
	
	// set NVIC
	NVIC_SetPriority(GPDMA1_Channel2_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 0, 0)); // Higher Priority = better
	NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);
	// GPDMA1_Channel2_IRQHandler stm32wbaxx_it.c
}

void WS2812B_DMA_Start(void)
{
	// GPDMA_CxFCR
	GPDMA1_Channel2->CFCR = (0x7F << DMA_CFCR_TCF_Pos); // Clear All DMA Interrupts

	// source address
	// GPDMA_CxSAR
	GPDMA1_Channel2->CSAR = (uint32_t)&ledBuffer.dma; // Memory address. this field is kept fixed or incremented by the data width

	// destination address
	// GPDMA_CxDAR
	GPDMA1_Channel2->CDAR = (uint32_t)&(TIM3->CCR1); // peripheral address 

	// GPDMA_CxBR1
	GPDMA1_Channel2->CBR1 = 0; 			// CLEAR
	GPDMA1_Channel2->CBR1 |= WS2812B_DMA_BUFFER_LENGTH; // SET  Number of data to transfer. LEGACY CNDTR

	GPDMA1_Channel2->CCR |= DMA_CCR_EN; // Enable DMA channel
}

void WS2812B_DMA_Stop(void)
{
	GPDMA1_Channel2->CCR &= ~(DMA_CCR_EN); // Disable DMA channel

	// GPDMA_CxFCR
	GPDMA1_Channel2->CFCR = (0x7F << DMA_CFCR_TCF_Pos); // Clear All DMA Interrupts
}

void WS2812B_PWM_Start(void)
{
	TIM3->DIER |= TIM_DIER_CC1DE; // Enable DMA requests from TIM3 CH1
	TIM3->CCR1 = 0; // Clear first CCR to get low state on WS2812B bus (for reset)
	TIM3->CR1 |= TIM_CR1_CEN; // Run the Timer
}

void WS2812B_PWM_Stop(void)
{
	TIM3->CCR1 = 0;
	TIM3->CR1 &= ~(TIM_CR1_CEN); // Stop the Timer
	TIM3->DIER &= ~(TIM_DIER_CC1DE); // Disable DMA requests from TIM3 CH1
	TIM3->CNT = 0; // Clear CNT - prepare for next run
}

void WS2812B_Transfer_Stop(void)
{
	WS2812B_PWM_Stop();
	WS2812B_DMA_Stop();
	ws2812b_TransferComplete = 1;
}


#if (WS2812B_LOW_RAM_ENABLE == 0)
	void WS2812B_ConvertColor(void)
	{
		uint8_t byteDMA = 0; 	// Iterator for byte of DMA buffer.
		int8_t bitColor; 		// Iterator for color bits.

		for(bitColor = 7; bitColor >= 0; bitColor--) // Iterate through all bits, from the highest to lowest.
		{
// scaning green color
			if(( ledBuffer.ledColor[CurrentLed].green & (1 << bitColor)) == 0)
			{
				ledBuffer.dma[(CurrentLed * WS2812B_LED_COLOR_DEPTH) + byteDMA] = WS2812B_T0H;
			}
			else
			{
				ledBuffer.dma[(CurrentLed * WS2812B_LED_COLOR_DEPTH) + byteDMA] = WS2812B_T1H;
			}

// scaning red color
			if(( ledBuffer.ledColor[CurrentLed].red & (1 << bitColor)) == 0)
			{
				ledBuffer.dma[(CurrentLed * WS2812B_LED_COLOR_DEPTH) + byteDMA + 8] = WS2812B_T0H;
			}
			else
			{
				ledBuffer.dma[(CurrentLed * WS2812B_LED_COLOR_DEPTH) + byteDMA + 8] = WS2812B_T1H;
			}

// scaning blue color
			if(( ledBuffer.ledColor[CurrentLed].blue & (1 << bitColor)) == 0)
			{
				ledBuffer.dma[(CurrentLed * WS2812B_LED_COLOR_DEPTH) + byteDMA + 16] = WS2812B_T0H;
			}
			else
			{
				ledBuffer.dma[(CurrentLed * WS2812B_LED_COLOR_DEPTH) + byteDMA + 16] = WS2812B_T1H;
			}

			byteDMA++;
			
		}
	}
#endif

///////////////// Interrupts Handlers ///////////////////////////

#if (WS2812B_LOW_RAM_ENABLE == 1)
	void WS2812B_DMA_HT_Handler(void)
	{
		if(CurrentLed > WS2812B_LEDS) // Check if there is something to send.
		{
			WS2812B_Transfer_Stop();
		}
		else if(CurrentLed == WS2812B_LEDS)
		{
			for(uint8_t i = 0; i < WS2812B_DMA_BUFFER_LENGTH/2; i++)
			{
				ledBuffer.dma[i] = WS2812B_RESET;
			}
			CurrentLed++;
		}	
		else
		{ 
			uint8_t j = 0; // Iterator for DMA buffer. From the beginnig.
			int8_t k; // Iterator for bits.

			for(k = 7; k >= 0; k--) // Iterate through all bits, from the highest to lowest.
			{
				if(( ledBuffer.ledColor[CurrentLed].green & (1<<k)) == 0)
				{
					ledBuffer.dma[j] = WS2812B_T0H;
				}
				else
				{
					ledBuffer.dma[j] = WS2812B_T1H;
				}

				if(( ledBuffer.ledColor[CurrentLed].red & (1<<k)) == 0)
				{
					ledBuffer.dma[j+8] = WS2812B_T0H;
				}
				else
				{
					ledBuffer.dma[j+8] = WS2812B_T1H;
				}

				if(( ledBuffer.ledColor[CurrentLed].blue & (1<<k)) == 0)
				{
					ledBuffer.dma[j+16] = WS2812B_T0H;
				}
				else
				{
					ledBuffer.dma[j+16] = WS2812B_T1H;
				}
				
				j++;
			}
			CurrentLed++; // Next LEd
			TEST3_reset();
		}

	}

	void WS2812B_DMA_TC_Handler(void)
	{
	// Leds Odd 1, 3, 5, 7...

		if(CurrentLed > WS2812B_LEDS) // Check if there is something to send.
		{
			WS2812B_Transfer_Stop();
		}
		else if(CurrentLed == WS2812B_LEDS)
		{
			for(uint8_t i = WS2812B_DMA_BUFFER_LENGTH/2; i < WS2812B_DMA_BUFFER_LENGTH; i++)
			{
				ledBuffer.dma[i] = WS2812B_RESET;
			}
			CurrentLed++;
		}
		else
		{
			uint8_t j = (WS2812B_DMA_BUFFER_LENGTH/2); // Iterator for DMA buffer. From the middle.
			int8_t k;

			for(k = 7; k >= 0; k--) // Iterate through all bits, from the highest to lowest.
			{
				if(( ledBuffer.ledColor[CurrentLed].green & (1<<k)) == 0)
				{
					ledBuffer.dma[j] = WS2812B_T0H;
				}
				else
				{
					ledBuffer.dma[j] = WS2812B_T1H;
				}

				if(( ledBuffer.ledColor[CurrentLed].red & (1<<k)) == 0)
				{
					ledBuffer.dma[j+8] = WS2812B_T0H;
				}
				else
				{
					ledBuffer.dma[j+8] = WS2812B_T1H;
				}

				if(( ledBuffer.ledColor[CurrentLed].blue & (1<<k)) == 0)
				{
					ledBuffer.dma[j+16] = WS2812B_T0H;
				}
				else
				{
					ledBuffer.dma[j+16] = WS2812B_T1H;
				}
				
				j++;
			}
			CurrentLed++;
		}
	}
#endif // (WS2812B_LOW_RAM_ENABLE == 1)

void WS2812B_interrupt(void)
{
	#if (WS2812B_LOW_RAM_ENABLE == 1)
		if(DMA1->ISR & DMA_ISR_HTIF1)
		{
			DMA1->IFCR |= DMA_IFCR_CHTIF1;

			// TEST1_set();
			WS2812B_DMA_HT_Handler();
			// TEST1_reset();
		}
	#endif // (WS2812B_LOW_RAM_ENABLE == 1)
	
	// GPDMA_CxSR
	
	if(GPDMA1_Channel2->CSR & DMA_CSR_TCF)
	{
		// GPDMA_CxFCR
		GPDMA1_Channel2->CFCR |= DMA_CFCR_TCF; // transfer complete flag clear 
		
		#if (WS2812B_LOW_RAM_ENABLE == 1)
		WS2812B_DMA_TC_Handler();
		#else
		WS2812B_Transfer_Stop();	
		#endif // (WS2812B_LOW_RAM_ENABLE == 1)
		// TEST2_set();
		// TEST2_reset();

	}
}

static void WS2812B_SetDiodeColorStruct(int16_t DiodeID, ws2812b_color colorStruct)
{
	if(DiodeID >= WS2812B_LEDS || DiodeID < 0) return;
	ledBuffer.ledColor[DiodeID] = colorStruct;
}

static void WS2812B_SetOneDiode(int16_t DiodeID, uint32_t color)
{
	if(DiodeID >= WS2812B_LEDS || DiodeID < 0) return; // Escape if we try to set diode out of chain.
	
	CurrentLed = DiodeID;

	ledBuffer.ledColor[DiodeID].red = ((color >> 16) & 0x000000FF);
	ledBuffer.ledColor[DiodeID].green = ((color >> 8) & 0x000000FF);
	ledBuffer.ledColor[DiodeID].blue = (color & 0x000000FF);

	#if (WS2812B_LOW_RAM_ENABLE == 0)

		WS2812B_ConvertColor();

		for (uint8_t i = 0; i < WS2812B_END_OF_TRANSFER; i++)
		{
			ledBuffer.dma[WS2812B_DMA_BUFFER_LENGTH - WS2812B_END_OF_TRANSFER + i] = WS2812B_RESET;
		}

	#endif // #if (WS2812B_LOW_RAM_ENABLE == 0)
}

static void WS2812B_SetOneDiodeRGB(int16_t DiodeID, uint8_t R, uint8_t G, uint8_t B)
{
	uint32_t color = 0;

	color |= (R << 16);
	color |= (G << 8);
	color |= (B);

	WS2812B_SetOneDiode(DiodeID, color);
}

static void WS2812B_SetAllDiodes(uint32_t color)
{
	for(int8_t i = 0; i < WS2812B_LEDS; i++)
    {
        WS2812B_SetOneDiode(i, color);
    }
}

// static void WS2812B_SetAllDiodesRGB(uint8_t R, uint8_t G, uint8_t B)
// {
// 	uint32_t color = 0;

// 	color |= (R << 16);
// 	color |= (G << 8);
// 	color |= (B);

// 	WS2812B_SetAllDiodes(color);
// }

static uint32_t WS2812B_GetColor(int16_t DiodeID)
{
	uint32_t color = 0;
	
	color |= (ledBuffer.ledColor[DiodeID].red << 16);
	color |= (ledBuffer.ledColor[DiodeID].green << 8);
	color |= (ledBuffer.ledColor[DiodeID].blue);
	return color;
}

static uint8_t* WS2812B_GetPixels(void)
{
	return (uint8_t*)ledBuffer.ledColor;
}

// static uint8_t WS2812B_GetTransferComplete(void)
// {
// 	return ws2812b_TransferComplete;
// }

static void WS2812B_Refresh(void)
{
	WS2812B_Transfer_Start();
}

static void WS2812B_Transfer_Start(void)
{
	#if (WS2812B_LOW_RAM_ENABLE == 1)
		uint8_t i;

		// Clear buffer to get Reset signal
		for(i = 0; i < WS2812B_DMA_BUFFER_LENGTH; i++)
		{
			ledBuffer.dma[i] = WS2812B_RESET;		
		}
	#endif
	if(ws2812b_TransferComplete)
	{
		CurrentLed = 0; // From first LED
		ws2812b_TransferComplete = 0;

		WS2812B_DMA_Start();
		WS2812B_PWM_Start();
	}	
}

static void WS2812B_InitHW(void)
{
	WS2812B_PwmOutput_Init();
	WS2812B_Timer_Init();
	WS2812B_DMA_Init();
}

void WS2812B_SimpleTest_task(void)
{
	uint32_t color;
   switch (ws2812b_state)
   {
   case 0:
	  	color = WS2812B_RED; 
	   break;
   case 1:
   		color = WS2812B_GREEN; 
	   break;
   case 2:
   		color = WS2812B_BLUE; 
	   break;
   
   default:
	   break;
   }

   WS2812B_SetAllDiodes(WS2812B_BLACK); 
   WS2812B_SetOneDiode(ws2812b_led, color);
   WS2812B_Transfer_Start();

   ws2812b_led++;

   if(ws2812b_led >= WS2812B_LEDS)
   {
	   ws2812b_led = 0;
	   ws2812b_state++;
   }

   if(ws2812b_state >= 3)
   {
	   ws2812b_state = 0;
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
  
//   static const uint8_t _gammaTable[256] = {
// 	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// 	  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,
// 	  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,
// 	  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,
// 	  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12,
// 	 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
// 	 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
// 	 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
// 	 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
// 	 58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
// 	 76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
// 	 97, 99,100,102,103,105,106,108,109,111,112,114,115,117,119,120,
// 	122,124,125,127,129,130,132,134,136,137,139,141,143,145,146,148,
// 	150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,
// 	182,184,186,188,191,193,195,197,199,202,204,206,209,211,213,215,
// 	218,220,223,225,227,230,232,235,237,240,242,245,247,250,252,255};
  
  static uint8_t WS2812B_sine8(uint8_t x)
  {
	  return _sineTable[x];
  }
  
//   static uint8_t WS2812B_gamma8(uint8_t x)
//   {
// 	  return _gammaTable[x];
//   }
  
  static uint16_t WS2812B_GetNumberOfLeds(void)
  {
	return WS2812B_LEDS;
  }

  const ws2812b_drv_t *ws2812b_getDrvSTM32(void)
  {
	return &ws2812b_drv_stm32;
  }