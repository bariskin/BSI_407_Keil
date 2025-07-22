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
 /*---------------------------------- STM32F401xE/STM32F411xE/STM32F446xx ------------------------------*/
//#if defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx)
//#define FLASH_SECTOR_0     0U /*!< Sector Number 0   */
//#define FLASH_SECTOR_1     1U /*!< Sector Number 1   */
//#define FLASH_SECTOR_2     2U /*!< Sector Number 2   */
//#define FLASH_SECTOR_3     3U /*!< Sector Number 3   */
//#define FLASH_SECTOR_4     4U /*!< Sector Number 4   */
//#define FLASH_SECTOR_5     5U /*!< Sector Number 5   */
//#define FLASH_SECTOR_6     6U /*!< Sector Number 6   */
//#define FLASH_SECTOR_7     7U /*!< Sector Number 7   */
//#endif /* STM32F401xE || STM32F411xE || STM32F446xx */
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
#define FLASH_ADDR_SECTOR_5	  0x08020000  //0x08020000 - 0x0803FFFF    131072  bytes 
#define FLASH_ADDR_SECTOR_6	  0x08040000  //0x08040000 - 0x0805FFFF    131072  bytes 
#define FLASH_ADDR_SECTOR_7	  0x08060000  //0x08060000 - 0x0807FFFF    131072  bytes 
		
		
#define FLASH_SLAVE_MODBUS_ID  	           FLASH_ADDR_SECTOR_5    // modbus addresse
#define FLASH_SETTING_UART     	           FLASH_ADDR_SECTOR_6		// baud_rate, parity, stop bits
#define FLASH_FW_ID_FACTORY_NUMBER         FLASH_ADDR_SECTOR_7	  // FW ID , factory ID  		
		
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
