/*HEADER******************************************************************************************
* Filename: DataLogger.h
* Date: Jan 1, 2016
* Author: Carlos Neri
*
**END********************************************************************************************/
#ifndef _DATALOGGER_H_
#define _DATALOGGER_H_


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "pin_mux.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#define DATALOGGER_FILE_NAME			"Lavamax.csv"

#define DATALOGGER_MAX_LOG_MESSAGE		255

#define DATALOGGER_PIN_DETECT			BOARD_SDHC_CD_GPIO_PIN

#define DATALOGGER_PIN_DETECT_PORT		BOARD_SDHC_CD_PORT

#define DATALOGGER_PIN_DETECT_GPIO		BOARD_SDHC_CD_GPIO
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////
enum eDataLoggerErrorCodes
{
	DATA_LOGGER_OK = 0,
	DATA_LOGGER_ERROR
};
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

uint32_t DataLogger_Init(void);

uint32_t DataLogger_PostEvent(uint8_t * pLogMessage, uint8_t *pLogData, uint16_t LogDataSize);

#if defined(__cplusplus)
}
#endif // __cplusplus


#endif /* _DATALOGGER_H_ */
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////