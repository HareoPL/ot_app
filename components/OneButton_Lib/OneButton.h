/*
 * OneButton.h
 *
 *  Created on: Nov 19, 2022
 *  @author Jan Łukaszewicz
 */

#ifndef INC_ONEBUTTON_H_
#define INC_ONEBUTTON_H_

#include "main.h"
//for STM32
// #define OB_READ_PIN() 			HAL_GPIO_ReadPin(Btn->GpioPort, Btn->GpioPin)
// #define OB_GET_TICK() 			HAL_GetTick()
// #define OB_BUTTON_PRESSED 		GPIO_PIN_RESET
// #define OB_BUTTON_NOT_PRESSED 	GPIO_PIN_SET
//

// for ESP32
#include "xtimers.h"
#include "ot_app.h"
#define OB_READ_PIN() 			gpio_get_level(Btn->GpioPin)
#define OB_GET_TICK() 			xTim_getTick()
#define OB_BUTTON_PRESSED 		0
#define OB_BUTTON_NOT_PRESSED 	1

//if you need other callback, modify here only. Keep the name.
// typedef void(*CallBackFunOneClick_t)(void);
// typedef void(*CallBackFunDoubleClick_t)(void);
// typedef void(*CallBackFunLongPressStart_t)(void);
// typedef void(*CallBackFunLongPressStop_t)(void);

typedef void(*CallBackFunOneClick_t)(uint16_t btnNum);
typedef void(*CallBackFunDoubleClick_t)(uint16_t btnNum);
typedef void(*CallBackFunLongPressStart_t)(uint16_t btnNum);
typedef void(*CallBackFunLongPressStop_t)(uint16_t btnNum);


typedef enum
{
	IDLE = 0,
	DEBOUNCE,
	DECIDE,
	ONECLICK,
	DOUBLECLICK,
	LONGPRESS

}ButtonState;

typedef struct
{
	ButtonState		State;

	// GPIO_TypeDef* 	GpioPort;
	uint16_t 		GpioPin;

	uint8_t			CoundClick;

	uint32_t 		LastTick;
	uint32_t		TimerDebounce;
	uint32_t		TimerDoubleClick;
	uint32_t		TimerLongPressStart;
	uint32_t		TimerLongPressTick;

	CallBackFunOneClick_t 			OneClick;
	CallBackFunDoubleClick_t  		DoubleClick;
	CallBackFunLongPressStart_t  	LongPressStart;
	CallBackFunLongPressStop_t  	LongPressStop;


}OneButton_t;


// Public functions
// void OneButtonInit(OneButton_t *Btn, GPIO_TypeDef *GpioPort, uint16_t GpioPin);
void OneButtonInit(OneButton_t *Btn, uint16_t GpioPin);
void OneButtonTask(OneButton_t *Btn);

//setup Timers functions
void OneButtonSetTimerDebounce(OneButton_t *Btn, uint32_t DebounceTime);
void OneButtonSetTimerDoubleClick(OneButton_t *Btn, uint32_t DoubleClickTime);
void OneButtonSetTimerLongPressStart(OneButton_t *Btn, uint32_t LongPressStartTime);
void OneButtonSetTimerLongPressTick(OneButton_t *Btn, uint32_t LongPressTickTime);

//registers of callback functions
void OneButtonCallbackOneClick(OneButton_t *Btn, CallBackFunOneClick_t OneClickCallback);
void OneButtonCallbackDoubleClick(OneButton_t *Btn, CallBackFunDoubleClick_t DoubleClickCallback);
void OneButtonCallbackLongPressStart(OneButton_t *Btn, CallBackFunLongPressStart_t LongPressStartCallback);
void OneButtonCallbackLongPressStop(OneButton_t *Btn, CallBackFunLongPressStop_t LongPressStopCallback);



#endif /* INC_ONEBUTTON_H_ */
