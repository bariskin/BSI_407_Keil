/**
******************************************************************************
* @file      HoldingRegisterHandler.c
* @author    @OnWert
* @version   
* @brief     This file provides common functions for the  holding register handler   
*/       
/* ------------------------Includes ----------------------------------*/
#include "HoldingRegisterSlaveHandler.h"
#include "UARTSlaveSettings.h"
#include "user_mb_app.h"
#include "cmsis_os.h"
#include "task.h"

/* ------------------------External variables -------------------------*/

extern uint16_t holdingRegsPart1[MAX_MODBUS_REGS_PART];  // Адреса 1-120

/* ------------------------Global variables----------------------------*/
volatile uint32_t  MB_BaudRateValue = 0x00000000;
volatile uint32_t  MB_ParityValue   = 0x00000000;
volatile uint32_t  MB_StopBitsValue = 0x00000000;
volatile uint32_t  MB_AddresseValue = 0x00000000;
/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/
	void HoldingRegisterFromModbusStack(uint16_t MBregIdx, uint16_t RegValue)
	{
		switch(MBregIdx)
		{ 
			/* ************ baud rate************* */
			case HOLDING_REGISTER_IDX_1 :
				
				if(RegValue >= 1  && RegValue <= 6 )	
				{
			  	MB_BaudRateValue =  getBaudrate(RegValue);
				}
		    else
        {
			    holdingRegsPart1[HOLDING_REGISTER_IDX_1] = getBaudRateId(MB_BaudRateValue);
			  }			
		  osDelay(1);
			break;
			/* ************* parity ************ */
			case HOLDING_REGISTER_IDX_2:
				
			if((int8_t)RegValue >= (int8_t)0  && (uint8_t)RegValue <= 2 )
		   {
			  MB_ParityValue = getParity(RegValue);
				
		   }
			else
			 {
         holdingRegsPart1[HOLDING_REGISTER_IDX_2] = getParityId(MB_ParityValue); 
			 }	
			 
		   osDelay(1);
			 break;
			/* ************* stop bits************ */
			case HOLDING_REGISTER_IDX_3: 
				
			 if(RegValue >= 1  && RegValue <= 2 )
		    {
			   MB_StopBitsValue = getStopBits(RegValue); 
		    }
			 else
			  {
         holdingRegsPart1[HOLDING_REGISTER_IDX_3] = getStopBitsId(MB_StopBitsValue); 
			  }
			  
        osDelay(1);				
			 break;
			/* ********* SLAVE ID (modbus addr) **** */ 
			 case HOLDING_REGISTER_IDX_4: 
				 
			 if(RegValue >= 1  && RegValue <= 247 )
			   { 
			    MB_AddresseValue = RegValue;
		    
				  holdingRegsPart1[HOLDING_REGISTER_IDX_4] = MB_AddresseValue;
		     } 
			  else
			  {
			   holdingRegsPart1[HOLDING_REGISTER_IDX_4] =  MB_AddresseValue;
			  } 
		   osDelay(1);				
			 break;
		}
		
	}
	
 uint16_t WriteParamToModbusStack(uint16_t MBregIdx)
  {
		uint16_t OutputValue = 0x0000;
		
		switch(MBregIdx)
      { 		
     		/* ************ baud rate************* */			
			 case HOLDING_REGISTER_IDX_1 :
				 
				 OutputValue =  holdingRegsPart1[HOLDING_REGISTER_IDX_1];
			   osDelay(1);
				 break;
			 /* ************* parity ************ */
			 case HOLDING_REGISTER_IDX_2 :
				 
				 OutputValue =  holdingRegsPart1[HOLDING_REGISTER_IDX_2];
			   osDelay(1);
				 break;
			 
			 /* ************* stop bits************ */
			 case HOLDING_REGISTER_IDX_3 :
				
			   OutputValue =  holdingRegsPart1[HOLDING_REGISTER_IDX_3];
			   osDelay(1);
				 break;		 
			 /* ********* SLAVE ID (modbus addr) **** */ 
			 case HOLDING_REGISTER_IDX_4 :
				
			   OutputValue =  holdingRegsPart1[HOLDING_REGISTER_IDX_4];
			   osDelay(1);
				 break;	
			}		
		return	OutputValue;
	}
 
/************************ (C) COPYRIGHT @OnWert *****END OF FILE****/



