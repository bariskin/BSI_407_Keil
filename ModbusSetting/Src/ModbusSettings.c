/**
******************************************************************************
* @file      ModbusSettings.c
* @author    @OnWert
* @version   
* @brief     This file provides common functions for the Modbus settings 
*/       
/* ------------------------Includes ----------------------------------*/
#include "ModbusSettings.h"
/* ------------------------External variables -------------------------*/

/* ------------------------Global variables----------------------------*/
/*  ModBus variable  */
TIM_HandleTypeDef *ModBusSlaveTimer;
uint16_t ModBusSlaveTimeout;
volatile uint16_t ModBusSlaveCounter;
uint8_t ModBusSlaveDefaultDeviceAddr = 0x01;
/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/

/************************ (C) COPYRIGHT OnWert  ****END OF FILE****/



