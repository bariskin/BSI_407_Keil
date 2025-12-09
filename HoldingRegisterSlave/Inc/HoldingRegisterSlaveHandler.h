/**
******************************************************************************
* @file      HoldingRegisterSlaveHandler.h
* @author    @OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the HoldingRegisterSlaveHandler.c
******************************************************************************
*/
#ifndef _HOLDING_REGISTER_SLAVE_HANDLER_H
#define _HOLDING_REGISTER_SLAVE_HANDLER_H

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
 #define HOLDING_REGISTER_SLAVE_IDX_1   (uint16_t)0x0000  // setting ModBus baud rate
 #define HOLDING_REGISTER_SLAVE_IDX_2   (uint16_t)0x0001	// setting ModBus parity
 #define HOLDING_REGISTER_SLAVE_IDX_3   (uint16_t)0x0002  // setting ModBus Stop Bits
 #define HOLDING_REGISTER_SLAVE_IDX_4   (uint16_t)0x0003  // setting ModBus addr
 #define HOLDING_REGISTER_SLAVE_IDX_5   (uint16_t)0x0004  // setting the time step for reading the sensor
 #define HOLDING_REGISTER_SLAVE_IDX_6   (uint16_t)0x0005  // год 
 #define HOLDING_REGISTER_SLAVE_IDX_7   (uint16_t)0x0006  // месяц
 #define HOLDING_REGISTER_SLAVE_IDX_8   (uint16_t)0x0007  // день
 #define HOLDING_REGISTER_SLAVE_IDX_9   (uint16_t)0x0008  // часы
 #define HOLDING_REGISTER_SLAVE_IDX_10  (uint16_t)0x0009  // минуты
 #define HOLDING_REGISTER_SLAVE_IDX_11  (uint16_t)0x000A  // секунды
 #define HOLDING_REGISTER_SLAVE_IDX_12  (uint16_t)0x000B 
 #define HOLDING_REGISTER_SLAVE_IDX_13  (uint16_t)0x000C 
 #define HOLDING_REGISTER_SLAVE_IDX_14  (uint16_t)0x000D 
 #define HOLDING_REGISTER_SLAVE_IDX_15  (uint16_t)0x000E
 #define HOLDING_REGISTER_SLAVE_IDX_16  (uint16_t)0x000F 
 #define HOLDING_REGISTER_SLAVE_IDX_17  (uint16_t)0x0010
 #define HOLDING_REGISTER_SLAVE_IDX_18  (uint16_t)0x0011 
 #define HOLDING_REGISTER_SLAVE_IDX_19  (uint16_t)0x0012 
 #define HOLDING_REGISTER_SLAVE_IDX_20  (uint16_t)0x0013  
 #define HOLDING_REGISTER_SLAVE_IDX_21  (uint16_t)0x0014 
 #define HOLDING_REGISTER_SLAVE_IDX_22  (uint16_t)0x0015 
 #define HOLDING_REGISTER_SLAVE_IDX_23  (uint16_t)0x0016 
 #define HOLDING_REGISTER_SLAVE_IDX_24  (uint16_t)0x0017 
 #define HOLDING_REGISTER_SLAVE_IDX_25  (uint16_t)0x0018 
 #define HOLDING_REGISTER_SLAVE_IDX_26  (uint16_t)0x0019 
 #define HOLDING_REGISTER_SLAVE_IDX_27  (uint16_t)0x001A 
 #define HOLDING_REGISTER_SLAVE_IDX_28  (uint16_t)0x001B 
 #define HOLDING_REGISTER_SLAVE_IDX_29  (uint16_t)0x001C  
 #define HOLDING_REGISTER_SLAVE_IDX_30  (uint16_t)0x001D
 #define HOLDING_REGISTER_SLAVE_IDX_31  (uint16_t)0x001E
 #define HOLDING_REGISTER_SLAVE_IDX_32  (uint16_t)0x001F
 #define HOLDING_REGISTER_SLAVE_IDX_33  (uint16_t)0x0020
 #define HOLDING_REGISTER_SLAVE_IDX_34  (uint16_t)0x0021
 #define HOLDING_REGISTER_SLAVE_IDX_35  (uint16_t)0x0022 
 
 
 
  // Макросы для вычисления адресов обработчка для slave modbus
 #define TOTAL_CHANNEL    (NUMBER_SLAVE_DEVICES * 2) 
 #define FLOAT_START HOLDING_REGISTER_SLAVE_IDX_13
 #define SUBSTANCE_START HOLDING_REGISTER_SLAVE_IDX_15
 #define SCALE_START (SUBSTANCE_START + TOTAL_CHANNEL * 10)
 
 /* ------------------------External variables -------------------------*/
 extern volatile  uint32_t  MB_BaudRateValue;
 extern volatile  uint32_t  MB_ParityValue;
 extern volatile  uint32_t  MB_StopBitsValue;
 extern volatile  uint32_t  MB_AddresseValue;
 extern volatile  uint16_t  timeStep;
 extern volatile  uint16_t   year;
 extern volatile  uint8_t   month;
 extern volatile  uint8_t   day;
 extern volatile  uint8_t   hour;
 extern volatile  uint8_t   minute;
 extern volatile  uint8_t   second;
 
 
 
  /** @addtogroup holding_register_handler
  * @{
  */
 void HoldingRegisterFromModbusSlaveStack(uint16_t MBregIdx, uint16_t RegValue);
 uint16_t WriteParamToModbusSlaveStack(uint16_t MBregIdx);
 void ModBusSlaveEventHoldingRegHandler(void);
 void setTimeStepReadingSensores(void);	
	
	/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _HOLDING_REGISTER_HANDLER_H */
