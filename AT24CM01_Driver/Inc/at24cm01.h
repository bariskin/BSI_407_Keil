/**
* @file      at24cm01.h
* @author    OnWert
* @brief     This file contains defines and all the functions prototypes for the AT24CM01 EEPROM driver
*/

#ifndef AT24CM01_H
#define AT24CM01_H

#ifdef __cplusplus
extern "C" {
#endif
/* ------------------------Includes ----------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/**
  * @defgroup at24cm01_driver AT24CM01 EEPROM Driver
  * @brief Driver for AT24CM01 1Mb serial EEPROM memory
  * 
  * Provides functions for reading/writing data to external EEPROM storage
  * with support for relay system configuration persistence.
  */
  
/**
  * @}
  */

/* ------------------------Defines ----------------------------------*/

/** @defgroup at24cm01_config Configuration Constants
  * @{
  */

/* EEPROM Device Parameters */
#define AT24_I2C_ADDR        (0x56 << 1)    /**< I2C device address (7-bit address shifted left) */
#define AT24_PAGE_SIZE       128            /**< EEPROM page size in bytes */
#define AT24_WRITE_DELAY_MS  10             /**< Delay between page writes (internal programming time) */
#define AT24_ACK_TIMEOUT_MS  20             /**< ACK polling timeout in milliseconds */

/* Memory Address Configuration */
#define EXTERN_EEPROM_ADDR   0x0001         /**< Starting address for relay configuration storage */

/* Relay Configuration Size Calculations */
/** Size of reaction matrix for one relay (excluding EVENT_POROG_NORMAL) */
#define RELAY_REACTION_SIZE_BYTES  ((EVENT_COUNT - 1) * MAX_CHANNELS)

/** Size of relay identifier */
#define RELAY_ID_SIZE              1

/** Total size of one relay structure in bytes */
#define RELAY_SIZE_BYTES           (RELAY_ID_SIZE + RELAY_REACTION_SIZE_BYTES)

/** Size of module identifier */
#define RELAY_MODULE_ID_SIZE       1

/** Total size of one relay module structure in bytes */
#define RELAY_MODULE_SIZE_BYTES    (RELAY_MODULE_ID_SIZE + (RELAY_SIZE_BYTES * RELAY_PER_MODULE))

/** Total EEPROM space required for all relay modules configuration */
#define MODULES_EEPROM_SIZE        (RELAY_MODULE_SIZE_BYTES * RELAY_MODULE_COUNT)

/** Page size for EEPROM write operations in Modules_SaveToEEPROM() */
#define EEPROM_PAGE_SIZE           64

/**
  * @}
  */

/* ------------------------External variables -------------------------*/
extern I2C_HandleTypeDef hi2c2;  /**< External I2C handle for EEPROM communication */

/** @addtogroup at24cm01_driver
  * @{
  */

/* Public API Functions */

/**
  * @brief  Write data to AT24CM01 EEPROM
  * @param  memAddr Starting memory address (0..AT24CM01_MAX_ADDR)
  * @param  data    Pointer to source data buffer
  * @param  size    Number of bytes to write
  * @retval HAL_StatusTypeDef Operation status (HAL_OK on success)
  * @note   Handles page boundary crossing automatically
  */
HAL_StatusTypeDef AT24_Write(uint16_t memAddr, uint8_t *data, uint16_t size);

/**
  * @brief  Read data from AT24CM01 EEPROM
  * @param  memAddr Starting memory address (0..AT24CM01_MAX_ADDR)
  * @param  data    Pointer to destination data buffer
  * @param  size    Number of bytes to read
  * @retval HAL_StatusTypeDef Operation status (HAL_OK on success)
  * @note   Uses sequential read for optimal performance
  */
HAL_StatusTypeDef AT24_Read(uint16_t memAddr, uint8_t *data, uint16_t size);

/**
  * @brief  Save relay modules configuration to EEPROM
  * @retval HAL_StatusTypeDef Operation status (HAL_OK on success)
  * @note   Persists entire relay system configuration to non-volatile memory
  * @note   Writes data in chunks to respect EEPROM page size limitations
  */
HAL_StatusTypeDef Modules_SaveToEEPROM(void);

/**
  * @brief  Load relay modules configuration from EEPROM
  * @retval HAL_StatusTypeDef Operation status (HAL_OK on success)
  * @note   Restores relay system configuration from non-volatile memory
  * @note   Verifies EEPROM device readiness before reading
  */
HAL_StatusTypeDef Modules_LoadFromEEPROM(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* AT24CM01_H */
