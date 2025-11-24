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
  case DEVICE_MODEL_CODE: 
    MBReg = DEVICE_MODEL_CODE_INTERN;    
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  case DEVICE_MODEL_CODE_2:
    MBReg = DEVICE_MODEL_CODE_INTERN_2; 
    break;   
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  case  SENSOR_SCALE_MAX_HIGH:
     MBReg = SENSOR_SCALE_MAX_HIGH_INTERN;  
    break;
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  case  SENSOR_SCALE_MAX_LOW:
     MBReg = SENSOR_SCALE_MAX_LOW_INTERN;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case SENSOR_SCALE_DIMENSTION:
    MBReg = SENSOR_SCALE_DIMENSTION_INTERN;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
	case  SENSOR_THRESHOLD_WARNIGN_HIGN:
     MBReg = SENSOR_THRESHOLD_WARNIGN_HIGN_INTERN;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  case  SENSOR_THRESHOLD_WARNIGN_LOW:	
     MBReg = SENSOR_THRESHOLD_WARNIGN_LOW_INTERN ;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case SENSOR_THRESHOLD_ALARM_HIGH :
    MBReg = SENSOR_THRESHOLD_ALARM_HIGH_INTERN ;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/           
	case  SENSOR_THRESHOLD_ALARM_LOW:
     MBReg = SENSOR_THRESHOLD_ALARM_LOW_INTERN;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
  case SENSOR_THRESHOLD_ADDITIONAL_HIGH :
    MBReg = SENSOR_THRESHOLD_ADDITIONAL_HIGH_INTERN ;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/           
	case  SENSOR_THRESHOLD_ADDITIONAL_LOW:
     MBReg = SENSOR_THRESHOLD_ADDITIONAL_LOW_INTERN;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  case  SENSOR_SUBSTANCE_CODE_1 :
     MBReg = SENSOR_SUBSTANCE_CODE_1_INTERN ;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/        
 case  SENSOR_SUBSTANCE_CODE_2:
     MBReg = SENSOR_SUBSTANCE_CODE_2_INTERN;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/     
  case  SENSOR_SUBSTANCE_CODE_3 :
     MBReg = SENSOR_SUBSTANCE_CODE_3_INTERN ;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
	 case  SENSOR_SUBSTANCE_CODE_4:
     MBReg = SENSOR_SUBSTANCE_CODE_4_INTERN;  
    break;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/     
  case  SENSOR_SUBSTANCE_CODE_5:
     MBReg = SENSOR_SUBSTANCE_CODE_5_INTERN;  
    break;    
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case  SENSOR_SUBSTANCE_CODE_6:
     MBReg = SENSOR_SUBSTANCE_CODE_6_INTERN;  
    break; 
	    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case  SENSOR_SUBSTANCE_CODE_7:
     MBReg = SENSOR_SUBSTANCE_CODE_7_INTERN ;  
    break;
		    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case  SENSOR_SUBSTANCE_CODE_8:
     MBReg = SENSOR_SUBSTANCE_CODE_8_INTERN ;  
    break; 
		    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case  SENSOR_SUBSTANCE_CODE_9:
     MBReg = SENSOR_SUBSTANCE_CODE_9_INTERN;  
    break; 
		    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case  SENSOR_SUBSTANCE_CODE_10:
     MBReg = SENSOR_SUBSTANCE_CODE_10_INTERN ;  
    break; 
		    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case  SENSOR_SUBSTANCE_CODE_11:
     MBReg = SENSOR_SUBSTANCE_CODE_11_INTERN;  
    break; 
		    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  case  SENSOR_SUBSTANCE_CODE_12:
     MBReg = SENSOR_SUBSTANCE_CODE_12_INTERN;  
    break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	case  SENSOR_SUBSTANCE_CODE_13:	
     MBReg = SENSOR_SUBSTANCE_CODE_13_INTERN;  
    break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	case  SENSOR_SUBSTANCE_CODE_14:
     MBReg = SENSOR_SUBSTANCE_CODE_14_INTERN;  
    break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	case  SENSOR_SUBSTANCE_CODE_15:	
     MBReg = SENSOR_SUBSTANCE_CODE_15_INTERN;  
    break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	case  SENSOR_SUBSTANCE_CODE_16:		
     MBReg = SENSOR_SUBSTANCE_CODE_16_INTERN;  
    break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
