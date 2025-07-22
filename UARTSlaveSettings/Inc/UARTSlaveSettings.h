/**
******************************************************************************
* @file      UARTSettings.h
* @author    @OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the UARTSettings.h
******************************************************************************
*/
#ifndef _UART_SETTINGS_H
#define _UART_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif
  
 /* ------------------------Includes ----------------------------------*/
#include "stm32f4xx_hal.h"
  /** @defgroup uart_settings
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
 #define UART_SET_FLAG_OFF  0
 #define UART_SET_FLAG_ON   1

  typedef struct
  {
	 uint32_t UartSetFlag;
	 uint32_t BaudRateID;
   uint32_t WordLengthID;
	 uint32_t ParityID;
   uint32_t StopBitsID;
  }UART_Settings_t; 
	
 /* ------------------------External variables -------------------------*/
  
	
  /** @addtogroup uart_settings
  * @{
  */	 
    uint32_t Switch_Parity(uint8_t num);
	  uint32_t Switch_StopBits(uint8_t num);
	  void setUartSettings(void);
	  uint32_t getBaudrate(uint8_t num);
	  uint8_t getBaudRateId(uint32_t BaudRate);
	  uint32_t getParityId(uint32_t parity);
	  uint32_t getParity(uint32_t idparity);
	  uint32_t getStopBits(uint32_t stopbits);
	  uint32_t getStopBitsId(uint32_t stopbitsId);
	  uint32_t getUartWordLength(uint32_t wordlength);
	  uint32_t getUartWordLengthId(uint32_t wordlengthId);
	  void updateUartSettings( UART_Settings_t *uartsetting );
	/**
  * @
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _UART_SETTINGS_H */
