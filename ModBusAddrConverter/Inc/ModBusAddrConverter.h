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
#define HOLDING_OFFSET                  (uint16_t)40000

#define DEVICE_MODEL_CODE                      (uint16_t)(40401 - 40000) //DEVICE_MODEL_CODE
#define DEVICE_MODEL_CODE_2                    (uint16_t)(40402 - 40000) //DEVICE_MODEL_CODE
#define SENSOR_SCALE_MAX_HIGH                  (uint16_t)(41003 - 40000)	//Sensor Scale Max High
#define SENSOR_SCALE_MAX_LOW                   (uint16_t)(41004 - 40000)	//Sensor Scale Max Low
#define SENSOR_SCALE_DIMENSTION                (uint16_t)(41005 - 40000)	//Sensor Scale Dimension

#define SENSOR_THRESHOLD_WARNIGN_HIGN          (uint16_t)(41011 - 40000)  //Sensor Threshold Warning High
#define SENSOR_THRESHOLD_WARNIGN_LOW           (uint16_t)(41012 - 40000)  //Sensor Threshold Warning LOW
#define SENSOR_THRESHOLD_ALARM_HIGH            (uint16_t)(41013 - 40000)  //Sensor Threshold ALARM LOW
#define SENSOR_THRESHOLD_ALARM_LOW             (uint16_t)(41014 - 40000)  //Sensor Threshold ALARM LOW
#define SENSOR_THRESHOLD_ADDITIONAL_HIGH       (uint16_t)(41015 - 40000)  //Sensor Threshold ADDITIONAL LOW
#define SENSOR_THRESHOLD_ADDITIONAL_LOW        (uint16_t)(41016 - 40000)  //Sensor Threshold ADDITIONAL LOW

#define CALIBRATION_PRIMATY_ZERO               (uint16_t)(41021 - 40000)  //Calibration Primary Zero 
#define CALIBRATION_PRIMATY_ZERO_STATUS        (uint16_t)(41022 - 40000)  //Calibration Primary Zero Status
#define CALIBRATION_PRIMATY_ZERO_SIGNAL_HIGH   (uint16_t)(41023 - 40000)  //Calibration Primary Zero Signal High
#define CALIBRATION_PRIMATY_ZERO_SIGNAL_LOW    (uint16_t)(41024 - 40000)  //Calibration Primary Zero Signal Low

#define CALIBRATION_PRIMATY_SPAN               (uint16_t)(41026 - 40000)  //Calibration Primary Span 
#define CALIBRATION_PRIMATY_SPAN_STATUS        (uint16_t)(41027 - 40000)  //Calibration Primary Span Status
#define CALIBRATION_PRIMATY_SPAN_SIGNAL_HIGH   (uint16_t)(41028 - 40000)  //Calibration Primary Span Signal High
#define CALIBRATION_PRIMATY_SPAN_SIGNAL_LOW    (uint16_t)(41029 - 40000)  //Calibration Primary Span Signal Low


#define SENSOR_SUBSTANCE_CODE_1                (uint16_t)(41031 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_2                (uint16_t)(41032 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_3                (uint16_t)(41033 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_4                (uint16_t)(41034 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_5                (uint16_t)(41035 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_6                (uint16_t)(41036 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_7                (uint16_t)(41037 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_8                (uint16_t)(41038 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_9                (uint16_t)(41039 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_10               (uint16_t)(41040 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_11               (uint16_t)(41041 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_12               (uint16_t)(41042 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_13               (uint16_t)(41043 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_14               (uint16_t)(41044 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_15               (uint16_t)(41045 - 40000)  //SENSOR_SUBSTANCE_CODE 
#define SENSOR_SUBSTANCE_CODE_16               (uint16_t)(41046 - 40000)  //SENSOR_SUBSTANCE_CODE 

/* ------------------------ Intern  for converter addr------------------------------------*/

#define DEVICE_MODEL_CODE_INTERN                 (uint16_t)(1) 
#define DEVICE_MODEL_CODE_INTERN_2               (uint16_t)(2) 
#define SENSOR_SCALE_MAX_HIGH_INTERN             (uint16_t)(3) 
#define SENSOR_SCALE_MAX_LOW_INTERN              (uint16_t)(4) 
#define SENSOR_SCALE_DIMENSTION_INTERN           (uint16_t)(5) 

