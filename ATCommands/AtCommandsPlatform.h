/*HEADER******************************************************************************************
* Filename: AtCommands.h
* Date: Apr 26, 2016
* Author: Carlos Neri
*
**END********************************************************************************************/
#ifndef ATCOMMANDSPLATFORM_H_
#define ATCOMMANDSPLATFORM_H_


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "fsl_uart.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define AT_COMMANS_PLAT_UART	(UART2)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
	ATCOMMANDS_PLATFORM_DATA_RECEIVED = 0,
	ATCOMMANDS_PLATFORM_DATA_OK,
	ATCOMMANDS_PLATFORM_ERROR = 0xFF
}AtCommandsPlatformStatus_t;

typedef void (*AtCommandsPlatformCallback_t)(uint8_t);
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                Function-like Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Extern Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Extern Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

void AtCommands_PlatformUartInit  (uint32_t BaudRate, AtCommandsPlatformCallback_t Callback);

void AtCommands_PlatformUartSend (uint8_t * CommandBuffer, uint16_t BufferSize);

AtCommandsPlatformStatus_t AtCommands_PlatformUartRxStatus(uint8_t * NewData);

uint8_t AtCommands_PlatformUartRead (void);
#if defined(__cplusplus)
}
#endif // __cplusplus


#endif /* ATCOMMANDSPLATFORM_H_ */
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
