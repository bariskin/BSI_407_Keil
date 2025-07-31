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
#include "UARTSlaveSettings.h"
#include "user_mb_app.h"
#include "cmsis_os.h"
#include "task.h"
#include "FlashDriver.h"
#include "bsp.h"
/* ------------------------External variables -------------------------*/

extern uint16_t holdingRegsPart1[MAX_MODBUS_SLAVE_REGS_PART];  // Àäðåñà 1-120
extern volatile UART_Settings_t UartSlaveSetting; 
extern volatile ModBusAddr_t  ModBusAddr; 
extern volatile ModBusAddr_t  ModBusAddrDefault;
extern volatile TimeStepReadingSensores_t TimeStep;
extern volatile TimeStepReadingSensores_t TimeStepDefault;

//extern osThreadId InputHandlerHandle;
extern osThreadId SlaveEventTaskHandle;
/* ------------------------Global variables----------------------------*/
volatile uint32_t  MB_BaudRateValue = 0x00000000;
volatile uint32_t  MB_ParityValue   = 0x00000000;
volatile uint32_t  MB_StopBitsValue = 0x00000000;
volatile uint32_t  MB_AddresseValue = 0x00000000;

volatile uint16_t  timeStep = (uint16_t)TIME_STEP_MIN_2; // ms

