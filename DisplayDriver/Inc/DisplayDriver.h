/**
******************************************************************************
* @file      DisplayDriver.h
* @author    OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the DisplayDriver.c
******************************************************************************
*/
#ifndef _DISPLAY_DRIVER_H
#define _DISPLAY_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif
  
 /* ------------------------Includes ----------------------------------*/
 #include "stm32f4xx_hal.h"
  /** @defgroup display_driver
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
  #define ARRAY_TX_SIZE		100
  #define ARRAY_RX_SIZE		256
	#define START_INIT_BYTE	0x10
	
		typedef struct
	  {
			uint8_t channelIndex;
			uint16_t status;
			char model[20];
			char posit[20];
			char gas[20];
			float scaleMax;
			char scaleDimension[20];
			float value;
			float current4_20;									  	// петля 4-20 mA
			float currentConsumptoin;								// ток потребления
			float Porog1;
			float Porog2;
			uint8_t error;
			uint8_t errorRS;
			uint32_t status_GA;
			uint32_t value_uint32;
			uint32_t scaleMax_uint32;					
			uint8_t scaleDimension_hex;					
			uint32_t Porog1_int32;							
			uint32_t Porog2_int32;							
			uint32_t model_hex;						
    }paramDev_t;
	
 /* ------------------------External variables -------------------------*/
  

  /** @addtogroup display_driver
  * @{
  */
	 void Init_qDev(void);
	 void UART_Display_StartReceiving(void);
	 void FixRussianEncodingForNextion (char *str);
   void SendNextionCommand(const char* format, ...);
	 void InitNextionDisplayWithDeviceData(uint8_t numberOfdevices);
	 void initDeviceData(uint8_t numberOfdevices);
	 uint8_t getIntFromChar( char * inputString, uint8_t stringSize);
	 void HandleDisplayCommands(uint8_t * displayresponse, uint8_t *arrDisplayRX, uint8_t *packet_ready, UART_HandleTypeDef *huart);
   void UpdateNextionDisplayWithChannelData(uint8_t channel_num);
		/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _DISPLAY_DRIVER_H */
