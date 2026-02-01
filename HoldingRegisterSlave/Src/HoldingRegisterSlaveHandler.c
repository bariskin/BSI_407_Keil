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
#include "DisplayDriver.h"
#include "SensorLogs.h"
/* ------------------------External variables -------------------------*/

extern uint16_t holdingRegsPart1[S_REG_HOLDING_NREGS];
extern volatile ModBusAddr_t  ModBusAddr; 
extern volatile ModBusAddr_t  ModBusAddrDefault;
extern volatile TimeStepReadingSensores_t TimeStep;
extern volatile TimeStepReadingSensores_t TimeStepDefault;

//extern osThreadId InputHandlerHandle;
extern osThreadId SlaveEventTaskHandle;


extern SensorState_t  SensorStateArray[NUMBER_SLAVE_DEVICES]; 
extern SensorInfo_t   SensorInfo;

/* ------------------------Global variables----------------------------*/
volatile uint32_t  MB_BaudRateValue = 0x00000000;
volatile uint32_t  MB_ParityValue   = 0x00000000;
volatile uint32_t  MB_StopBitsValue = 0x00000000;
volatile uint32_t  MB_AddresseValue = 0x00000000;

volatile uint16_t  timeStep = (uint16_t)TIME_STEP_MIN_2; // ms

volatile uint16_t  year    = 0x00;
volatile uint8_t   month   = 0x00;
volatile uint8_t   day     = 0x00;
volatile uint8_t   hour    = 0x00;
volatile uint8_t   minute  = 0x00;
volatile uint8_t   second  = 0x00;

