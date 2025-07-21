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
  
/* ------------------------ Defines ------------------------------------*/
#define HOLDING_OFFSET        (uint16_t)40000

#define DEVICE_MODEL_CODE            (uint16_t)(40401 - 40000)
#define DEVICE_ID                    (uint16_t)(40403 - 40000)
#define VERSION_ID                   (uint16_t)(40417 - 40000)
#define CALIBRATION_ZERO_POINT       (uint16_t)(40021 - 40000)
#define SENSOR_SCALE_MIN_HIGH        (uint16_t)(41001 - 40000)		


/* ------------------------ Intern ------------------------------------*/

#define DEVICE_MODEL_CODE_INTERN      (uint16_t)(1)   // два адреса всего 
#define DEVICE_ID_INTERN              (uint16_t)(3)   // три адреса всего 
#define VERSION_ID_INTERN             (uint16_t)(6)   // три адреса всего 
#define CALIBRATION_ZERO_POINT_INTERN (uint16_t)(9)   // два адреса всего 
#define SENSOR_SCALE_MIN_HIGH_INTERN  (uint16_t)(11)	// пять адресов всего 

         /*    PRIMARY SENSOR   */
	 
								 
/* ------------------------Global variables----------------------------*/

/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/
  
  /** @addtogroup modbus_addr_converter
  * @{
  */
  
 uint16_t convertModBusAddr(uint16_t MBAddress);
 
  /**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif  /* _MODBUS_ADDR_CONVERTER */