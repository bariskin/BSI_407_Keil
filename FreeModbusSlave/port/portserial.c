#include "port.h"
#include "mb.h"
#include "mbport.h"

#if MB_SLAVE_RTU_ENABLED > 0 || MB_SLAVE_ASCII_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
UART_HandleTypeDef *slaveUart;
volatile uint8_t singlechar;


extern UART_HandleTypeDef* modbusUartMaster ;
/* ----------------------- User defenitions ---------------------------------*/
#define RS485_RD_LOW_SLAVE	  HAL_GPIO_WritePin(RDen1_GPIO_Port, RDen1_Pin, GPIO_PIN_RESET)
#define RS485_RD_HIGH_SLAVE 	HAL_GPIO_WritePin(RDen1_GPIO_Port, RDen1_Pin, GPIO_PIN_SET)

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit( void *dHUART, ULONG ulBaudRate, void *dHTIM )
{																																		 
	slaveUart = (UART_HandleTypeDef *)dHUART;
	return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if(xRxEnable)
	{
		RS485_RD_LOW_SLAVE;
		HAL_UART_Receive_IT(slaveUart, (uint8_t*)&singlechar, 1);
	}	
	else
	{
		HAL_UART_AbortReceive_IT(slaveUart);
	}

	if(xTxEnable)
	{
		RS485_RD_HIGH_SLAVE;
		pxMBFrameCBTransmitterEmpty();
	}
	else
	{
		HAL_UART_AbortTransmit_IT(slaveUart);
	}
}

void vMBPortClose(void)
{
	HAL_UART_AbortReceive_IT(slaveUart);
	HAL_UART_AbortTransmit_IT(slaveUart);
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	HAL_UART_Transmit_IT(slaveUart, (uint8_t*)&ucByte, 1);
	return TRUE;
}

BOOL xMBPortSerialPutBytes(volatile UCHAR *ucByte, USHORT usSize)
{
	HAL_UART_Transmit_IT(slaveUart, (uint8_t *)ucByte, usSize);
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = (uint8_t)(singlechar);
	return TRUE;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	
  if(huart->Instance == slaveUart->Instance)
		{
		 pxMBFrameCBByteReceived();
		 HAL_UART_Receive_IT(slaveUart, (uint8_t *)&singlechar, 1);
		}
	else	if (huart->Instance == modbusUartMaster->Instance)
  {
    //prvvUARTRxISR();
		pxMBMasterFrameCBByteReceived();
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == slaveUart->Instance)
	{
		pxMBFrameCBTransmitterEmpty();
	}
	else if (huart->Instance == modbusUartMaster->Instance)
  {
    //prvvUARTTxReadyISR();
		 pxMBMasterFrameCBTransmitterEmpty();
  }
}

#endif