/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/
	void HoldingRegisterFromModbusSlaveStack(uint16_t MBregIdx, uint16_t RegValue)
	{
		switch(MBregIdx)
		{ 
			/* ************ baud rate************* */
			case HOLDING_REGISTER_SLAVE_IDX_1 :
				
				if(RegValue >= 1  && RegValue <= 6 )	
				{
			  	MB_BaudRateValue =  getBaudrate(RegValue);
					xTaskNotify( SlaveEventTaskHandle , HOLDING_REGISTER_SLAVE_IDX_1, eSetValueWithOverwrite); 
				}
		    else
        {
			    holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_1] = getBaudRateId(MB_BaudRateValue);
			  }			
		  osDelay(1);
			break;
			/* ************* parity ************ */
			case HOLDING_REGISTER_SLAVE_IDX_2:
				
			if((int8_t)RegValue >= (int8_t)0  && (uint8_t)RegValue <= 2 )
		   {
			  MB_ParityValue = getParity(RegValue);
				xTaskNotify( SlaveEventTaskHandle , HOLDING_REGISTER_SLAVE_IDX_2, eSetValueWithOverwrite); 	
		   }
			else
			 {
         holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_2] = getParityId(MB_ParityValue); 
			 }	
			 
		   osDelay(1);
			 break;
			/* ************* stop bits************ */
			case HOLDING_REGISTER_SLAVE_IDX_3: 
				
			 if(RegValue >= 1  && RegValue <= 2 )
		    {
			   MB_StopBitsValue = getStopBits(RegValue);
         xTaskNotify( SlaveEventTaskHandle , HOLDING_REGISTER_SLAVE_IDX_3, eSetValueWithOverwrite); 
		    }					 
			 else
			  {
         holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_3] = getStopBitsId(MB_StopBitsValue); 
			  } 
        osDelay(1);				
			 break;
			/* ********* SLAVE ID (modbus addr) **** */ 
			 case HOLDING_REGISTER_SLAVE_IDX_4: 
				 
			 if(RegValue >= 1  && RegValue <= 247 )
			   { 
			    MB_AddresseValue = RegValue;
		    
					xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_4, eSetValueWithOverwrite); 
				  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_4] = MB_AddresseValue;
		     } 
			  else
			  {
			   holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_4] =  MB_AddresseValue;
			  } 
		   osDelay(1);				
			 break;
		 case HOLDING_REGISTER_SLAVE_IDX_5: 

		 
			 if(RegValue >= TIME_STEP_MIN_2  && RegValue <= 2000)
			   { 	 
					 timeStep = RegValue; 
					 xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_5, eSetValueWithOverwrite); 
				 }
				 
				else
			  {
			    holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_5] =  timeStep;
			  } 			 
		 osDelay(1);				
			 break;
						
		}
		
	}
	
 uint16_t WriteParamToModbusSlaveStack(uint16_t MBregIdx)
  {
		uint16_t OutputValue = 0x0000;
		
		switch(MBregIdx)
      { 		
     		/* ************ baud rate************* */			
			 case HOLDING_REGISTER_SLAVE_IDX_1 :
				 
				 OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_1];
			   osDelay(1);
				 break;
			 /* ************* parity ************ */
			 case HOLDING_REGISTER_SLAVE_IDX_2 :
				 
				 OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_2];
			   osDelay(1);
				 break;
			 
			 /* ************* stop bits************ */
			 case HOLDING_REGISTER_SLAVE_IDX_3 :
				
			   OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_3];
			   osDelay(1);
				 break;		 
			 /* ********* SLAVE ID (modbus addr) **** */ 
			 case HOLDING_REGISTER_SLAVE_IDX_4 :
				
			   OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_4];
			   osDelay(1);
				 break;
			 /* ********* the time step for reading the sensor **** */   
			 case HOLDING_REGISTER_SLAVE_IDX_5 :
				
			   OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_5];
			   osDelay(1);
				 break;
			 
			}		
		return	OutputValue;
	}
 
	
  void ModBusSlaveEventHoldingRegHandler(void)
	 {
		 	 uint32_t ulNotifiedValue = 0 ;
		
		 xTaskNotifyWait
				(             
				 0x00,             /* Don’t clear any notification bits on entry. */
				 0xFFFFFFFFUL,     /* Reset the notification value to 0 on exit. */
				 &ulNotifiedValue, /* Notified value pass out in ulNotifiedValue. */                      
				 portMAX_DELAY     /* Block indefinitely. */
					 );
		 
		 		if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_1)      /* baud rate */
			  	{ 
			  		eMBDisable( );
						
						osDelay(1);
				
				  	UartSlaveSetting.BaudRateID = 	getBaudRateId(MB_BaudRateValue);
					
					  Flash_Write_Data(FLASH_SETTING_UART,(uint32_t *)&UartSlaveSetting, 5);
		
					  osDelay(10);
					 
            NVIC_SystemReset(); 
				 }	

         else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_2) /* parity */ 

             {
						   eMBDisable( );
							 
							 	osDelay(1);
				
				     	 UartSlaveSetting.ParityID = getParityId(MB_ParityValue);
							 
							 Flash_Write_Data(FLASH_SETTING_UART,(uint32_t *)&UartSlaveSetting, 5);
							 
							 osDelay(10);
					
					     NVIC_SystemReset(); 
						 }	

         else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_3)   /* stop bits */

             {
						  	eMBDisable( );
							 	osDelay(1);
				
					      UartSlaveSetting.StopBitsID = getStopBitsId(MB_StopBitsValue);
							 
							  Flash_Write_Data(FLASH_SETTING_UART,(uint32_t *)&UartSlaveSetting, 5);
							 
							  osDelay(10);
					
					      NVIC_SystemReset(); 
						 }
          else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_4) /*  SLAVE ID (modbus addr)  */
             {
						 	  eMBDisable( );
							 	osDelay(1);
					
				      	ModBusAddr.ModBuAddrSetFlag = 0x01;  
			          ModBusAddr.ModBusAddr = MB_AddresseValue;  
					      
							  Flash_Write_Data (FLASH_SLAVE_MODBUS_ID,(uint32_t *)&ModBusAddr,4);
							 						 
							 	osDelay(10);
				
					      NVIC_SystemReset();	
						 }	
					else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_5) 
             {
						 	  eMBDisable( );
							 	osDelay(2);
					
				      	TimeStep.SetFlag = 0x01;  
			          TimeStep.Timestep = timeStep; 
							 			      
							  Flash_Write_Data (FLASH_TIME_STEP_READING,(uint32_t *)&TimeStep,2);
							 
							 	eMBEnable( );					 
							 	osDelay(10);
						 }	 
				 
	 }
/************************ (C) COPYRIGHT @OnWert *****END OF FILE****/



