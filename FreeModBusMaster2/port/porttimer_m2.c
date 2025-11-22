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
/* ----------------------- Platform includes --------------------------------*/
#include "port.h"



/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport2.h"
#include "mb_m2.h"
#include "stm32f4xx_hal.h"

/* ----------------------- static functions ---------------------------------*/
//static void prvvTIMERExpiredISR(void);

/* ----------------------- Variables ----------------------------------------*/
//extern TIM_HandleTypeDef* modbusTimer;

extern TIM_HandleTypeDef htim13;
uint16_t timerPeriod2 = 0;
volatile uint16_t timerCounter2 = 0;

/* ----------------------- Start implementation -----------------------------*/

/*----------------------------------------------------------------------------*/
BOOL xMBMaster2PortTimersInit(USHORT usTim1Timerout50us)
{
  timerPeriod2 = usTim1Timerout50us;
  return TRUE;
}

/* --------------------------------------------------------------------------*/
inline void vMBMaster2PortTimersT35Enable()
{
  vMBMaster2SetCurTimerMode(MB_TMODE_T35);
  timerCounter2 = 0;
  HAL_TIM_Base_Start_IT(&htim13);
}
/* --------------------------------------------------------------------------*/
void vMBMaster2PortTimersConvertDelayEnable()
{
  vMBMaster2SetCurTimerMode(MB_TMODE_CONVERT_DELAY);
}
/* --------------------------------------------------------------------------*/
void vMBMaster2PortTimersRespondTimeoutEnable()
{
  vMBMaster2SetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
}
/* --------------------------------------------------------------------------*/
inline void vMBMaster2PortTimersDisable()
{
  HAL_TIM_Base_Stop_IT(&htim13);
}

/* --------------------------------------------------------------------------*/
//static void prvvTIMERExpiredISR(void)
//{
//    ( void )pxMBMasterPortCBTimerExpired();
//}
/* --------------------------------------------------------------------------*/
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  if (htim->Instance == htim10.Instance)
//  {
//    timerCounter++;

//    if (timerCounter == timerPeriod)
//    {
//      prvvTIMERExpiredISR();
//    }
//  }
//}

/* --------------------------------------------------------------------------*/