#define SENSOR_THRESHOLD_WARNIGN_HIGN_INTERN     (uint16_t)(6) 
#define SENSOR_THRESHOLD_WARNIGN_LOW_INTERN      (uint16_t)(7) 
#define SENSOR_THRESHOLD_ALARM_HIGH_INTERN       (uint16_t)(8) 
#define SENSOR_THRESHOLD_ALARM_LOW_INTERN        (uint16_t)(9) 
#define SENSOR_THRESHOLD_ADDITIONAL_HIGH_INTERN  (uint16_t)(10) 
#define SENSOR_THRESHOLD_ADDITIONAL_LOW_INTERN   (uint16_t)(11) 

#define SENSOR_SUBSTANCE_CODE_1_INTERN           (uint16_t)(12) 
#define SENSOR_SUBSTANCE_CODE_2_INTERN           (uint16_t)(13) 
#define SENSOR_SUBSTANCE_CODE_3_INTERN           (uint16_t)(14) 
#define SENSOR_SUBSTANCE_CODE_4_INTERN           (uint16_t)(15) 
#define SENSOR_SUBSTANCE_CODE_5_INTERN           (uint16_t)(16) 
#define SENSOR_SUBSTANCE_CODE_6_INTERN         	 (uint16_t)(17) 
#define SENSOR_SUBSTANCE_CODE_7_INTERN           (uint16_t)(18) 
#define SENSOR_SUBSTANCE_CODE_8_INTERN           (uint16_t)(19) 
#define SENSOR_SUBSTANCE_CODE_9_INTERN           (uint16_t)(20) 
#define SENSOR_SUBSTANCE_CODE_10_INTERN          (uint16_t)(21) 
#define SENSOR_SUBSTANCE_CODE_11_INTERN          (uint16_t)(22) 
#define SENSOR_SUBSTANCE_CODE_12_INTERN          (uint16_t)(23) 
#define SENSOR_SUBSTANCE_CODE_13_INTERN          (uint16_t)(24) 
#define SENSOR_SUBSTANCE_CODE_14_INTERN          (uint16_t)(25) 
#define SENSOR_SUBSTANCE_CODE_15_INTERN          (uint16_t)(26) 
#define SENSOR_SUBSTANCE_CODE_16_INTERN          (uint16_t)(27) 

#define CALIBRATION_PRIMATY_ZERO_INTERN               (uint16_t)(28)  
#define CALIBRATION_PRIMATY_ZERO_STATUS_INTERN        (uint16_t)(29)  
#define CALIBRATION_PRIMATY_ZERO_SIGNAL_HIGH_INTERN   (uint16_t)(30)  
#define CALIBRATION_PRIMATY_ZERO_SIGNAL_LOW_INTERN    (uint16_t)(31)  

#define CALIBRATION_PRIMATY_SPAN_INTERN               (uint16_t)(32)  
#define CALIBRATION_PRIMATY_SPAN_STATUS_INTERN        (uint16_t)(33)  
#define CALIBRATION_PRIMATY_SPAN_SIGNAL_HIGH_INTERN   (uint16_t)(34)  
#define CALIBRATION_PRIMATY_SPAN_SIGNAL_LOW_INTERN    (uint16_t)(35)  





/* ------------------------ END HOLDING REGISTERS ------------------------------------*/	


/* ------------------------ Defines INPUT REGISTERS ----------------------------------*//* ------------------------ END INPUT REGISTERS --------------------------------------*/	
#define INPUT_OFFSET                   (uint16_t)30000								 
/* ------------------------Global variables----------------------------*/

#define SENSOR_PRIMARY_VALUE_HIGH       (uint16_t)(33001 - 30000) // один адрес , концентрация 
#define SENSOR_PRIMARY_VALUE_LOW        (uint16_t)(33002 - 30000) // один адрес , концентрация 
#define SENSOR_PRIMARY_STATUS           (uint16_t)(33003 - 30000) // один адрес
  
#define SENSOR_PRIMARY_VALUE_HIGH_INTERN (uint16_t)(1) // один адрес , концентрация   */
#define SENSOR_PRIMARY_VALUE_LOW_INTERN  (uint16_t)(2) // один адрес , концентрация   */
#define SENSOR_PRIMARY_STATUS_INTERN     (uint16_t)(3) 

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
