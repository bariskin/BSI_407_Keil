/**
******************************************************************************
* @file      HoldingRegisterHandler.h
* @author    @OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the HoldingRegisterHandler.c
******************************************************************************
*/
#ifndef _HOLDING_REGISTER_HANDLER_H
#define _HOLDING_REGISTER_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f4xx_hal.h"
 /* ------------------------Includes ----------------------------------*/
 
  /** @defgroup holding_register_handler
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
 #define HOLDING_REGISTER_IDX_1   (uint16_t)0x0000  // setting ModBus baud rate
 #define HOLDING_REGISTER_IDX_2   (uint16_t)0x0001	// setting ModBus parity
 #define HOLDING_REGISTER_IDX_3   (uint16_t)0x0002  // setting ModBus Stop Bits
 #define HOLDING_REGISTER_IDX_4   (uint16_t)0x0003  // setting ModBus addr
 
 /* ------------------------External variables -------------------------*/
 extern volatile  uint32_t  MB_BaudRateValue;
 extern volatile  uint32_t  MB_ParityValue;
 extern volatile  uint32_t  MB_StopBitsValue;
  extern volatile  uint32_t  MB_AddresseValue;

  /** @addtogroup holding_register_handler
  * @{
  */
 void HoldingRegisterFromModbusStack(uint16_t MBregIdx, uint16_t RegValue);
 uint16_t WriteParamToModbusStack(uint16_t MBregIdx);
 void ModBusEventHandler(void);
	
	/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _HOLDING_REGISTER_HANDLER_H */
