/*
 * OneButton.c
 *
 *  Created on: Nov 19, 2022
 *  @author Jan Łukaszewicz
 */
#include "OneButton.h"


void ButtonIdle(OneButton_t *Btn)
{
	if (OB_BUTTON_PRESSED == OB_READ_PIN())
	{
		Btn->State = DEBOUNCE;
		Btn->LastTick = OB_GET_TICK();
	}

	// decide about one click
	if (2 == Btn->CoundClick)
	{
		Btn->State = DOUBLECLICK;

	}
	else if ((1 == Btn->CoundClick) && (OB_GET_TICK() - Btn->LastTick) > Btn->TimerDoubleClick)
	{
		Btn->State = ONECLICK;
	}

}

void ButtonDebounce(OneButton_t *Btn)
{
	if((OB_GET_TICK() - Btn->LastTick) > Btn->TimerDebounce)
	{
		if (OB_BUTTON_PRESSED == OB_READ_PIN())
		{
			Btn->State = DECIDE;
			Btn->LastTick = OB_GET_TICK();
		}
		else
		{
			Btn->State = IDLE;
		}
	}

}

void ClickDecide(OneButton_t *Btn)
{
	// decide of long press
	if (OB_BUTTON_PRESSED == OB_READ_PIN())
	{
		//checking LongPress
		if((OB_GET_TICK() - Btn->LastTick) > Btn->TimerLongPressStart)
		{
			Btn->State = LONGPRESS;
		}
	}
	else // decide of one or double click
	{
		Btn->CoundClick++;
		Btn->State = IDLE;
		Btn->LastTick = OB_GET_TICK();
	}
}

void ButtonOneclick(OneButton_t *Btn)
{
	Btn->CoundClick = 0;
	Btn->State = IDLE;

	if(NULL != Btn->OneClick)
	{
		Btn->OneClick(Btn->GpioPin);
	}
}

void ButtonDoubleClick(OneButton_t *Btn)
{
	Btn->CoundClick = 0;
	Btn->State = IDLE;

	if(NULL != Btn->DoubleClick)
	{
		Btn->DoubleClick(Btn->GpioPin);
	}
}

void ButtonLongPress(OneButton_t *Btn)
{
	//starting long press
	if (OB_BUTTON_PRESSED == OB_READ_PIN())
	{
		if ((OB_GET_TICK() - Btn->LastTick) > Btn->TimerLongPressTick)
		{
			Btn->LastTick = OB_GET_TICK();

			if (NULL != Btn->LongPressStart)
			{
				Btn->LongPressStart(Btn->GpioPin);
			}
		}
	}
	else // long press stop
	{
		Btn->State = IDLE;

		if (NULL != Btn->LongPressStop)
		{
			Btn->LongPressStop(Btn->GpioPin);
		}

	}

}

void OneButtonTask(OneButton_t *Btn)
{
	switch(Btn->State)
	{
	case IDLE:
		ButtonIdle(Btn);
		break;

	case DEBOUNCE:
		ButtonDebounce(Btn);
		break;

	case DECIDE:
		ClickDecide(Btn);
		break;

	case ONECLICK:
		ButtonOneclick(Btn);
		break;

	case DOUBLECLICK:
		ButtonDoubleClick(Btn);
		break;

	case LONGPRESS:
		ButtonLongPress(Btn);
		break;

	}

}

//Timers functions
void OneButtonSetTimerDebounce(OneButton_t *Btn, uint32_t DebounceTime)
{
	Btn->TimerDebounce = DebounceTime;
}

void OneButtonSetTimerDoubleClick(OneButton_t *Btn, uint32_t DoubleClickTime)
{
	Btn->TimerDoubleClick = DoubleClickTime;
}

void OneButtonSetTimerLongPressStart(OneButton_t *Btn, uint32_t LongPressStartTime)
{
	Btn->TimerLongPressStart = LongPressStartTime;
}

void OneButtonSetTimerLongPressTick(OneButton_t *Btn, uint32_t LongPressTickTime)
{
	Btn->TimerLongPressTick = LongPressTickTime;
}


// registers of callback
void OneButtonCallbackOneClick(OneButton_t *Btn, CallBackFunOneClick_t OneClickCallback)
{
	Btn->OneClick = OneClickCallback;
}

void OneButtonCallbackDoubleClick(OneButton_t *Btn, CallBackFunDoubleClick_t DoubleClickCallback)
{
	Btn->DoubleClick = DoubleClickCallback;
}

void OneButtonCallbackLongPressStart(OneButton_t *Btn, CallBackFunLongPressStart_t LongPressStartCallback)
{
	Btn->LongPressStart = LongPressStartCallback;
}

void OneButtonCallbackLongPressStop(OneButton_t *Btn, CallBackFunLongPressStop_t LongPressStopCallback)
{
	Btn->LongPressStop = LongPressStopCallback;
}

// initialized function
#ifdef OB_STM32_PLATFORM
	void OneButtonInit(OneButton_t *Btn, GPIO_TypeDef *GpioPort, uint16_t GpioPin)
#endif
#ifdef OB_ESP32_PLATFORM
	void OneButtonInit(OneButton_t *Btn, uint16_t GpioPin)
#endif
{
	Btn->State = IDLE;
	Btn->CoundClick = 0;

	Btn->OneClick = NULL;
	Btn->DoubleClick = NULL;
	Btn->LongPressStart = NULL;
	Btn->LongPressStop = NULL;

	Btn->TimerDebounce = 10;
	Btn->TimerDoubleClick = 350;
	Btn->TimerLongPressStart = 700;
	Btn->TimerLongPressTick = 500;
#ifdef OB_STM32_PLATFORM
	 Btn->GpioPort = GpioPort;
#endif
	
	Btn->GpioPin = GpioPin;
}






