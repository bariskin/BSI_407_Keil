#include "port.h"
#include "mb.h"
#include "mbport.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
UART_HandleTypeDef *uart2;
volatile uint8_t singlechar;

//extern UART_HandleTypeDef huart2;
/* ----------------------- User defenitions ---------------------------------*/
#define RS485_RD_LOW	  HAL_GPIO_WritePin(USART2_RD_GPIO_Port, USART2_RD_Pin, GPIO_PIN_RESET)
#define RS485_RD_HIGH 	HAL_GPIO_WritePin(USART2_RD_GPIO_Port, USART2_RD_Pin, GPIO_PIN_SET)

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit( void *dHUART, ULONG ulBaudRate, void *dHTIM )
{																																		 
	uart2 = (UART_HandleTypeDef *)dHUART;
	return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if(xRxEnable)
	{
		RS485_RD_LOW;
		HAL_UART_Receive_IT(uart2, (uint8_t*)&singlechar, 1);
	}	
	else
	{
		HAL_UART_AbortReceive_IT(uart2);
	}

	if(xTxEnable)
	{
		RS485_RD_HIGH;
		pxMBFrameCBTransmitterEmpty();
	}
	else
	{
		HAL_UART_AbortTransmit_IT(uart2);
	}
}

void vMBPortClose(void)
{
	HAL_UART_AbortReceive_IT(uart2);
	HAL_UART_AbortTransmit_IT(uart2);
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	HAL_UART_Transmit_IT(uart2, (uint8_t*)&ucByte, 1);
	return TRUE;
}

BOOL xMBPortSerialPutBytes(volatile UCHAR *ucByte, USHORT usSize)
{
	HAL_UART_Transmit_IT(uart2, (uint8_t *)ucByte, usSize);
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = (uint8_t)(singlechar);
	return TRUE;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	
if(huart->Instance == uart2->Instance)
		{
		 pxMBFrameCBByteReceived();
		 HAL_UART_Receive_IT(uart2, (uint8_t *)&singlechar, 1);
		}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == uart2->Instance)
	{
		pxMBFrameCBTransmitterEmpty();
	}
}

#endif
