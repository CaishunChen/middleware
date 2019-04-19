///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "board.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "Relay.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
	GPIO_Type * RelayPortBase;
	uint32_t RelayPinMask;
	uint8_t RelayPin;
	uint8_t RelayLogic;
}relay_port_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static relay_port_t const RelayPorts[RELAY_MAX] =
{
		{
			.RelayPortBase = BOARD_RELAY_RELAY_0_GPIO,
			.RelayPin = BOARD_RELAY_RELAY_0_PIN,
			.RelayPinMask = 1<< BOARD_RELAY_RELAY_0_PIN,
			.RelayLogic = 0
		},
		{
			.RelayPortBase = BOARD_RELAY_RELAY_1_GPIO,
			.RelayPin = BOARD_RELAY_RELAY_1_PIN,
			.RelayPinMask = 1<< BOARD_RELAY_RELAY_1_PIN,
			.RelayLogic = 0
		},
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////

void Relay_Initialization(void)
{
	uint8_t RelayCount = RELAY_MAX;
	gpio_pin_config_t RelayConfig =
	{
		kGPIO_DigitalOutput, 0,
	};

	BOARD_Relay();

	while(RelayCount--)
	{
		RelayConfig.outputLogic = RelayPorts[RelayCount].RelayLogic;
		GPIO_PinInit(RelayPorts[RelayCount].RelayPortBase, RelayPorts[RelayCount].RelayPin, &RelayConfig);

		Relay_Disable(RelayCount);
	}
}

void Relay_Enable(relay_t SelectedRelay)
{
	if(SelectedRelay < RELAY_MAX)
	{
		GPIO_PinWrite(RelayPorts[SelectedRelay].RelayPortBase, RelayPorts[SelectedRelay].RelayPin, (RelayPorts[SelectedRelay].RelayLogic & 0x01));
	}
}

void Relay_Disable(relay_t SelectedRelay)
{
	if(SelectedRelay < RELAY_MAX)
	{
		GPIO_PinWrite(RelayPorts[SelectedRelay].RelayPortBase, RelayPorts[SelectedRelay].RelayPin, (~RelayPorts[SelectedRelay].RelayLogic) & 0x01);

	}
}

bool Relay_GetStatus(relay_t SelectedRelay)
{
	uint32_t RelayValue;
	bool RelayStatus = false;

	if(SelectedRelay < RELAY_MAX)
	{
		RelayValue = GPIO_PinRead(RelayPorts[SelectedRelay].RelayPortBase, RelayPorts[SelectedRelay].RelayPin);

		if(RelayPorts[SelectedRelay].RelayLogic)
		{
			RelayStatus = RelayValue ? true : false;
		}
		else
		{
			RelayStatus = RelayValue ? false : true;
		}
	}

	return(RelayStatus);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////


