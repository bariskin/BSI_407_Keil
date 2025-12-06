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
#define HOLDING_OFFSET                         (uint16_t)40000

static const uint16_t  DEVICE_MODEL_CODE           =           (uint16_t)(40400 - HOLDING_OFFSET); //DEVICE_MODEL_CODE
static const uint16_t  DEVICE_MODEL_CODE_2          =          (uint16_t)(40401 - HOLDING_OFFSET); //DEVICE_MODEL_CODE
static const uint16_t  CHANNEL_COUNT               =           (uint16_t)(40405 - HOLDING_OFFSET)	;//Channel count

/*-------------------------------------  PRIMARY SENSOR   -------------------------------   */
static const uint16_t SENSOR_SCALE_MIN_HIGH       =            (uint16_t)(41000 - HOLDING_OFFSET);//Sensor Scale Min High
static const uint16_t  SENSOR_SCALE_MIN_LOW        =           (uint16_t)(41001 - HOLDING_OFFSET);	//Sensor Scale Min Low
static const uint16_t  SENSOR_SCALE_MAX_HIGH        =          (uint16_t)(41002 - HOLDING_OFFSET);	//Sensor Scale Max High
static const uint16_t SENSOR_SCALE_MAX_LOW         =           (uint16_t)(41003 - HOLDING_OFFSET);	//Sensor Scale Max Low
static const uint16_t  SENSOR_SCALE_DIMENSTION      =          (uint16_t)(41004 - HOLDING_OFFSET);	//Sensor Scale Dimension

static const uint16_t  SENSOR_THRESHOLD_WARNIGN_HIGN    =      (uint16_t)(41010 - HOLDING_OFFSET);  //Sensor Threshold Warning High
static const uint16_t  SENSOR_THRESHOLD_WARNIGN_LOW     =      (uint16_t)(41011 - HOLDING_OFFSET);  //Sensor Threshold Warning LOW
static const uint16_t  SENSOR_THRESHOLD_ALARM_HIGH      =      (uint16_t)(41012 - HOLDING_OFFSET); //Sensor Threshold ALARM LOW
static const uint16_t  SENSOR_THRESHOLD_ALARM_LOW         =    (uint16_t)(41013 - HOLDING_OFFSET);  //Sensor Threshold ALARM LOW
static const uint16_t SENSOR_THRESHOLD_ADDITIONAL_HIGH    =    (uint16_t)(41014 - HOLDING_OFFSET);  //Sensor Threshold ADDITIONAL LOW
static const uint16_t  SENSOR_THRESHOLD_ADDITIONAL_LOW     =   (uint16_t)(41015 - HOLDING_OFFSET);  //Sensor Threshold ADDITIONAL LOW

static const uint16_t  CALIBRATION_PRIMARY_ZERO_VALUE_HIGH  =  (uint16_t)(41018 - HOLDING_OFFSET);  //Calibration Primary Zero Signal High
static const uint16_t  CALIBRATION_PRIMARY_ZERO_VALUE_LOW   =  (uint16_t)(41019 - HOLDING_OFFSET);  //Calibration Primary Zero Signal Low
static const uint16_t CALIBRATION_PRIMARY_ZERO             =   (uint16_t)(41020 - HOLDING_OFFSET) ; //Calibration Primary Zero 
static const uint16_t  CALIBRATION_PRIMARY_ZERO_STATUS      =  (uint16_t)(41021 - HOLDING_OFFSET);  //Calibration Primary Zero Status !!!!TODO!!!!

static const uint16_t CALIBRATION_PRIMARY_SPAN_VALUE_HIGH  =   (uint16_t)(41023 - HOLDING_OFFSET);  //Calibration Primary Span Value High
static const uint16_t CALIBRATION_PRIMARY_SPAN_VALUE_LOW   =   (uint16_t)(41024 - HOLDING_OFFSET);  //Calibration Primary Span Value Low
static const uint16_t  CALIBRATION_PRIMARY_SPAN             =  (uint16_t)(41025 - HOLDING_OFFSET) ; //Calibration Primary Span 
static const uint16_t  CALIBRATION_PRIMARY_SPAN_STATUS      =  (uint16_t)(41026 - HOLDING_OFFSET);  //Calibration Primary Span Status

