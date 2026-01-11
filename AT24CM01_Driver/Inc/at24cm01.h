/**
******************************************************************************
* @file      at24cm01.h
* @author    OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the at24cm01.c
******************************************************************************
*/
#ifndef AT24CM01_H
#define AT24CM01_H

#ifdef __cplusplus
extern "C" {
#endif
  
 /* ------------------------Includes ----------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
  /** @defgroup  at24cm01_file
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
    /* EEPROM parameters */
 #define AT24_I2C_ADDR        (0x56 << 1)
 #define AT24_PAGE_SIZE       128           // размер страницы
 #define AT24_WRITE_DELAY_MS  10            // задержка между страницами
 #define AT24_ACK_TIMEOUT_MS  20            // таймаут ACK polling

 #define EXTERN_EEPROM_ADDR   0x0001
 #define TEST_SIZE   32

 #define RELAY_REACTION_SIZE_BYTES  (EVENT_COUNT * MAX_CHANNELS)
 #define RELAY_ID_SIZE              1
 #define RELAY_SIZE_BYTES           (RELAY_ID_SIZE + RELAY_REACTION_SIZE_BYTES)
 #define RELAY_MODULE_ID_SIZE       1
 #define RELAY_MODULE_SIZE_BYTES    (RELAY_MODULE_ID_SIZE + (RELAY_SIZE_BYTES * RELAY_PER_MODULE))
 #define MODULES_EEPROM_SIZE        (RELAY_MODULE_SIZE_BYTES * RELAY_MODULE_COUNT)
 #define EEPROM_PAGE_SIZE           64 //special for writting function Modules_SaveToEEPROM(void)


 /* ------------------------External variables -------------------------*/
 extern I2C_HandleTypeDef hi2c2;

  /** @addtogroup  at24cm01_file
  * @{
  */
	/* API */
  HAL_StatusTypeDef AT24_Write(uint16_t memAddr, uint8_t *data, uint16_t size);
  HAL_StatusTypeDef AT24_Read (uint16_t memAddr, uint8_t *data, uint16_t size);
  HAL_StatusTypeDef Modules_SaveToEEPROM(void);
	HAL_StatusTypeDef Modules_LoadFromEEPROM(void);
  /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _AT24CM01_H */
