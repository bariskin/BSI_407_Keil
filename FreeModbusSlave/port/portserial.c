#include "port.h"
#include "mb.h"
#include "mbport.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
UART_HandleTypeDef *uart1;
volatile uint8_t singlechar;

extern UART_HandleTypeDef *huart2;

/* ----------------------- User defenitions ---------------------------------*/
//#define RS485_RD_LOW	  HAL_GPIO_WritePin(USART2_RD_GPIO_Port, USART2_RD_Pin, GPIO_PIN_RESET)
//#define RS485_RD_HIGH 	HAL_GPIO_WritePin(USART2_RD_GPIO_Port, USART2_RD_Pin, GPIO_PIN_SET)

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit( void *dHUART, ULONG ulBaudRate, void *dHTIM )
{																																		 
	uart1 = (UART_HandleTypeDef *)dHUART;
	return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if(xRxEnable)
	{
		//RS485_RD_LOW;
		HAL_UART_Receive_IT(uart1, (uint8_t*)&singlechar, 1);
	}	
	else
	{
		HAL_UART_AbortReceive_IT(uart1);
	}

	if(xTxEnable)
	{
		//RS485_RD_HIGH;
		pxMBFrameCBTransmitterEmpty();
	}
	else
	{
		HAL_UART_AbortTransmit_IT(uart1);
	}
}

void vMBPortClose(void)
{
	HAL_UART_AbortReceive_IT(uart1);
	HAL_UART_AbortTransmit_IT(uart1);
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	HAL_UART_Transmit_IT(uart1, (uint8_t*)&ucByte, 1);
	return TRUE;
}

BOOL xMBPortSerialPutBytes(volatile UCHAR *ucByte, USHORT usSize)
{
	HAL_UART_Transmit_IT(uart1, (uint8_t *)ucByte, usSize);
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = (uint8_t)(singlechar);
	return TRUE;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == uart1->Instance)
	{
		pxMBFrameCBTransmitterEmpty();
	}
 else if (huart->Instance == huart2->Instance)
  {
     pxMBMasterFrameCBTransmitterEmpty();
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	
 if(huart->Instance == uart1->Instance)
		{
		 pxMBFrameCBByteReceived();
		 HAL_UART_Receive_IT(uart1, (uint8_t *)&singlechar, 1);
		}
	else  if(huart->Instance ==  huart2->Instance)
  {
    
	 pxMBMasterFrameCBByteReceived();
  }			
}



#endif
