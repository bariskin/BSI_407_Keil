/**
******************************************************************************
* @file      ModbusSettings.h
* @author    @OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the  ModbusSettings.c
******************************************************************************
*/
#ifndef _MODBUS_SETTINGS_H
#define _MODBUS_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif
  
 /* ------------------------Includes ----------------------------------*/
 #include "stm32f4xx_hal.h"
	/** @defgroup modbus_settings
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
    
 /* ------------------------External variables -------------------------*/
 
 extern TIM_HandleTypeDef *ModBusSlaveTimer;
 extern uint16_t ModBusSlaveTimeout;
 extern volatile uint16_t ModBusSlaveCounter;
 extern uint8_t ModBusDefaultDeviceAddr; 
  /** @addtogroup modbus_settings
  * @{
  */

  /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _MODBUS_SETTINGS_H */
