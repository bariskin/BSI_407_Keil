/**
******************************************************************************
* @file      FlashDriver.h
* @author    @OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the FlashDriver.c
******************************************************************************
*/

// https://microsin.net/programming/arm/stm32f4-embedded-flash-memory-interface.html
#ifndef _FLASH_DRIVER_FILE_H
#define _FLASH_DRIVER_FILE_H

#ifdef __cplusplus
extern "C" {
#endif
  
 /* ------------------------Includes ----------------------------------*/
 #include "stm32f4xx_hal.h"
 #include "UARTSlaveSettings.h"
/*-----------------------------------------------------------------------------------------------------*/
  /** @defgroup flash_driver_file
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
#define FLASH_ADDR_SECTOR_0   0x08000000 
#define FLASH_ADDR_SECTOR_1	  0x08004000 
#define FLASH_ADDR_SECTOR_2	  0x08008000 
#define FLASH_ADDR_SECTOR_3   0x0800C000 
#define FLASH_ADDR_SECTOR_4	  0x08010000 
#define FLASH_ADDR_SECTOR_5	  0x08020000  // 128 kB
#define FLASH_ADDR_SECTOR_6	  0x08040000  // 128 kB
#define FLASH_ADDR_SECTOR_7	  0x08060000  // 128 kB
#define FLASH_ADDR_SECTOR_8		0x08080000  // 128 kB
#define FLASH_ADDR_SECTOR_9	  0x080A0000  // 128 kB
#define FLASH_ADDR_SECTOR_10	0x080C0000  // 128 kB
#define FLASH_ADDR_SECTOR_11	0x080E0000  // 128 kB

#define FLASH_SLAVE_MODBUS_ID  	           FLASH_ADDR_SECTOR_9   // modbus addresse
#define FLASH_SETTING_UART     	           FLASH_ADDR_SECTOR_10		// baud_rate, parity, stop bits
		
		
 /* ------------------------External variables -------------------------*/
  

  /** @addtogroup flash_driver_file
  * @{
  */
	 void Flash_Read_Data (uint32_t StartSectorAddress, uint32_t *RxBuf, uint16_t numberofwords);
	 uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);
	 uint8_t getSector(uint32_t flashAddr);
	 /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _FLASH_DRIVER_FILE_H */
