/**
  ******************************************************************************
  * @file      ModBusAddrConverter.h
  * @author    MTL  Application Team
  * @version   
  * @brief     This file provides code common functions  for ModBus address converter       
  ******************************************************************************
*/

#ifndef _MODBUS_ADDR_CONVERTER
#define _MODBUS_ADDR_CONVERTER

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------Includes ----------------------------------*/

#include "stm32f4xx_hal.h"
 
  /** @defgroup modbus_addr_converter
  * @{
  */
  
  /**
  * @}
  */   
  
/* ------------------------ Defines  HOLDING REGISTERS ------------------------------------*/
#define HOLDING_OFFSET        (uint16_t)40000

#define DEVICE_MODEL_CODE            (uint16_t)(40401 - 40000)
#define DEVICE_ID                    (uint16_t)(40403 - 40000)
#define VERSION_ID                   (uint16_t)(40417 - 40000)
#define CALIBRATION_ZERO_POINT       (uint16_t)(40021 - 40000)
#define SENSOR_SCALE_MIN_HIGH        (uint16_t)(41001 - 40000)		


/* ------------------------ Intern  for converter addr------------------------------------*/

#define DEVICE_MODEL_CODE_INTERN      (uint16_t)(1)   // два адреса всего 
#define DEVICE_ID_INTERN              (uint16_t)(3)   // три адреса всего 
#define VERSION_ID_INTERN             (uint16_t)(6)   // три адреса всего 
#define CALIBRATION_ZERO_POINT_INTERN (uint16_t)(9)   // два адреса всего 
#define SENSOR_SCALE_MIN_HIGH_INTERN  (uint16_t)(11)	// пять адресов всего 

         /*    PRIMARY SENSOR   */
/* ------------------------ END HOLDING REGISTERS ------------------------------------*/	

/* ------------------------ Defines INPUT REGISTERS ----------------------------------*/
#define INPUT_OFFSET                   (uint16_t)30000

#define DEVICE_STATUS                  (uint16_t)(31001 - 30000) // один адрес
#define SENSOR_PRIMARY_VAUE_HIGH       (uint16_t)(33001 - 30000) // один адрес , концентрация 
#define SENSOR_PRIMARY_VAUE_LOW        (uint16_t)(33002 - 30000) // один адрес , концентрация 
#define SENSOR_PRIMARY_STATUS          (uint16_t)(33003 - 30000) // один адрес

#define DEVICE_STATUS_INTERN            (uint16_t)(1) // один адрес
#define SENSOR_PRIMARY_VAUE_HIGH_INTERN (uint16_t)(2) // один адрес , концентрация 
#define SENSOR_PRIMARY_VAUE_LOW_INTERN  (uint16_t)(3) // один адрес , концентрация 
#define SENSOR_PRIMARY_STATUS_INTERN    (uint16_t)(4) // один адрес


/* ------------------------ END INPUT REGISTERS --------------------------------------*/	
								 
/* ------------------------Global variables----------------------------*/

/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/
  
  /** @addtogroup modbus_addr_converter
  * @{
  */
  
 uint16_t convertModBusAddrHolding_M(uint16_t MBAddress);
 uint16_t convertModBusAddrInput_M(uint16_t MBAddress);
 uint16_t convertModBusAddrHolding_S(uint16_t MBAddress);
 uint16_t convertModBusAddrInput_S(uint16_t MBAddress);
  /**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif  /* _MODBUS_ADDR_CONVERTER */