static const uint16_t CALIBRATION_PROCESS_STATUS          =    (uint16_t)(40922 - HOLDING_OFFSET);  //CALIBRATION_PROCESS_STATUS  !!!!TODO!!!!

static const uint16_t SENSOR_SUBSTANCE_CODE_1            =     (uint16_t)(41030 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t  SENSOR_SUBSTANCE_CODE_2             =   (uint16_t)(41031 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 
static const uint16_t  SENSOR_SUBSTANCE_CODE_3             =   (uint16_t)(41032 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_4             =    (uint16_t)(41033 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_5             =    (uint16_t)(41034 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_6            =     (uint16_t)(41035 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_7            =     (uint16_t)(41036 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_8            =     (uint16_t)(41037 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_9            =     (uint16_t)(41038 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_10           =     (uint16_t)(41039 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_11            =    (uint16_t)(41040 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_12            =    (uint16_t)(41041 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_13           =     (uint16_t)(41042 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_14           =     (uint16_t)(41043 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t SENSOR_SUBSTANCE_CODE_15           =     (uint16_t)(41044 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 
static const uint16_t  SENSOR_SUBSTANCE_CODE_16           =    (uint16_t)(41045 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 


/*-------------------------------------  SECONADARY SENSOR   -------------------------------   */
static const uint16_t SENSOR_SCALE_MIN_HIGH_2          =       (uint16_t)(42000 - HOLDING_OFFSET);	//Sensor Scale Min High 2
static const uint16_t  SENSOR_SCALE_MIN_LOW_2            =     (uint16_t)(42001 - HOLDING_OFFSET);	//Sensor Scale Min Low  2
static const uint16_t  SENSOR_SCALE_MAX_HIGH_2           =     (uint16_t)(42002 - HOLDING_OFFSET);	//Sensor Scale Max High 2
static const uint16_t SENSOR_SCALE_MAX_LOW_2            =      (uint16_t)(42003 - HOLDING_OFFSET);	//Sensor Scale Max Low 2
static const uint16_t SENSOR_SCALE_DIMENSTION_2         =      (uint16_t)(42004 - HOLDING_OFFSET);	//Sensor Scale Dimension 2

static const uint16_t SENSOR_THRESHOLD_WARNIGN_HIGN_2   =      (uint16_t)(42010 - HOLDING_OFFSET) ; //Sensor Threshold Warning High 2
static const uint16_t SENSOR_THRESHOLD_WARNIGN_LOW_2    =      (uint16_t)(42011 - HOLDING_OFFSET) ; //Sensor Threshold Warning LOW 2
static const uint16_t SENSOR_THRESHOLD_ALARM_HIGH_2     =      (uint16_t)(42012 - HOLDING_OFFSET) ; //Sensor Threshold ALARM LOW 2
static const uint16_t SENSOR_THRESHOLD_ALARM_LOW_2       =     (uint16_t)(42013 - HOLDING_OFFSET);  //Sensor Threshold ALARM LOW 2
static const uint16_t SENSOR_THRESHOLD_ADDITIONAL_HIGH_2   =   (uint16_t)(42014 - HOLDING_OFFSET) ; //Sensor Threshold ADDITIONAL LOW 2
static const uint16_t SENSOR_THRESHOLD_ADDITIONAL_LOW_2    =   (uint16_t)(42015 - HOLDING_OFFSET) ; //Sensor Threshold ADDITIONAL LOW 2

static const uint16_t CALIBRATION_SECONDARY_ZERO_VALUE_HIGH  = (uint16_t)(42018 - HOLDING_OFFSET) ; //Calibration Secondary Zero Signal High
static const uint16_t CALIBRATION_SECONDARY_ZERO_VALUE_LOW   = (uint16_t)(42019 - HOLDING_OFFSET) ;//Calibration Secondary  Zero Signal Low

static const uint16_t CALIBRATION_SECONDARY_SPAN_VALUE_HIGH  = (uint16_t)(42023 - HOLDING_OFFSET) ; //Calibration Secondary  Span Value High
static const uint16_t CALIBRATION_SECONDARY_SPAN_VALUE_LOW   = (uint16_t)(42024 - HOLDING_OFFSET) ; //Calibration Secondary  Span Value Low

static const uint16_t SENSOR_SUBSTANCE_CODE_1_2          =     (uint16_t)(42030 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 2
static const uint16_t SENSOR_SUBSTANCE_CODE_2_2           =    (uint16_t)(42031 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 2
static const uint16_t SENSOR_SUBSTANCE_CODE_3_2           =    (uint16_t)(42032 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_4_2           =   (uint16_t)(42033 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_5_2           =    (uint16_t)(42034 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 2
static const uint16_t SENSOR_SUBSTANCE_CODE_6_2           =     (uint16_t)(42035 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 2
static const uint16_t SENSOR_SUBSTANCE_CODE_7_2            =    (uint16_t)(42036 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 2
static const uint16_t SENSOR_SUBSTANCE_CODE_8_2            =    (uint16_t)(42037 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_9_2            =    (uint16_t)(42038 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_10_2           =    (uint16_t)(42039 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_11_2           =    (uint16_t)(42040 - HOLDING_OFFSET) ; //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_12_2           =    (uint16_t)(42041 - HOLDING_OFFSET)  ;//SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_13_2           =    (uint16_t)(42042 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_14_2           =    (uint16_t)(42043 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_15_2            =   (uint16_t)(42044 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 2
static const uint16_t  SENSOR_SUBSTANCE_CODE_16_2            =   (uint16_t)(42045 - HOLDING_OFFSET);  //SENSOR_SUBSTANCE_CODE 2

/* ------------------------ Intern  for converter addr------------------------------------*/

static const uint16_t  DEVICE_MODEL_CODE_INTERN              =          (uint16_t)(0) ;
static const uint16_t  DEVICE_MODEL_CODE_INTERN_2              =        (uint16_t)(1) ;
static const uint16_t  SENSOR_SCALE_MAX_HIGH_INTERN           =         (uint16_t)(2) ;
static const uint16_t  SENSOR_SCALE_MAX_LOW_INTERN            =         (uint16_t)(3) ;
static const uint16_t  SENSOR_SCALE_DIMENSTION_INTERN         =         (uint16_t)(4) ;

static const uint16_t  SENSOR_THRESHOLD_WARNIGN_HIGN_INTERN     =       (uint16_t)(5) ;
static const uint16_t  SENSOR_THRESHOLD_WARNIGN_LOW_INTERN        =     (uint16_t)(6) ;
static const uint16_t  SENSOR_THRESHOLD_ALARM_HIGH_INTERN          =    (uint16_t)(7) ;
static const uint16_t SENSOR_THRESHOLD_ALARM_LOW_INTERN           =    (uint16_t)(8) ;
static const uint16_t  SENSOR_THRESHOLD_ADDITIONAL_HIGH_INTERN     =    (uint16_t)(9) ;
static const uint16_t SENSOR_THRESHOLD_ADDITIONAL_LOW_INTERN      =    (uint16_t)(10) ;

static const uint16_t  CALIBRATION_SECONDARY_ZERO_VALUE_HIGH_INTERN  =  (uint16_t)(11) ;
static const uint16_t CALIBRATION_SECONDARY_ZERO_VALUE_LOW_INTERN   = (uint16_t)(12) ;
static const uint16_t CALIBRATION_SECONDARY_ZERO_INTERN            =   (uint16_t)(13) ;
static const uint16_t  CALIBRATION_SECONDARY_ZERO_STATUS_INTERN     =   (uint16_t)(14) ;


static const uint16_t SENSOR_SUBSTANCE_CODE_1_INTERN              =    (uint16_t)(15) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_2_INTERN             =     (uint16_t)(16) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_3_INTERN              =    (uint16_t)(17) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_4_INTERN               =   (uint16_t)(18) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_5_INTERN               =   (uint16_t)(19) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_6_INTERN              =	  (uint16_t)(20) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_7_INTERN              =    (uint16_t)(21) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_8_INTERN              =    (uint16_t)(22) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_9_INTERN              =    (uint16_t)(23) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_10_INTERN             =    (uint16_t)(24) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_11_INTERN             =    (uint16_t)(25) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_12_INTERN             =    (uint16_t)(26) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_13_INTERN             =    (uint16_t)(27) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_14_INTERN             =    (uint16_t)(28) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_15_INTERN              =   (uint16_t)(29) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_16_INTERN              =   (uint16_t)(30) ;


/*-------------------------------------  SECONADARY SENSOR   -------------------------------   */
static const uint16_t  SENSOR_SCALE_MIN_HIGH_2_INTERN           =       (uint16_t)(31) ;   
static const uint16_t SENSOR_SCALE_MIN_LOW_2_INTERN            =       (uint16_t)(32) ;
static const uint16_t  SENSOR_SCALE_MAX_HIGH_2_INTERN           =       (uint16_t)(33) ;   
static const uint16_t SENSOR_SCALE_MAX_LOW_2_INTERN            =       (uint16_t)(34) ;    
static const uint16_t SENSOR_SCALE_DIMENSTION_2_INTERN         =       (uint16_t)(35) ; 

static const uint16_t  SENSOR_THRESHOLD_WARNIGN_HIGN_2_INTERN      =    (uint16_t)(36) ;
static const uint16_t SENSOR_THRESHOLD_WARNIGN_LOW_2_INTERN       =    (uint16_t)(37) ; 
static const uint16_t SENSOR_THRESHOLD_ALARM_HIGH_2_INTERN         =   (uint16_t)(38) ;
static const uint16_t  SENSOR_THRESHOLD_ALARM_LOW_2_INTERN          =   (uint16_t)(39) ; 
static const uint16_t SENSOR_THRESHOLD_ADDITIONAL_HIGH_2_INTERN     =  (uint16_t)(40) ;
static const uint16_t SENSOR_THRESHOLD_ADDITIONAL_LOW_2_INTERN      =  (uint16_t)(41) ;

static const uint16_t  SENSOR_SUBSTANCE_CODE_1_2_INTERN            =    (uint16_t)(42) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_2_2_INTERN            =    (uint16_t)(43) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_3_2_INTERN            =    (uint16_t)(44) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_4_2_INTERN             =   (uint16_t)(45) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_5_2_INTERN             =   (uint16_t)(46) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_6_2_INTERN            =    (uint16_t)(47); 
static const uint16_t SENSOR_SUBSTANCE_CODE_7_2_INTERN            =    (uint16_t)(48) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_8_2_INTERN             =   (uint16_t)(49) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_9_2_INTERN             =   (uint16_t)(50) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_10_2_INTERN            =   (uint16_t)(51) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_11_2_INTERN            =   (uint16_t)(52) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_12_2_INTERN            =   (uint16_t)(53) ;
static const uint16_t SENSOR_SUBSTANCE_CODE_13_2_INTERN            =   (uint16_t)(54) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_14_2_INTERN            =   (uint16_t)(55) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_15_2_INTERN            =   (uint16_t)(56) ;
static const uint16_t  SENSOR_SUBSTANCE_CODE_16_2_INTERN           =    (uint16_t)(57) ;

static const uint16_t  CALIBRATION_PROCESS_STATUS_INTERN           =    (uint16_t)(58) ;
/* ------------------------ END HOLDING REGISTERS ------------------------------------*/	

/* ------------------------ Defines INPUT REGISTERS ----------------------------------*/
#define INPUT_OFFSET                    (uint16_t)30000								 
/* ------------------------Global variables----------------------------*/

static const uint16_t SENSOR_PRIMARY_VALUE_HIGH             =  (uint16_t)(33000 - INPUT_OFFSET); // один адрес , концентрация 
static const uint16_t  SENSOR_PRIMARY_VALUE_LOW             =  (uint16_t)(33001 - INPUT_OFFSET); // один адрес , концентрация 
static const uint16_t SENSOR_PRIMARY_STATUS                 =  (uint16_t)(33002 - INPUT_OFFSET); // один адрес

static const uint16_t  SENSOR_SECONDARY_VALUE_HIGH          =  (uint16_t)(33003 - INPUT_OFFSET); // один адрес , концентрация 2
static const uint16_t SENSOR_SECONDARY_VALUE_LOW            =  (uint16_t)(33004 - INPUT_OFFSET);// один адрес , концентрация 2
static const uint16_t  SENSOR_SECONDARY_STATUS              =  (uint16_t)(33005 - INPUT_OFFSET); // один адрес 2
 
static const uint16_t  SENSOR_PRIMARY_VALUE_HIGH_INTERN     =  (uint16_t)(0); // один адрес , концентрация   */
static const uint16_t  SENSOR_PRIMARY_VALUE_LOW_INTERN      =  (uint16_t)(1); // один адрес , концентрация   */
static const uint16_t  SENSOR_PRIMARY_STATUS_INTERN         =  (uint16_t)(2); 
static const uint16_t  SENSOR_SECONDARY_VALUE_HIGH_INTERN   =  (uint16_t)(3); // один адрес , концентрация   */
static const uint16_t  SENSOR_SECONDARY_VALUE_LOW_INTERN    =  (uint16_t)(4); // один адрес , концентрация   */
static const uint16_t SENSOR_SECONDARY_STATUS_INTERN        =  (uint16_t)(5); 
 


static const uint16_t SENSOR_PRIMARY_VALUE_HIGH_2             =  (uint16_t)(33000 - INPUT_OFFSET); // один адрес , концентрация 
static const uint16_t  SENSOR_PRIMARY_VALUE_LOW_2             =  (uint16_t)(33001 - INPUT_OFFSET); // один адрес , концентрация 
static const uint16_t SENSOR_PRIMARY_STATUS_2                 =  (uint16_t)(33002 - INPUT_OFFSET); // один адрес

static const uint16_t  SENSOR_SECONDARY_VALUE_HIGH_2          =  (uint16_t)(33003 - INPUT_OFFSET); // один адрес , концентрация 2
static const uint16_t SENSOR_SECONDARY_VALUE_LOW_2            =  (uint16_t)(33004 - INPUT_OFFSET);// один адрес , концентрация 2
static const uint16_t  SENSOR_SECONDARY_STATUS_2              =  (uint16_t)(33005 - INPUT_OFFSET); // один адрес 2
 
static const uint16_t  SENSOR_PRIMARY_VALUE_HIGH_INTERN_2     =  (uint16_t)(0); // один адрес , концентрация   */
static const uint16_t  SENSOR_PRIMARY_VALUE_LOW_INTERN_2      =  (uint16_t)(1); // один адрес , концентрация   */
static const uint16_t  SENSOR_PRIMARY_STATUS_INTERN_2         =  (uint16_t)(2); 
static const uint16_t  SENSOR_SECONDARY_VALUE_HIGH_INTERN_2   =  (uint16_t)(3); // один адрес , концентрация   */
static const uint16_t  SENSOR_SECONDARY_VALUE_LOW_INTERN_2    =  (uint16_t)(4); // один адрес , концентрация   */
static const uint16_t SENSOR_SECONDARY_STATUS_INTERN_2        =  (uint16_t)(5); 




 /* ------------------------ END INPUT REGISTERS --------------------------------------*/
 
 uint16_t convertModBusAddrHolding_M(uint16_t MBAddress);
 uint16_t convertModBusAddrInput_M(uint16_t MBAddress);
 uint16_t convertModBusAddrHolding_S(uint16_t MBAddress);
 uint16_t convertModBusAddrInput_S(uint16_t MBAddress);
 uint16_t convertModBusAddrInput_M2(uint16_t MBAddress);
  /**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif  /* _MODBUS_ADDR_CONVERTER */
