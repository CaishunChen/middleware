/*HEADER******************************************************************************************
* Filename: AtCommands.h
* Date: Apr 26, 2016
* Author: Carlos Neri
*
**END********************************************************************************************/
#ifndef ATCOMMANDS_H_
#define ATCOMMANDS_H_


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define AT_COMMANDS_BAUDRATE	(115200)

#define AT_COMMANDS_TIMEOUT_MS	(2000)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	const uint8_t * Response;
	uint16_t ResponseSize;
	bool (*ResponseCallback)(uint8_t * , uint16_t );
}AtCommandResponse_t;

typedef enum
{
	ATCOMMANDS_OK = 0,
	ATCOMMANDS_ERROR,
	ATCOMMANDS_WRONG_PARAMETER,
}AtCommandsStatus_t;

typedef enum
{
	ATCOMMANDS_RESPONSE_NOT_FOUND_EVENT = 0,
	ATCOMMANDS_CUSTOM_RESPONSE_EVENT,
	ATCOMMANDS_COMMAND_TIMEOUT_ERROR_EVENT,
}AtCommandsEvent_t;

typedef void (* AtCommand_callback_t)(AtCommandsEvent_t, uint8_t*, uint16_t);
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

#ifndef FSL_RTOS_FREE_RTOS
void AtCommands_Task(void);
#else
void AtCommands_Task(void * param);
#endif

void AtCommands_Init(AtCommand_callback_t AppCallback, AtCommandResponse_t * ResponseTable, uint16_t AmountCommands);

AtCommandsStatus_t ATCommands_SetCommand(uint8_t * CommandToSend, uint8_t *Parameters);

AtCommandsStatus_t ATCommands_ExecuteCommand(uint8_t * CommandToSend);

AtCommandsStatus_t ATCommands_SendCustomCommand(uint8_t *CommandToSend, uint16_t CommandSize);


#if defined(__cplusplus)
}
#endif // __cplusplus


#endif /* ATCOMMANDS_H_ */
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