//	case	CALIBRATION_PRIMARY_ZERO:	
//		MBReg = CALIBRATION_PRIMARY_ZERO_INTERN;
//    break; 		
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
//	case CALIBRATION_PRIMARY_ZERO_STATUS: 	
 //   MBReg = CALIBRATION_PRIMARY_ZERO_STATUS_INTERN;
	//	break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	//case	CALIBRATION_PRIMATY_ZERO_VALUE_HIGH:		
//		MBReg = CALIBRATION_PRIMATY_ZERO_SIGNAL_HIGH_INTERN;
	//	break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	//case	CALIBRATION_PRIMATY_ZERO_VALUE_LOW: 	
	//	MBReg = CALIBRATION_PRIMATY_ZERO_SIGNAL_LOW_INTERN;
	//  break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
//	case	CALIBRATION_PRIMARY_SPAN:	
//		MBReg = CALIBRATION_PRIMARY_SPAN;
//    break; 		
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	//case	CALIBRATION_PRIMARY_SPAN_STATUS: 	
	//	MBReg = CALIBRATION_PRIMARY_SPAN_STATUS_INTERN;
 //  break;     
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
//	case	CALIBRATION_PRIMARY_SPAN_VALUE_HIGH:
	//	MBReg = CALIBRATION_PRIMARY_SPAN_VALUE_HIGH_INTERN;
//		break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
//	case	CALIBRATION_PRIMARY_SPAN_VALUE_LOW: 	
	//	MBReg = CALIBRATION_PRIMARY_SPAN_VALUE_LOW_INTERN;
//		break; 
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	
//	case CALIBRATION_PRIMARY_ZERO_VALUE_HIGH:
		
 //   MBReg =  CALIBRATION_PRIMARY_ZERO_VALUE_HIGH_INTERN;
//	break;	

//  case CALIBRATION_PRIMARY_ZERO_VALUE_LOW:
		
   // MBReg =  CALIBRATION_PRIMARY_ZERO_VALUE_LOW_INTERN;
