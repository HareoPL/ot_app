/**
 * @file ws2812b_drv_tim_dma_stm32.h
 * @brief Implementation of the WS2812B LED driver using TIM PWM and the new GPDMA architecture.
 * @details see more information in section: @ref ws2812b_drv_tim_dma_stm32 
 * 
 * @defgroup ws2812b_drv_tim_dma_stm32 WS2812B tim dma driver for stm32
 * @ingroup ws2812b_fx
 * @brief Implementation of the WS2812B LED driver using TIM PWM and the new GPDMA architecture. 
 * @details
 * @{
 *  =========================================================================================
 * @section CONCEPT 1. TIM PWM + GPDMA COLLABORATION CONCEPT
 * =========================================================================================
 * The driver offloads the CPU by utilizing hardware peripherals. WS2812B LEDs require a
 * strict 800 kHz timing regime, where logic "1" and "0" differ only by the duty cycle.
 * Instead of bit-banging the GPIO, a Timer in PWM mode is used, and its duty cycle (CCR)
 * is updated on-the-fly by the GPDMA.
 *
 * ### Step-by-step Execution:
 * 1. **SRAM Buffer:** Colors are translated into a byte array (`ledBuffer.dma`), where each byte
 * represents a specific PWM duty cycle (`WS2812B_T0H` or `WS2812B_T1H`).
 * 2. **PWM Generation (TIM3):** The Timer runs with an 800 kHz period.
 * 3. **Hardware Request:** On each update/overflow, the Timer generates a hardware DMA request (`CC1DE`).
 * 4. **Data Injection (GPDMA):** The GPDMA channel intercepts the request, reads one byte from RAM,
 * pads it to 32-bit (PAM=00), and writes it to the `TIM3->CCR1` register.
 * 5. **End of Frame:** When the `BNDT` byte counter reaches 0, the transfer stops, triggering a
 * `TCF` (Transfer Complete) interrupt. The software then stops the Timer, pulling the line LOW
 * for the required RESET time.
 *
 * ### Collaboration Diagram
 * ```mermaid
 * sequenceDiagram
 * participant RAM as SRAM Memory<br>(ledBuffer)
 * participant DMA as GPDMA1_Ch2
 * participant TIM as TIM3 (PWM)
 * participant PIN as GPIO (PA2)
 *
 * Note over TIM, PIN: Timer runs independently (800kHz)
 * loop Every PWM period (1.25 µs)
 * TIM->>PIN: Start PWM HIGH state
 * TIM-->>DMA: DREQ (Hardware Request from CH1)
 * DMA->>RAM: Read 1 byte (SINC=1)
 * RAM-->>DMA: Data (e.g. T1H value)
 * Note over DMA: Pad 8-bit to 32-bit (PAM=00)
 * DMA->>TIM: Write to TIM3->CCR1 register (DINC=0)
 * TIM->>PIN: Go LOW when CCR1 match occurs
 * end
 * Note over DMA, TIM: BNDT counter reaches 0 (CBR1 = 0)
 * DMA-->>RAM: Trigger TCF Interrupt (Transfer Complete)
 * Note over RAM, PIN: CPU stops TIM3 -> RESET signal for LEDs
 * ```
 *
 * =========================================================================================
 * @section CHANGES 2. CHANGES COMPARED TO CLASSIC DMA (STM32F4/L4 -> STM32U5/H5/WBA6)
 * =========================================================================================
 *
 * 1. **TRANSFER DIRECTION AND THE 'DIR' BIT**
 * - The `DIR` bit has been removed in GPDMA. Transfers ALWAYS occur from Source to Destination.
 * - Instead, we configure the `DREQ` (Destination Request) bit in the TR2 register. 
 * - For WS2812B, we set `DREQ = 1`, which means: "GPDMA copies data from RAM to the Timer, 
 * but the Timer dictates the pace by generating hardware requests".
 *
 * 2. **DATA SIZE REGISTER (Former CNDTR -> New BR1.BNDT)**
 * - @warning In the old DMA, we entered the number of ELEMENTS (e.g., 10 32-bit words).
 * - In GPDMA, the `CBR1` register (`BNDT[15:0]` field) ALWAYS takes the NUMBER OF BYTES 
 * to transfer from the source memory, regardless of the configured data width.
 *
 * 3. **REQUEST MULTIPLEXER (Former DMAMUX -> New TR2.REQSEL)**
 * - GPDMA has a built-in multiplexer. The hardware request ID (e.g., TIM3_CH1) 
 * is written directly to the `REQSEL[6:0]` field in the `CTR2` register.
 *
 * 4. **CLEARING FLAGS (CFCR Register)**
 * - @warning Before enabling the channel again (`EN = 1`), you MUST clear the 
 * interrupt flags (TCF, HTF, errors) in the `CFCR` register using `=`. Do not use `|=`.
 * Failure to do so will trigger an immediate, erroneous interrupt.
 *
 * =========================================================================================
 * @section MODES 3. OPERATION MODES: SINGLE VS CIRCULAR
 * =========================================================================================
 *  ### SINGLE MODE (Recommended for WS2812B)
 * The WS2812B protocol requires a >50µs LOW state reset after a frame. Single mode allows
 * the transfer to naturally stop so the CPU/RTOS can apply the reset delay before sending
 * the next frame.
 * - Configuration: `GPDMA1_Channel2->CLLR = 0;` (No linked lists).
 * - @note The Source Address (`CSAR`) and Block Size (`CBR1`) must be manually reloaded
 * before each start because the SRAM pointer increments indefinitely.
 *  ### CIRCULAR MODE (Hardware Ring Buffer)
 * - **The `CLLR = 4` Trick:** This resets the byte counter (`BNDT`) but DOES NOT restore
 * the `CSAR` pointer. This breaks SRAM-to-Peripheral transfers (`SINC=1`) because the
 * GPDMA will continue reading beyond the buffer array, sending garbage data.
 * - **True Circular Mode:** Requires a Linked-List Item (LLI) in RAM. The LLI explicitly
 * instructs the GPDMA to reload `CBR1` (size) and `CSAR` (base address) before looping
 * back to itself (`ULL=1`).
 * 
 * @version 0.1
 * @date 03-04-2025
 * @author Jan Łukaszewicz (plhareo@gmail.com)
 * @copyright © 2025 MIT @ref prj_license 
 */
