/**
******************************************************************************
* @file      HoldingRegisterHandler.c
* @author    @OnWert
* @version   
* @brief     This file provides common functions for the  holding register handler   
*/       
/* ------------------------Includes ----------------------------------*/
#include "HoldingRegisterSlaveHandler.h"
#include "user_mb_app.h"
/* ------------------------External variables -------------------------*/
extern uint16_t   usSRegHoldBuf[S_REG_HOLDING_NREGS]; 

/* ------------------------Global variables----------------------------*/

/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/
	void HoldingRegisterFromModbusStack(uint16_t MBregIdx, uint16_t RegValue)
	{
		switch(MBregIdx)
		{ 
			/* ************ baud rate************* */
			case HOLDING_REGISTER_IDX_1 :
				
		
			break;
			/* ************* parity ************ */
			case HOLDING_REGISTER_IDX_2: 
				
		
			 break;
			/* ************* stop bits************ */
			case HOLDING_REGISTER_IDX_3: 
				
					 
			break;
			/* ************* FW ID release ******* */
			case HOLDING_REGISTER_IDX_4:
				
		
			 break;
			/* ************ factory number ********* */
			case HOLDING_REGISTER_IDX_5: 
				
			 break;
			/* ********* SLAVE ID (modbus addr) **** */
			case HOLDING_REGISTER_IDX_6: 
				
		
			 break;	
			/* ************ DO************* */
			case HOLDING_REGISTER_IDX_7:
		
			break;
			/* ************ DI************* */
			case HOLDING_REGISTER_IDX_8: 
			
			break;
		}
		
	}
	
	
 uint16_t WriteParamToModbusStack(uint16_t MBregIdx)
  {
		uint16_t OutputValue = 0x0000;
		
		switch(MBregIdx)
      { 
     			
			 case HOLDING_REGISTER_IDX_1 :
				
				 break;
	
			 case HOLDING_REGISTER_IDX_2 :
				 
				 break;
			
			 case HOLDING_REGISTER_IDX_3 :
				
				 break;
			 
			 case HOLDING_REGISTER_IDX_4 :
	
				 break;
			 /* ************ factory number ********* */
			 case HOLDING_REGISTER_IDX_5 :

				 break;
			 /* ********* SLAVE ID (modbus addr) **** */
			 case HOLDING_REGISTER_IDX_6 :
				
				 break;
			 /* ************ DO************* */
			 case HOLDING_REGISTER_IDX_7 :
				
				 break;
		/* ************ DI************* */	 
			case HOLDING_REGISTER_IDX_8 :
	
				break;
			}
			
		return	OutputValue;
	}
  






					



/************************ (C) COPYRIGHT @OnWert *****END OF FILE****/



