/**
******************************************************************************
* @file      UARTSettings.c
* @author    @OnWert
* @version   
* @brief     This file provides common functions for the uart settings
*/       
/* ------------------------Includes ----------------------------------*/
#include "UARTSlaveSettings.h"
#include "cmsis_os.h"
/* ------------------------External variables -------------------------*/
extern UART_HandleTypeDef huart1;
extern void Error_Handler(void);
extern UART_HandleTypeDef huart1;
/* ------------------------Global variables----------------------------*/

volatile UART_Settings_t UartSettingDefault  = 
	{ 
    .UartSetFlag =  0x00000001, 
    .BaudRateID  =  0x00000003, 
		.WordLengthID=  0x00000000, 
    .ParityID    =  0x00000000,
		.StopBitsID  =  0x00000001
  };

volatile UART_Settings_t UartSlaveSetting  = 
	{
     .UartSetFlag =  0x00000000, 
     .BaudRateID  =  0x00000000, 
		 .WordLengthID=  0x00000000, 
     .ParityID    =  0x00000000,
		 .StopBitsID  =  0x00000000
	};
/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/

uint32_t getUartWordLength(uint32_t wordlength)
 {
  uint32_t ReturnValue = 0x00000000;
	 
  switch(wordlength)
	 {
	  case 0:
			ReturnValue = UART_WORDLENGTH_8B;
     break;
     
    case 1:
			 ReturnValue = UART_WORDLENGTH_9B;
     break;
	 }
	
  return ReturnValue;
 }

 uint32_t getUartWordLengthId(uint32_t wordlengthId)
 {
  uint32_t ReturnValue = 0x00000000;
	 
  switch(wordlengthId)
	 {
	  case UART_WORDLENGTH_8B:
			ReturnValue = 0;

     break;
     
    case UART_WORDLENGTH_9B:
			 ReturnValue = 1;
     break;
	 }
	
  return ReturnValue;
 }
uint32_t getStopBits(uint32_t stopbits)
{
 uint32_t ReturnValue = 0x00000000;
	
	switch(stopbits)
	{
		case 1:
			ReturnValue = UART_STOPBITS_1;
		break;
		
		case 2:
			ReturnValue = UART_STOPBITS_2;
		break;
	}
	return ReturnValue;
}

uint32_t getStopBitsId(uint32_t stopbitsId)
 {
	 uint32_t ReturnValue = 0;
	 
	 switch(stopbitsId)
    {
			case UART_STOPBITS_1:
			ReturnValue = 1;
		  break;
		case UART_STOPBITS_2:
			ReturnValue = 2;
		break;
		
		}
	 return ReturnValue;
 }

uint32_t getParity(uint32_t idparity)
 {
   uint32_t ReturnValue = 0x00000000;
	 
	 switch(idparity)
	 {
		 case 0:
			 ReturnValue = UART_PARITY_NONE;
     break;
     case 1:
			 ReturnValue = UART_PARITY_ODD;    
     break;
     case 2:
			 ReturnValue = UART_PARITY_EVEN;
     break;
   
	 }
	 return  ReturnValue ;
 }

uint32_t getParityId(uint32_t parity)
 {
   uint32_t ReturnValue = 0;
	 
	 switch(parity)
	 {
		 case UART_PARITY_NONE:
			 ReturnValue = 0;
     break;
     case UART_PARITY_ODD:
			  ReturnValue = 1;
     break;
     case UART_PARITY_EVEN:
			ReturnValue = 2;  
     break;
	 }
	 return  ReturnValue ;
 }
 
 
	uint32_t getBaudrate(uint8_t numbaudrate)
	 {
		uint32_t ReturnValue = 0x00000000;
		
		 switch(numbaudrate)
			{
				case 0: 
				ReturnValue = 1200;
									break;
					case 1: 
				ReturnValue = 2400;
									break;
				case 2: 
				ReturnValue = 4800;
									break;
				case 3: 
				ReturnValue = 9600;
									break;
				case 4: 
				ReturnValue = 19200;
									break;
				case 5: 
				ReturnValue = 38400;
									break;		
				case 6:
				ReturnValue = 115200;
									break;
			}
		return ReturnValue;
	}
 
uint8_t getBaudRateId(uint32_t BaudRate)
 {
	 
	uint8_t ReturnValue = 0x00;
	
	 switch(BaudRate)
			{
				case 1200: 
				ReturnValue = 0;
								break;
				case 2400: 
				ReturnValue = 1;
								break;	
			case 4800: 
				ReturnValue = 2;
								break;
			case 9600: 
				ReturnValue = 3;
								break;
			case 19200: 
				ReturnValue = 4;
								break;
			case 38400: 
				ReturnValue = 5;
								break;	
			case 115200:
				ReturnValue = 6;
								break;
			}
	return ReturnValue;
}

void updateUartSettings( UART_Settings_t *uartsetting )
 {
     huart1.Instance = USART1;
     huart1.Init.BaudRate   = getBaudrate(uartsetting->BaudRateID);
     //huart1.Init.WordLength = getUartWordLength(uartsetting->WordLengthID);
     huart1.Init.StopBits   = getStopBits(uartsetting->StopBitsID);
     huart1.Init.Parity     = getParity(uartsetting->ParityID);
     huart1.Init.Mode = UART_MODE_TX_RX;
     huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE; 
     huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	 
	 
	 	           /* FOR PARITY */
			 if(UartSlaveSetting.ParityID == 1 || UartSlaveSetting.ParityID == 2)
			 {
			  huart1.Init.WordLength = getUartWordLength(1); //9 bits working with   8 bit host (even ,odd)
			 }
			 
			 else if (UartSlaveSetting.ParityID == 0)
			 {
			   huart1.Init.WordLength = getUartWordLength(0); //8 bits
			 }
	 
	 
     if (HAL_UART_Init(&huart1) != HAL_OK)
      {
       Error_Handler();
      }
			osDelay(20);
 }



/************************ (C) COPYRIGHT OnWert  ****END OF FILE****/