//	break;	
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	case     CALIBRATION_PROCESS_STATUS + 1: // так работает 
		MBReg =   CALIBRATION_PROCESS_STATUS_INTERN + 1;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   	
	case     	SENSOR_SCALE_MIN_HIGH_2:  
		MBReg = 	SENSOR_SCALE_MIN_HIGH_2_INTERN; 
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   	
	case     	SENSOR_SCALE_MIN_LOW_2:
		MBReg = SENSOR_SCALE_MIN_LOW_2_INTERN;  
	break;	
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   	
	case     	SENSOR_SCALE_MAX_HIGH_2:
		MBReg =  SENSOR_SCALE_MAX_HIGH_2_INTERN; 
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	case     	SENSOR_SCALE_MAX_LOW_2:
		MBReg =  SENSOR_SCALE_MAX_LOW_2_INTERN;  
	break;	
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   	
	case     	SENSOR_SCALE_DIMENSTION_2:
    MBReg = SENSOR_SCALE_DIMENSTION_2_INTERN;		
	break;
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	case     	SENSOR_THRESHOLD_WARNIGN_HIGN_2:
		MBReg = SENSOR_THRESHOLD_WARNIGN_HIGN_2_INTERN;   
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/   
	case     	SENSOR_THRESHOLD_WARNIGN_LOW_2:
		MBReg =  SENSOR_THRESHOLD_WARNIGN_LOW_2_INTERN; 
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
	case     	SENSOR_THRESHOLD_ALARM_HIGH_2:
		MBReg = SENSOR_THRESHOLD_ALARM_HIGH_2_INTERN; 
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_THRESHOLD_ALARM_LOW_2:
		MBReg = SENSOR_THRESHOLD_ALARM_LOW_2_INTERN; 
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_THRESHOLD_ADDITIONAL_HIGH_2:
		MBReg = SENSOR_THRESHOLD_ADDITIONAL_HIGH_2_INTERN; 
	break;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_THRESHOLD_ADDITIONAL_LOW_2: 
    MBReg = SENSOR_THRESHOLD_ADDITIONAL_LOW_2_INTERN;  		
	break;	
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
	case     	SENSOR_SUBSTANCE_CODE_1_2:
		MBReg = SENSOR_SUBSTANCE_CODE_1_2_INTERN;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_2_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_2_2_INTERN;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_3_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_3_2_INTERN;
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
	case     	SENSOR_SUBSTANCE_CODE_4_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_4_2_INTERN;
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
	case     	SENSOR_SUBSTANCE_CODE_5_2:
    MBReg =  SENSOR_SUBSTANCE_CODE_5_2_INTERN;		
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
	case     	SENSOR_SUBSTANCE_CODE_6_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_6_2_INTERN;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_7_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_7_2_INTERN;
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
	case     	SENSOR_SUBSTANCE_CODE_8_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_8_2_INTERN;
	break;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_9_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_9_2_INTERN;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_10_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_10_2_INTERN;
	break;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_11_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_11_2_INTERN;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_12_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_12_2_INTERN;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_13_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_13_2_INTERN;
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
	case     	SENSOR_SUBSTANCE_CODE_14_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_14_2_INTERN;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_15_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_15_2_INTERN;
	break;
 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  	
	case     	SENSOR_SUBSTANCE_CODE_16_2:
		MBReg =  SENSOR_SUBSTANCE_CODE_16_2_INTERN;
	break;			
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
	}  
  return MBReg;  
}


 uint16_t convertModBusAddrInput_M(uint16_t MBAddress)
 {
   uint16_t MBReg = 0x0000;
	 
 switch(MBAddress)
  {
	 case SENSOR_PRIMARY_VALUE_HIGH:
    
    MBReg = SENSOR_PRIMARY_VALUE_HIGH_INTERN; 
    break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 case SENSOR_PRIMARY_VALUE_LOW:
    
    MBReg = SENSOR_PRIMARY_VALUE_LOW_INTERN;  
    break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
 case SENSOR_PRIMARY_STATUS:
    
    MBReg = SENSOR_PRIMARY_STATUS_INTERN; 
    break;
 	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
 case SENSOR_SECONDARY_VALUE_HIGH:
	 
	  MBReg = SENSOR_SECONDARY_VALUE_HIGH_INTERN;
		break;
 	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
 case SENSOR_SECONDARY_VALUE_LOW:
	 
	 MBReg = SENSOR_SECONDARY_VALUE_LOW_INTERN;
		break;
 	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
 case SENSOR_SECONDARY_STATUS:
	 
	  MBReg = SENSOR_SECONDARY_STATUS_INTERN;
		break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	}
   return MBReg;
 }

 
 uint16_t convertModBusAddrInput_M2(uint16_t MBAddress)
 {
   uint16_t MBReg = 0x0000;
	 
 switch(MBAddress)
  {
	 case SENSOR_PRIMARY_VALUE_HIGH:
    
    MBReg = SENSOR_PRIMARY_VALUE_HIGH_INTERN; 
    break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 case SENSOR_PRIMARY_VALUE_LOW:
    
    MBReg = SENSOR_PRIMARY_VALUE_LOW_INTERN;  
    break;
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
// case SENSOR_PRIMARY_STATUS:
    
//    MBReg = SENSOR_PRIMARY_STATUS_INTERN; 
//    break;
 	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
// case SENSOR_SECONDARY_VALUE_HIGH:
	 
	//  MBReg = SENSOR_SECONDARY_VALUE_HIGH_INTERN;
//		break;
 	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
// case SENSOR_SECONDARY_VALUE_LOW:
//	 
//	 MBReg = SENSOR_SECONDARY_VALUE_LOW_INTERN;
//		break;
// 	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
// case SENSOR_SECONDARY_STATUS:
//	 
//	  MBReg = SENSOR_SECONDARY_STATUS_INTERN;
//		break;
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
 