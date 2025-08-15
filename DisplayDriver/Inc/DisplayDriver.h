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
	 void HandleDisplayCommands(uint8_t displayResponse, uint8_t *arrDisplayRX, uint8_t *packet_ready, UART_HandleTypeDef *huart);
  /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _DISPLAY_DRIVER_H */