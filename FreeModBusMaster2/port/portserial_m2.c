/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport2.h"
#include "stm32f4xx_hal.h"
#include "main.h"



#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdbool.h"
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR2(void);
//static void prvvUARTRxISR(void);
void ModbusMaster2_Enable(bool enable);
/* ----------------------- Variables ----------------------------------------*/

extern UART_HandleTypeDef huart4;
UART_HandleTypeDef* modbusUartMaster2 = &huart4;
static uint8_t txByte = 0x00;
static volatile uint8_t rxByte2 = 0x00;

extern osThreadId MasterModbus2TasHandle;
extern osThreadId HoldingHandlerHandle2;
extern TIM_HandleTypeDef htim13;

//extern UART_HandleTypeDef* modbusUartMaster2 ;
/* ----------------------- User defenitions ---------------------------------*/
#define RS485_RD_LOW_MASTER2	  HAL_GPIO_WritePin(RDen_GPIO_Port, RDen_Pin, GPIO_PIN_RESET)
#define RS485_RD_HIGH_MASTER2 	HAL_GPIO_WritePin(RDen_GPIO_Port, RDen_Pin, GPIO_PIN_SET)
/* ----------------------- Start implementation -----------------------------*/

/*----------------------------------------------------------------------------*/
void vMBMaster2PortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	
 if(xRxEnable) 
  {
		RS485_RD_LOW_MASTER2;	
    HAL_UART_Receive_IT(modbusUartMaster2, (uint8_t*)&rxByte2, 1);
  }
		
 else
  {
    HAL_UART_AbortReceive_IT(modbusUartMaster2);
  }
 
	if(xTxEnable)
  { 
    if (modbusUartMaster2->gState == HAL_UART_STATE_READY)
    {
			RS485_RD_HIGH_MASTER2; 
      prvvUARTTxReadyISR2();
    }
	}
  else
  {
    HAL_UART_AbortTransmit_IT(modbusUartMaster2);
  }
  
}



/* --------------------------------------------------------------------------*/
BOOL xMBMaster2PortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
  return TRUE;
}



/* --------------------------------------------------------------------------*/
BOOL xMBMaster2PortSerialPutByte(CHAR ucByte)
{
  txByte = ucByte;
  HAL_UART_Transmit_IT(modbusUartMaster2, &txByte, 1);
  return TRUE;
}

/* --------------------------------------------------------------------------*/
BOOL xMBMaster2PortSerialGetByte( CHAR * pucByte )
{
  *pucByte = rxByte2;
  HAL_UART_Receive_IT(modbusUartMaster2, (uint8_t*)&rxByte2, 1);
  return TRUE;
}

/* --------------------------------------------------------------------------*/
static void prvvUARTTxReadyISR2(void)
{
  pxMBMaster2FrameCBTransmitterEmpty();
}

/* --------------------------------------------------------------------------*/
//static void prvvUARTRxISR(void)
//{
//  pxMBMasterFrameCBByteReceived();
//}

void ModbusMaster2_Enable(bool enable)
{
    if(enable)
    {
        // Включаем UART прерывания
        __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE | UART_IT_IDLE);
        HAL_UART_Receive_IT(&huart4, (uint8_t*)&rxByte2, 1);

        // RS485 готов к приёму
        RS485_RD_LOW_MASTER2;

        // Старт таймера t3.5
        //StartT35Timer_Master2();
        HAL_TIM_Base_Start_IT(&htim13);
        // Resume оба таска
        vTaskResume(MasterModbus2TasHandle);
        vTaskResume(HoldingHandlerHandle2);
    }
    else
    {
        // Suspend оба таска
			 vTaskSuspend(HoldingHandlerHandle2); 
		   vTaskSuspend(MasterModbus2TasHandle);

        // Останавливаем UART полностью
        HAL_UART_Abort_IT(&huart4);
        __HAL_UART_DISABLE_IT(&huart4, UART_IT_RXNE | UART_IT_IDLE | UART_IT_TXE | UART_IT_TC);

        // RS485 переводим в безопасное состояние
        RS485_RD_LOW_MASTER2;

        // Останов таймера t3.5
        //StopT35Timer_Master2();
			  HAL_TIM_Base_Stop_IT(&htim13);
    }
}

/* --------------------------------------------------------------------------*/