/* ------------------------Locale variables----------------------------*/
uint8_t sensorIDX = 0;
uint8_t currentChannel = 1;
uint8_t oldCurrentChannel = 0;
bool isEven = false; 
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
			 break;
		 case HOLDING_REGISTER_SLAVE_IDX_6: 
			 
       if(RegValue >= 2000  && RegValue <= 2099 )
		     {
					  year  = RegValue - 2000;
				  	xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_6, eSetValueWithOverwrite); 
				 }
				else
				{
				  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_6] =  Get_RTC_Year() + 2000;;
				}
				 break;
		 case HOLDING_REGISTER_SLAVE_IDX_7:
			 
		    if(RegValue >= 1  && RegValue <= 12 )
		     {
					  month  = RegValue;
				  	xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_7, eSetValueWithOverwrite); 
				 }
				else
				{
				  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_7] = Get_RTC_Month();
				}
			 
				 break;		
		 case HOLDING_REGISTER_SLAVE_IDX_8: 		
			   if(RegValue >= 1  && RegValue <= 31 )
		     {
					  day  = RegValue;
				  	xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_8, eSetValueWithOverwrite); 
				 }
				else
				{
				  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_8] = Get_RTC_Day();
				}
		     
				 break; 
				/* hour */
		 case HOLDING_REGISTER_SLAVE_IDX_9: 
			 
		     if(RegValue >= 1  && RegValue <= 23 )
		     {
					  hour  = RegValue;
				  	xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_9, eSetValueWithOverwrite); 
				 }
				else
				{
				  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_9] = Get_RTC_Hour();
				}
			 
				 break;
				/* minute */
		 case HOLDING_REGISTER_SLAVE_IDX_10: 	
			 
			   if(RegValue >= 1  && RegValue <= 59 )
		     {
					  minute  = RegValue;
				  	xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_10, eSetValueWithOverwrite); 
				 }
				else
				{
				  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_10] = Get_RTC_Minute();
				}
				 break;	
        /* second */				
		 case HOLDING_REGISTER_SLAVE_IDX_11: 	
			 
			   if(RegValue >= 1  && RegValue <= 59 )
		     {
					  second  = RegValue;
				  	xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_11, eSetValueWithOverwrite); 
				 }
				else
				{
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_11] = Get_RTC_Second();
				}
				 break; 
				/* channelID */
			case 	 HOLDING_REGISTER_SLAVE_IDX_12:
				
			   if(RegValue >=(uint16_t)1 && RegValue <= SensorInfo.count * 2)
				 {					 
           holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_12] = RegValue;
           currentChannel = RegValue;
				 }
				break;
				
		}
		osDelay(1);	
	}
	
 uint16_t WriteParamToModbusSlaveStack(uint16_t MBregIdx)
  {
		uint16_t OutputValue = 0x0000;
		
		switch(MBregIdx)
      { 		
     		/* ************ baud rate************* */			
			 case HOLDING_REGISTER_SLAVE_IDX_1 :
				 
				 OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_1];
				 break;
			 /* ************* parity ************ */
			 case HOLDING_REGISTER_SLAVE_IDX_2 :
				 
				 OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_2];
				 break;
			 
			 /* ************* stop bits************ */
			 case HOLDING_REGISTER_SLAVE_IDX_3 :
				
			   OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_3];
				 break;		 
			 /* ********* SLAVE ID (modbus addr) **** */ 
			 case HOLDING_REGISTER_SLAVE_IDX_4 :
				
			   OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_4];
				 break;
			 /* ********* the time step for reading the sensor **** */   
			 case HOLDING_REGISTER_SLAVE_IDX_5 :
				
			   OutputValue =  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_5];
				 break;
			  /* ************************* год ******************** */   
			  case HOLDING_REGISTER_SLAVE_IDX_6: 
					
				 OutputValue =  Get_RTC_Year() + 2000;
				
				 break;
				/* ************************* мес€ц ******************** */   
		 case HOLDING_REGISTER_SLAVE_IDX_7: 
			 
         OutputValue =  Get_RTC_Month();	
		 
				 break;	
        /* ************************* день ******************** */  		 
		 case HOLDING_REGISTER_SLAVE_IDX_8: 

         OutputValue = Get_RTC_Day() ;			 
				 break;
		   /* ************************* час ******************** */  	
		 case HOLDING_REGISTER_SLAVE_IDX_9: 	

		     OutputValue =  Get_RTC_Hour();		
				 break;
		 /* ************************* минуты ******************** */  
		 case HOLDING_REGISTER_SLAVE_IDX_10: 
         
         OutputValue =  Get_RTC_Minute();				 
				 break;		
		 case HOLDING_REGISTER_SLAVE_IDX_11: 
         OutputValue =  Get_RTC_Second();					 
				 break;
		 
		 case HOLDING_REGISTER_SLAVE_IDX_12: 
		      
		      OutputValue = currentChannel;
		      if(oldCurrentChannel != currentChannel)
					{
		       sensorIDX  = GetSensorIdxByChannel(currentChannel,(SensorState_t *)&SensorStateArray);
					 oldCurrentChannel = currentChannel;
					}
		    
		      isEven =  (currentChannel % 2 == 0);
		   break; 
		 
		 case HOLDING_REGISTER_SLAVE_IDX_13 :
			 
			 if (!isEven)
          {
            OutputValue = SensorStateArray[sensorIDX].Concentration_H;
          }
       else
          {
            OutputValue = SensorStateArray[sensorIDX].Concentration_H_2;
          }
			  
		  break;
		 case  HOLDING_REGISTER_SLAVE_IDX_14:
			 if (!isEven)
          {
			     OutputValue = SensorStateArray[sensorIDX].Concentration_L;
					}	
					else
					{
					  OutputValue = SensorStateArray[sensorIDX].Concentration_L_2;
					}
		  break;
		                /* SensorScaleDimension */
		 case 	   HOLDING_REGISTER_SLAVE_IDX_15 :
	      if (!isEven)
          {			 
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[0];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_15] = SensorStateArray[sensorIDX].SensorScaleDimension[0];
					}
				else
        {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[0];
					  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_15] = SensorStateArray[sensorIDX].SensorScaleDimension_2[0];
				}	
			  break;
		  case 	   HOLDING_REGISTER_SLAVE_IDX_16 :
					if (!isEven)
          {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[1];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_16] = SensorStateArray[sensorIDX].SensorScaleDimension[1];
					}
				 else
          {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[1];
					  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_16] = SensorStateArray[sensorIDX].SensorScaleDimension_2[1];
				  }
			  break;
		  case 	   HOLDING_REGISTER_SLAVE_IDX_17 :
				 if (!isEven)
          {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[2];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_17] = SensorStateArray[sensorIDX].SensorScaleDimension[2];
					}
				else
         {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[2];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_17] = SensorStateArray[sensorIDX].SensorScaleDimension_2[2];
				 }
			  break;
			case HOLDING_REGISTER_SLAVE_IDX_18 :
				 if (!isEven)
          {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[3];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_18] = SensorStateArray[sensorIDX].SensorScaleDimension[3];
					}
				 else
          {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[3];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_18] = SensorStateArray[sensorIDX].SensorScaleDimension_2[3];
				  }
			  break;
			case 	   HOLDING_REGISTER_SLAVE_IDX_19 :
			 if (!isEven)
          {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[4];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_19] = SensorStateArray[sensorIDX].SensorScaleDimension[4];
					}
				 else
          {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[4];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_19] = SensorStateArray[sensorIDX].SensorScaleDimension_2[4];
				  }
    
         break;

			case 	   HOLDING_REGISTER_SLAVE_IDX_20 :
				 if (!isEven)
          {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[5];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_20] = SensorStateArray[sensorIDX].SensorScaleDimension[5];
					}
				 else
          {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[5];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_20] = SensorStateArray[sensorIDX].SensorScaleDimension_2[5];
				  }
    
			  break;
			case 	   HOLDING_REGISTER_SLAVE_IDX_21 :
					if (!isEven)
          {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[6];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_21] = SensorStateArray[sensorIDX].SensorScaleDimension[6];
					}
				 else
          {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[6];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_21] = SensorStateArray[sensorIDX].SensorScaleDimension_2[6];
				  }
			  break;
			case HOLDING_REGISTER_SLAVE_IDX_22 :
			    if (!isEven)
           {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[7];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_22] = SensorStateArray[sensorIDX].SensorScaleDimension[7];
					 }
				  else
          {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[7];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_22] = SensorStateArray[sensorIDX].SensorScaleDimension_2[7];
				  }
			  break;
			case 	HOLDING_REGISTER_SLAVE_IDX_23 :
				  if (!isEven)
           {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[8];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_23] = SensorStateArray[sensorIDX].SensorScaleDimension[8];
					 }
				  else
          {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[8];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_23] = SensorStateArray[sensorIDX].SensorScaleDimension_2[8];
				  }
			  break;
			case 	HOLDING_REGISTER_SLAVE_IDX_24 :
					if (!isEven)
           {
			      OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension[9];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_24] = SensorStateArray[sensorIDX].SensorScaleDimension[9];
					 }
				  else
          {
				    OutputValue =  SensorStateArray[sensorIDX].SensorScaleDimension_2[9];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_24] = SensorStateArray[sensorIDX].SensorScaleDimension_2[9];
				  }
			  break;
		 
		            /* SensorGas */
	 
		 case  HOLDING_REGISTER_SLAVE_IDX_25 :
			 	 if (!isEven)
         {
			     OutputValue =  SensorStateArray[sensorIDX ].SensorGas[0];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_25] = SensorStateArray[sensorIDX].SensorGas[0];
				 }
				else
         {
				   OutputValue =  SensorStateArray[sensorIDX ].SensorGas_2[0];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_25] = SensorStateArray[sensorIDX].SensorGas_2[0];
				 }
			  break;
		 case HOLDING_REGISTER_SLAVE_IDX_26 :
			 	 if (!isEven)
         {
			     OutputValue =  SensorStateArray[sensorIDX].SensorGas[1];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_26] = SensorStateArray[sensorIDX].SensorGas[1];
				 }
				else
         {
				  OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[1];
					holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_26] = SensorStateArray[sensorIDX].SensorGas_2[1];
				 }
			  break;
		 case  HOLDING_REGISTER_SLAVE_IDX_27 :
			 	 if (!isEven)
          {
			     OutputValue =  SensorStateArray[sensorIDX].SensorGas[2];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_27] = SensorStateArray[sensorIDX].SensorGas[2];
					}
				else
         {
				  OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[2];
					holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_27] = SensorStateArray[sensorIDX].SensorGas_2[2];
				 }
			  break;
		 case  HOLDING_REGISTER_SLAVE_IDX_28  :
			 	 if (!isEven)
          {
			     OutputValue =  SensorStateArray[sensorIDX].SensorGas[3];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_28] = SensorStateArray[sensorIDX].SensorGas[3];
					}
				else
          {
				   OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[3];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_28] = SensorStateArray[sensorIDX].SensorGas_2[3];
				  }
			  break;
		 case  HOLDING_REGISTER_SLAVE_IDX_29 :
			 	 if (!isEven)
          {
			     OutputValue =  SensorStateArray[sensorIDX].SensorGas[4];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_29] = SensorStateArray[sensorIDX].SensorGas[4];
					}
				 else
         {
				  OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[4];
					holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_29] = SensorStateArray[sensorIDX].SensorGas_2[4];
				 }
			  break;
		 case  HOLDING_REGISTER_SLAVE_IDX_30 :
			 	 if (!isEven)
          {
			     OutputValue =  SensorStateArray[sensorIDX].SensorGas[5];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_30] = SensorStateArray[sensorIDX].SensorGas[5];
					}
					else
          {
				   OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[5];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_30] = SensorStateArray[sensorIDX].SensorGas_2[5];
				  }
			  break;
		 	 case  HOLDING_REGISTER_SLAVE_IDX_31 :
				 	if (!isEven)
          {
			      OutputValue =  SensorStateArray[sensorIDX].SensorGas[6];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_31] = SensorStateArray[sensorIDX].SensorGas[6];
					}
				 else
          {
				   OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[6];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_31] = SensorStateArray[sensorIDX].SensorGas_2[6];
				  }
			  break;
		 
			 case HOLDING_REGISTER_SLAVE_IDX_32 :
					if (!isEven)
          {
			     OutputValue =  SensorStateArray[sensorIDX].SensorGas[7];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_32] = SensorStateArray[sensorIDX].SensorGas[7];
					}
				  else
          {
				   OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[7];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_32] = SensorStateArray[sensorIDX].SensorGas_2[7];
				  }
			  break;
		 
				case HOLDING_REGISTER_SLAVE_IDX_33 :
					if (!isEven)
          {
			      OutputValue =  SensorStateArray[sensorIDX].SensorGas[8];
						holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_33] = SensorStateArray[sensorIDX].SensorGas[8];
					}
				 else
          {
				   OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[8];
					 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_33] = SensorStateArray[sensorIDX].SensorGas_2[8];
				  }
			  break;
		 
				case  HOLDING_REGISTER_SLAVE_IDX_34 :
					if (!isEven)
            {
			       OutputValue =  SensorStateArray[sensorIDX].SensorGas[9];
						 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_34] = SensorStateArray[sensorIDX].SensorGas[9];
						}
				 else
           {
				     OutputValue =  SensorStateArray[sensorIDX].SensorGas_2[9];
						 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_34] = SensorStateArray[sensorIDX].SensorGas_2[9];
				   }
			  break;
		 
	} 
		osDelay(1);	
		return	OutputValue;	
 
}	
  void ModBusSlaveEventHoldingRegHandler(void)
	 {
		 	 uint32_t ulNotifiedValue = 0 ;
		
		 xTaskNotifyWait
				(             
				 0x00,             /* DonТt clear any notification bits on entry. */
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
          else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_6) 
             { 
							 eMBDisable( );
							 osDelay(5);
							 Set_RTC_Year(year);
							 eMBEnable( );					 
							 osDelay(10);

						 }
					  else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_7) 
             { 
							 eMBDisable( );
							 osDelay(5);
							 Set_RTC_Month(month);
							 eMBEnable( );					 
							 osDelay(10);
						 }	
             else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_8) 
             { 
							 eMBDisable( );
							 osDelay(5);
							 Set_RTC_Day(day);
							 eMBEnable( );					 
							 osDelay(10);
						 }	
						 else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_9) 
             { 
							 eMBDisable( );
							 osDelay(5);
							 Set_RTC_Hour(hour);
							 eMBEnable( );					 
							 osDelay(10);
						 }	
             else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_10) 
             { 
							 eMBDisable( );
							 osDelay(5);
							 Set_RTC_Minute(minute);
							 eMBEnable( );					 
							 osDelay(10);
						 }	
            else if(ulNotifiedValue == HOLDING_REGISTER_SLAVE_IDX_11) 
             { 
							 eMBDisable( );
							 osDelay(1);
	             Set_RTC_Second(second);
							 eMBEnable( );					 
							 osDelay(10);
						 }				 
	 }
/************************ (C) COPYRIGHT @OnWert *****END OF FILE****/



