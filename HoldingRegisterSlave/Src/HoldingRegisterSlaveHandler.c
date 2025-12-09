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

extern uint16_t holdingRegsPart1[MAX_MODBUS_SLAVE_REGS_PART];  // Адреса 1-120
extern volatile UART_Settings_t UartSlaveSetting; 
extern volatile ModBusAddr_t  ModBusAddr; 
extern volatile ModBusAddr_t  ModBusAddrDefault;
extern volatile TimeStepReadingSensores_t TimeStep;
extern volatile TimeStepReadingSensores_t TimeStepDefault;

//extern osThreadId InputHandlerHandle;
extern osThreadId SlaveEventTaskHandle;


extern SensorState_t  SensorStateArray[NUMBER_SLAVE_DEVICES]; 


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
		
		}
		osDelay(1);	
	}
	
 uint16_t WriteParamToModbusSlaveStack(uint16_t MBregIdx)
  {
		uint16_t OutputValue = 0x0000;
		
		 // Обработка регистров 1-12
    if (MBregIdx <= HOLDING_REGISTER_SLAVE_IDX_12)
    {
		
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
				/* ************************* месяц ******************** */   
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
			 
		 break;
	   }
	 }
		 
//		 case 		 HOLDING_REGISTER_SLAVE_IDX_13 : 
//			  
//		     /* первая часть float значения  */
//		    OutputValue = (uint16_t)SensorStateArray[0].Concentration;
//			 break;
//		 case 		 HOLDING_REGISTER_SLAVE_IDX_14 :
//		    /* вторая  часть float значения  */
//		
//		    //OutputValue = 
//			  break;
//		 /*       Код вещества в ascii (10 байт) */
//		 
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_15 :
//			  OutputValue = SensorStateArray[0].SensorSubstanceCode[0];
//		  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_16 :
//			  OutputValue = SensorStateArray[0].SensorSubstanceCode[1];
//		  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_17 :
//			  OutputValue =  SensorStateArray[0].SensorSubstanceCode[2];
//			  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_18 :
//			 OutputValue =  SensorStateArray[0].SensorSubstanceCode[3];
//			  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_19 :
//			  OutputValue =  SensorStateArray[0].SensorSubstanceCode[4];
//			  break;
//		 case      HOLDING_REGISTER_SLAVE_IDX_20  :
//			  OutputValue =  SensorStateArray[0].SensorSubstanceCode[5];
//			  break;
//		 case 		 HOLDING_REGISTER_SLAVE_IDX_21 :
//			  OutputValue =  SensorStateArray[0].SensorSubstanceCode[6];
//			  break;
//		 case   	 HOLDING_REGISTER_SLAVE_IDX_22 :
//			OutputValue =  SensorStateArray[0].SensorSubstanceCode[7];
//			  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_23 :
//			 OutputValue =  SensorStateArray[0].SensorSubstanceCode[8];
//			  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_24 :
//			  OutputValue = SensorStateArray[0].SensorSubstanceCode[9];
//			  break;
		 
//		 /* Размерность вещества в ascii (10 байт) */
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_25 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[0];
//			  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_26 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[1];
//			  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_27 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[2];
//			  break;
//		 case   	 HOLDING_REGISTER_SLAVE_IDX_28 :
//		     OutputValue = SensorStateArray[0].SensorScaleDimension[3];
//			  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_29 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[4];
//			  break;
//		 case 	   HOLDING_REGISTER_SLAVE_IDX_30 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[5];
//		    break;
//		 case 	HOLDING_REGISTER_SLAVE_IDX_31 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[6];
//		    break;
//		 case HOLDING_REGISTER_SLAVE_IDX_32 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[7];
//		    break;
//		 case HOLDING_REGISTER_SLAVE_IDX_33 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[8];
//		    break;
//		 case  HOLDING_REGISTER_SLAVE_IDX_34 :
//			   OutputValue = SensorStateArray[0].SensorScaleDimension[9];
//		 break;
		
		   /* ============ FLOAT ЗНАЧЕНИЯ ДЛЯ 46 КАНАЛОВ ============ */
      // Регистры 13-14: Float значение первого датчика
    else if (MBregIdx == HOLDING_REGISTER_SLAVE_IDX_13 || 
             MBregIdx == HOLDING_REGISTER_SLAVE_IDX_14)
     {
        uint16_t offset = MBregIdx - HOLDING_REGISTER_SLAVE_IDX_13;
        uint8_t sensor_idx = offset / 2;      
        uint8_t float_part = offset % 2;       
    
        if (sensor_idx < TOTAL_CHANNEL)
          {
           float concentration = SensorStateArray[sensor_idx].Concentration;
           uint16_t *float_ptr = (uint16_t*)&concentration;
           return float_ptr[float_part];
          }
     }
		   
		    // Массив из  TOTAL_CHANNEL каналов: SubstanceCode (10 байт на датчик)
    else if (MBregIdx >= SUBSTANCE_START && 
             MBregIdx < SUBSTANCE_START + TOTAL_CHANNEL * 10)
     {
        uint16_t offset = MBregIdx - SUBSTANCE_START;
        uint8_t sensor_idx = offset / 10;
        uint8_t byte_idx = offset % 10;
        
        if (sensor_idx < TOTAL_CHANNEL)
        {
            return SensorStateArray[sensor_idx].SensorSubstanceCode[byte_idx];
        }
     }
		
     // Массив из TOTAL_CHANNEL каналов: ScaleDimension (10 байт на датчик)
    else if (MBregIdx >= SCALE_START && 
             MBregIdx < SCALE_START + TOTAL_CHANNEL * 10)
    {
        uint16_t offset = MBregIdx - SCALE_START;
        uint8_t sensor_idx = offset / 10;
        uint8_t byte_idx = offset % 10;
        
        if (sensor_idx < TOTAL_CHANNEL)
        {
            return SensorStateArray[sensor_idx].SensorScaleDimension[byte_idx];
        }
    }		 
		 
     osDelay(1);			
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