#ifndef WS2812B_HW_TIM_DMA_STM32_H_
#define WS2812B_HW_TIM_DMA_STM32_H_

#include "ws2812b_if.h"

#define WS2812B_LOW_RAM_ENABLE	0 // it is not yet implemented
#define WS2812B_LEDS	10 							// 25 // max leds uint16_t

#define WS2812B_TIMER_PRESCALER	0 					// Prescaler
#define WS2812B_TIME_PERIOD	39 						// ARR
#define WS2812B_T0H	(WS2812B_TIME_PERIOD / 3) 		// CCR = 13. Logic 0 = 1/3 time period
#define WS2812B_T1H ((2*WS2812B_TIME_PERIOD) / 3)	// CCR = 26. Logic 1 = 2/3 time period
#define WS2812B_RESET	0

#if (WS2812B_LOW_RAM_ENABLE == 1)
	#define WS2812B_DMA_BUFFER_LENGTH	48
#else
	#define WS2812B_END_OF_TRANSFER		2
	#define WS2812B_LED_COLOR_DEPTH		24
	#define WS2812B_DMA_BUFFER_LENGTH	(WS2812B_LED_COLOR_DEPTH * WS2812B_LEDS + WS2812B_END_OF_TRANSFER)
#endif // (WS2812B_LOW_RAM_ENABLE == 1)


#define WS2812B_RED		((uint32_t)0x050000)
#define WS2812B_GREEN	((uint32_t)0x000500)
#define WS2812B_BLUE	((uint32_t)0x000005)
#define WS2812B_BLACK	((uint32_t)0x000000)

// first you have to call initHW from ws2812b_drv_t
void WS2812B_SimpleTest_task(void); // add task to your OS 

// Interrupts
void WS2812B_interrupt(void);

const ws2812b_drv_t *ws2812b_getDrvSTM32(void);

#endif /* WS2812B_HW_TIM_DMA_STM32_H_ */

/**
 * @}
 */
