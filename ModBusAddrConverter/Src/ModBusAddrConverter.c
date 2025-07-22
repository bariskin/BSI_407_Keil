/**
  ******************************************************************************
  * @file      ModBusAddrConverter.c
  * @author    MTL  Application Team
  * @version   
  * @brief     This file provides code common functions  for modbus address converter       
  ******************************************************************************
*/
/* ------------------------Includes ----------------------------------*/
#include "ModBusAddrConverter.h"
/* ------------------------Global variables----------------------------*/

/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/

/**
  * @brief  convert modbus address, master
  * @param  MBAddress: extern modbus register address
  * @retval internal address of the Modbus SyncMan register
*/
uint16_t convertModBusAddrHolding_M(uint16_t MBAddress)
{
  uint16_t MBReg = 0x0000;
  
  switch(MBAddress)
  {   
    /* ************ FIRMWARE_VERSION ********* */   
  case DEVICE_MODEL_CODE:
    
    MBReg = DEVICE_MODEL_CODE_INTERN; 
    
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  case DEVICE_MODEL_CODE + 1:
    
    MBReg = DEVICE_MODEL_CODE_INTERN + 1; 
    
    break;   
    /* ********************* */   
  case  DEVICE_ID:
  
     MBReg = DEVICE_ID_INTERN;  
    break;
    /* ********************* */   
  case  DEVICE_ID + 1:
		
     MBReg = DEVICE_ID_INTERN + 1;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case DEVICE_ID + 2:
    
    MBReg = DEVICE_ID_INTERN + 3;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  
	case  VERSION_ID:
  
     MBReg = VERSION_ID_INTERN;  
    break;
    /* ********************* */   
  case  VERSION_ID + 1:
		
     MBReg = VERSION_ID_INTERN + 1;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case VERSION_ID + 2:
    
    MBReg = VERSION_ID_INTERN + 3;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  
	case  CALIBRATION_ZERO_POINT:
  
     MBReg = CALIBRATION_ZERO_POINT_INTERN;  
    break;
    /* ********************* */   
  case  CALIBRATION_ZERO_POINT + 1:
		
     MBReg = CALIBRATION_ZERO_POINT_INTERN + 1;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
 case  SENSOR_SCALE_MIN_HIGH:
  
     MBReg = SENSOR_SCALE_MIN_HIGH_INTERN;  
    break;
    /* ********************* */   
  case  SENSOR_SCALE_MIN_HIGH + 1:
		
     MBReg = SENSOR_SCALE_MIN_HIGH_INTERN + 1;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      

	 case  SENSOR_SCALE_MIN_HIGH + 2:
  
     MBReg = SENSOR_SCALE_MIN_HIGH_INTERN + 2;  
    break;
    /* ********************* */   
  case  SENSOR_SCALE_MIN_HIGH + 3:
		
     MBReg = SENSOR_SCALE_MIN_HIGH_INTERN + 3;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case  SENSOR_SCALE_MIN_HIGH + 4:
		
     MBReg = SENSOR_SCALE_MIN_HIGH_INTERN + 4;  
    break; 

	} 
  return MBReg; 
  
}


 uint16_t convertModBusAddrInput_M(uint16_t MBAddress)
 {
   uint16_t MBReg = 0x0000;
	 
	 
 switch(MBAddress)
  {
	 case DEVICE_STATUS:
    
    MBReg = DEVICE_STATUS_INTERN; 
    
    break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 case SENSOR_PRIMARY_VAUE_HIGH:
    
    MBReg = SENSOR_PRIMARY_VAUE_HIGH_INTERN; 
    
    break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
 case SENSOR_PRIMARY_VAUE_LOW:
    
    MBReg = SENSOR_PRIMARY_VAUE_LOW_INTERN; 
    
    break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
		 case SENSOR_PRIMARY_STATUS:
    
    MBReg = SENSOR_PRIMARY_STATUS_INTERN; 
    
    break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
	}
 
   return MBReg;
 }

 /**
  * @brief  convert modbus address, slave
  * @param  MBAddress: extern modbus register address
  * @retval internal address of the Modbus SyncMan register
*/
uint16_t convertModBusAddrHolding_S(uint16_t MBAddress)
 {
  uint16_t MBReg = 0x0000;
	return MBReg;
 }


uint16_t convertModBusAddrInput_S(uint16_t MBAddress)
 {
   uint16_t MBReg = 0x0000;
	return MBReg;
 } 
	 