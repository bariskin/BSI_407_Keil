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
 
 #define HOLDING_REGISTER_SLAVE_IDX_36  (uint16_t)0x0023 
 #define HOLDING_REGISTER_SLAVE_IDX_37  (uint16_t)0x0024 
 #define HOLDING_REGISTER_SLAVE_IDX_38  (uint16_t)0x0025  
 #define HOLDING_REGISTER_SLAVE_IDX_39  (uint16_t)0x0026 
 #define HOLDING_REGISTER_SLAVE_IDX_40  (uint16_t)0x0027 
 #define HOLDING_REGISTER_SLAVE_IDX_41  (uint16_t)0x0028 
 #define HOLDING_REGISTER_SLAVE_IDX_42  (uint16_t)0x0029 
 #define HOLDING_REGISTER_SLAVE_IDX_43  (uint16_t)0x002A 
 #define HOLDING_REGISTER_SLAVE_IDX_44  (uint16_t)0x002B
 #define HOLDING_REGISTER_SLAVE_IDX_45  (uint16_t)0x002C 
 #define HOLDING_REGISTER_SLAVE_IDX_46  (uint16_t)0x002D 
 #define HOLDING_REGISTER_SLAVE_IDX_47  (uint16_t)0x002E 
 #define HOLDING_REGISTER_SLAVE_IDX_48  (uint16_t)0x002F 
 #define HOLDING_REGISTER_SLAVE_IDX_49  (uint16_t)0x0030 
 #define HOLDING_REGISTER_SLAVE_IDX_50  (uint16_t)0x0031 
 #define HOLDING_REGISTER_SLAVE_IDX_51  (uint16_t)0x0032 
 #define HOLDING_REGISTER_SLAVE_IDX_52  (uint16_t)0x0033 
 #define HOLDING_REGISTER_SLAVE_IDX_53  (uint16_t)0x0034 
 #define HOLDING_REGISTER_SLAVE_IDX_54  (uint16_t)0x0035 
 #define HOLDING_REGISTER_SLAVE_IDX_55  (uint16_t)0x0036 
 #define HOLDING_REGISTER_SLAVE_IDX_56  (uint16_t)0x0038 
 #define HOLDING_REGISTER_SLAVE_IDX_57  (uint16_t)0x0039 
 #define HOLDING_REGISTER_SLAVE_IDX_58  (uint16_t)0x003A 
 #define HOLDING_REGISTER_SLAVE_IDX_59  (uint16_t)0x003B 
 #define HOLDING_REGISTER_SLAVE_IDX_60  (uint16_t)0x003C
 #define HOLDING_REGISTER_SLAVE_IDX_61  (uint16_t)0x003D 
 #define HOLDING_REGISTER_SLAVE_IDX_62  (uint16_t)0x003E 
 
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
