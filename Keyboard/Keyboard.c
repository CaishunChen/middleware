/*HEADER******************************************************************************************
BSD 3-Clause License

Copyright (c) 2020, Carlos Neri
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**END********************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "board.h"
#include "SW_Timer.h"
#include "Keyboard.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define KEYBOARD_LONGPRESS_ITERATION	(KEYBOARD_LONGPRESS_DETECT/KEYBOARD_DEBOUNCE_INTERVAL)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	GPIO_Type * ButtonPort;
	uint8_t ButtonPin;
}keyboard_pins_t;
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static keyboard_callback_t Keyboard_AppCallback;

void Keyboard_TimerCallback(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static keyboard_pins_t Keyboard_pins[KEYBOARD_MAX_SWITCHES] =
{
		{
			.ButtonPort = BOARD_SW2_GPIO,
			.ButtonPin = BOARD_SW2_GPIO_PIN
		},
		{
			.ButtonPort = BOARD_SW3_GPIO,
			.ButtonPin = BOARD_SW3_GPIO_PIN
		}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static uint8_t Keyboard_Timer;

static uint8_t Keyboard_LongPressCounter[KEYBOARD_MAX_SWITCHES];
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void Keyboard_Init(keyboard_callback_t AppCallback)
{
	gpio_pin_config_t SwConfig =
	{
	        kGPIO_DigitalInput, 0,
	};

	if(AppCallback != NULL)
	{
		Keyboard_AppCallback = AppCallback;
	}

	/* SW2 */
	PORT_SetPinInterruptConfig(BOARD_SW2_PORT, BOARD_SW2_GPIO_PIN, kPORT_InterruptFallingEdge);
	NVIC_SetPriority(BOARD_SW2_IRQ, 2);
	EnableIRQ(BOARD_SW2_IRQ);
	GPIO_PinInit(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PIN, &SwConfig);
	Keyboard_LongPressCounter[0] = KEYBOARD_LONGPRESS_ITERATION;

	/* SW3 */
	PORT_SetPinInterruptConfig(BOARD_SW3_PORT, BOARD_SW3_GPIO_PIN, kPORT_InterruptFallingEdge);
	NVIC_SetPriority(BOARD_SW3_IRQ, 2);
	EnableIRQ(BOARD_SW3_IRQ);
	GPIO_PinInit(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PIN, &SwConfig);
	Keyboard_LongPressCounter[1] = KEYBOARD_LONGPRESS_ITERATION;

	Keyboard_Timer = SWTimer_AllocateChannel(KEYBOARD_DEBOUNCE_INTERVAL,Keyboard_TimerCallback);
}

void Keyboard_LowPowerEnable(void)
{
	/* SW2 */
	PORT_SetPinInterruptConfig(BOARD_SW2_PORT, BOARD_SW2_GPIO_PIN, kPORT_InterruptOrDMADisabled);
	/* SW3 */
	PORT_SetPinInterruptConfig(BOARD_SW3_PORT, BOARD_SW3_GPIO_PIN, kPORT_InterruptOrDMADisabled);
}

void Keyboard_LowPowerDisable(void)
{
	/* SW2 */
	PORT_SetPinInterruptConfig(BOARD_SW2_PORT, BOARD_SW2_GPIO_PIN, kPORT_InterruptOrDMADisabled);
	/* SW3 */
	PORT_SetPinInterruptConfig(BOARD_SW3_PORT, BOARD_SW3_GPIO_PIN, kPORT_InterruptOrDMADisabled);
}

void Keyboard_TimerCallback (void)
{
	uint8_t PinStatus = 0;
	uint8_t PinCount = 0;
	keyboard_event_t ReportEvent;
	bool isPinProcessed = false;
	static uint8_t PreviousPressed;
	static uint32_t LongPressedReported = 0;

	while(PinCount < KEYBOARD_MAX_SWITCHES)
	{
		PinStatus = GPIO_ReadPinInput(Keyboard_pins[PinCount].ButtonPort,Keyboard_pins[PinCount].ButtonPin);

		if(!PinStatus)
		{
			/* to report a key pressed must be a fresh press */
			if(!(PreviousPressed & (1 << PinCount)))
			{
				ReportEvent = KEYBOARD_PRESSED;
				PreviousPressed |= (1 << PinCount);
				Keyboard_AppCallback(ReportEvent,(1 << PinCount));

			}
			else
			{
				/* means is being pressed*/
				if(Keyboard_LongPressCounter[PinCount])
				{
					Keyboard_LongPressCounter[PinCount]--;
				}
				else
				{
					ReportEvent = KEYBOARD_LONG_PRESSED;
					Keyboard_AppCallback(ReportEvent,(1 << PinCount));
					LongPressedReported |= 1 << PinCount;
					Keyboard_LongPressCounter[PinCount]  = KEYBOARD_LONGPRESS_ITERATION;
				}
			}
			isPinProcessed = true;
		}
		else
		{
			if((PreviousPressed & (1 << PinCount)))
			{
				/* just report release when there's wans't a long press report */
				if(!(LongPressedReported & (1 << PinCount)))
				{
					/* report the release event and erase the prev status*/
					ReportEvent = KEYBOARD_RELEASED;
					PreviousPressed &= ~(1 << PinCount);
					Keyboard_AppCallback(ReportEvent,(1 << PinCount));
				}
				else
				{
					LongPressedReported &= ~(1 << PinCount);
				}

				isPinProcessed = false;
				Keyboard_LongPressCounter[PinCount]  = KEYBOARD_LONGPRESS_ITERATION;
			}
		}

		PinCount++;
	}

	if(isPinProcessed == false)
	{
		SWTimer_DisableTimer(Keyboard_Timer);
	}

}

void PORTD_IRQHandler(void)
{
    /* Clear external interrupt flag. */
    GPIO_ClearPinsInterruptFlags(BOARD_SW3_GPIO, 1U << BOARD_SW3_GPIO_PIN);
    SWTimer_EnableTimer(Keyboard_Timer);
}

void PORTA_IRQHandler(void)
{
    /* Clear external interrupt flag. */
    GPIO_ClearPinsInterruptFlags(BOARD_SW2_GPIO, 1U << BOARD_SW2_GPIO_PIN);
    SWTimer_EnableTimer(Keyboard_Timer);
}

