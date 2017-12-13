///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "led.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "board.h"
#include "SW_Timer.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define LED_FLASHING_RATE_MS		(100)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	GPIO_Type * LedPortBase;
	uint32_t LedPinMask;
	uint8_t LedPin;
}led_port_t;
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void Led_FlashingTimerCallback(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static led_port_t LedsPorts[LED_MAX] =
{
		{
			.LedPortBase = BOARD_LED_RED_GPIO,
			.LedPin = BOARD_LED_RED_GPIO_PIN,
			.LedPinMask = 1<< BOARD_LED_RED_GPIO_PIN
		},
		{
			.LedPortBase = BOARD_LED_BLUE_GPIO,
			.LedPin = BOARD_LED_BLUE_GPIO_PIN,
			.LedPinMask = 1<< BOARD_LED_BLUE_GPIO_PIN
		},
		{
			.LedPortBase = BOARD_LED_GREEN_GPIO,
			.LedPin = BOARD_LED_GREEN_GPIO_PIN,
			.LedPinMask = 1<< BOARD_LED_GREEN_GPIO_PIN
		},
};

static uint8_t Led_FlashingTimer;

static uint8_t Led_ToFlash;
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void Led_Initialization(void)
{
	uint8_t LedCount = LED_MAX;
	gpio_pin_config_t LedConfig =
	{
		kGPIO_DigitalOutput, 0,
	};

	BOARD_InitLEDs();

	while(LedCount--)
	{
		GPIO_PinInit(LedsPorts[LedCount].LedPortBase, LedsPorts[LedCount].LedPin, &LedConfig);
		Led_TurnOff((led_t)LedCount);
	}

	Led_FlashingTimer = SWTimer_AllocateChannel(LED_FLASHING_RATE_MS,Led_FlashingTimerCallback);

}

void Led_TurnOn(led_t LedToHandle)
{
	GPIO_ClearPinsOutput(LedsPorts[LedToHandle].LedPortBase, LedsPorts[LedToHandle].LedPinMask);
}

void Led_TurnOff(led_t LedToHandle)
{
	GPIO_SetPinsOutput(LedsPorts[LedToHandle].LedPortBase, LedsPorts[LedToHandle].LedPinMask);
}

void Led_Toggle(led_t LedToHandle)
{
	GPIO_TogglePinsOutput(LedsPorts[LedToHandle].LedPortBase, LedsPorts[LedToHandle].LedPinMask);
}

void Led_StartFlashing(led_t LedToFlash)
{
	Led_ToFlash |= (1 << LedToFlash);

	SWTimer_EnableTimer(Led_FlashingTimer);
}

void Led_StopFlashing(led_t LedToFlash)
{
	Led_ToFlash &= ~(1<<LedToFlash);

	Led_TurnOff(LedToFlash);

	if(!Led_ToFlash)
	{
		SWTimer_DisableTimer(Led_FlashingTimer);
	}
}

void Led_FlashingTimerCallback(void)
{
	uint8_t LedFlashing = LED_MAX;

	while(LedFlashing--)
	{
		if(Led_ToFlash & (1<<LedFlashing))
		{
			Led_Toggle((led_t)LedFlashing);
		}
	}

}
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////

