///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fsl_lpuart.h"
#include "clock_config.h"
#include "SerialPlatform.h"
#include "MiscFunctions.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define SERIAL_PLATFORM_UART			(LPUART0)

#define SERIAL_PLATFORM_RX_DONE_FLAG	(0)

#define SERIAL_PLATFORM_TX_DONE_FLAG	(0)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

static void SerialPlatform_Callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Global Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////
static lpuart_handle_t UartHandle;

static lpuart_transfer_t TxUartTransfer;

static lpuart_transfer_t RxUartTransfer;

static uint8_t DataReceived;

static volatile uint16_t SerialPlatformStatus = 0;

static serialplatformcallback_t ReportDataCallback = NULL;
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                   Static Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions Section
///////////////////////////////////////////////////////////////////////////////////////////////////


void SerialPlatform_Init (uint32_t BaudRate, serialplatformcallback_t Callback)
{
	lpuart_config_t UartConfig;
	uint32_t UartClockSource;

	CLOCK_SetLpuart0Clock(0x1U);

	LPUART_GetDefaultConfig(&UartConfig);

	ReportDataCallback = Callback;

	UartConfig.baudRate_Bps = BaudRate;
	UartConfig.enableTx = true;
	UartConfig.enableRx = true;

	UartClockSource = CLOCK_GetFreq(kCLOCK_McgIrc48MClk);

	LPUART_Init(SERIAL_PLATFORM_UART, &UartConfig, UartClockSource);

	LPUART_TransferCreateHandle(SERIAL_PLATFORM_UART,&UartHandle,SerialPlatform_Callback,NULL);

	RxUartTransfer.data = &DataReceived;
	RxUartTransfer.dataSize = 1;

	LPUART_TransferReceiveNonBlocking(SERIAL_PLATFORM_UART, &UartHandle, &RxUartTransfer, NULL);

	EnableIRQ(LPUART0_IRQn);
}

void SerialPlatform_SendNonBlocking(uint8_t * CommandBuffer, uint16_t BufferSize)
{
	TxUartTransfer.data = CommandBuffer;
	TxUartTransfer.dataSize = BufferSize;

	CLEAR_FLAG(SerialPlatformStatus,SERIAL_PLATFORM_TX_DONE_FLAG);

	LPUART_TransferSendNonBlocking(SERIAL_PLATFORM_UART, &UartHandle, &TxUartTransfer);
}

void SerialPlatform_SendBlocking(uint8_t * CommandBuffer, uint16_t BufferSize)
{
	TxUartTransfer.data = CommandBuffer;
	TxUartTransfer.dataSize = BufferSize;

	CLEAR_FLAG(SerialPlatformStatus,SERIAL_PLATFORM_TX_DONE_FLAG);

	LPUART_TransferSendNonBlocking(SERIAL_PLATFORM_UART, &UartHandle, &TxUartTransfer);

	while(!CHECK_FLAG(SerialPlatformStatus,SERIAL_PLATFORM_TX_DONE_FLAG))
	{

	}
}

uint8_t SerialPlatform_Read (void)
{
	CLEAR_FLAG(SerialPlatformStatus,SERIAL_PLATFORM_RX_DONE_FLAG);

	LPUART_TransferReceiveNonBlocking(SERIAL_PLATFORM_UART, &UartHandle, &RxUartTransfer, NULL);

	return DataReceived;
}

serialplatformstatus_t SerialPlatform_RxStatus(uint8_t * NewData)
{
	serialplatformstatus_t Status = SERIAL_PLATFORM_ERROR;

	if(CHECK_FLAG(SerialPlatformStatus,SERIAL_PLATFORM_RX_DONE_FLAG))
	{
		*NewData = DataReceived;

		CLEAR_FLAG(SerialPlatformStatus,SERIAL_PLATFORM_RX_DONE_FLAG);

		LPUART_TransferReceiveNonBlocking(SERIAL_PLATFORM_UART, &UartHandle, &RxUartTransfer, NULL);

		Status = SERIAL_PLATFORM_DATA_RECEIVED;
	}

	return Status;
}

void SerialPlatform_Callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
	if(kStatus_LPUART_RxIdle == status)
	{
		SET_FLAG(SerialPlatformStatus,SERIAL_PLATFORM_RX_DONE_FLAG);

		ReportDataCallback(DataReceived);

		LPUART_TransferReceiveNonBlocking(SERIAL_PLATFORM_UART, &UartHandle, &RxUartTransfer, NULL);
	}

	if(kStatus_LPUART_RxHardwareOverrun == status)
	{
		LPUART_ClearStatusFlags(SERIAL_PLATFORM_UART,kLPUART_RxOverrunFlag);
	}

	if(kStatus_LPUART_TxIdle == status)
	{
		SET_FLAG(SerialPlatformStatus,SERIAL_PLATFORM_TX_DONE_FLAG);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
