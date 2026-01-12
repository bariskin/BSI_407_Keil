/**
* @file      at24cm01.c
* @author    OnWert
* @brief     This file provides common functions for the AT24CM01 EEPROM driver
*/      

/* ------------------------Includes ----------------------------------*/
#include <string.h>
#include "RelaySystem.h"
#include "at24cm01.h"
/* ------------------------External variables -------------------------*/
extern RelayModule modules[RELAY_MODULE_COUNT]; /**< External reference to relay modules array */
/* ------------------------Global variables----------------------------*/
/* ------------------------Locale variables----------------------------*/

/**
  * @brief  Write data to AT24CM01 EEPROM memory
  * @param  memAddr Starting memory address (0..AT24CM01_MAX_ADDR)
  * @param  data    Pointer to data buffer to write
  * @param  size    Number of bytes to write
  * @retval HAL_StatusTypeDef Operation status:
  *         - HAL_OK: Write successful
  *         - HAL_ERROR: Write failed or timeout occurred
  * @note   Handles page boundary crossing automatically
  * @note   Implements ACK polling with timeout for write completion
  * @note   Respects EEPROM page size limitations
  */
HAL_StatusTypeDef AT24_Write(uint16_t memAddr, uint8_t *data, uint16_t size)
{
    HAL_StatusTypeDef status;

    while (size > 0)
    {
        /* Calculate available space in current page */
        uint16_t pageRemain = AT24_PAGE_SIZE - (memAddr % AT24_PAGE_SIZE);
        uint16_t chunk = (size < pageRemain) ? size : pageRemain;

        /* Prepare transmission buffer with address and data */
        uint8_t txBuf[AT24_PAGE_SIZE + 2];
        txBuf[0] = memAddr >> 8;        /**< Address MSB */
        txBuf[1] = memAddr & 0xFF;      /**< Address LSB */
        memcpy(&txBuf[2], data, chunk); /**< Copy data to transmit buffer */

        /* Transmit address and data */
        status = HAL_I2C_Master_Transmit(&hi2c2, AT24_I2C_ADDR, txBuf, chunk + 2, HAL_MAX_DELAY);
        if (status != HAL_OK)
            return status;

        /* Wait for EEPROM internal write cycle completion */
        HAL_Delay(AT24_WRITE_DELAY_MS);
        
        /* ACK polling with timeout */
        uint32_t start = HAL_GetTick();
        while (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 1, 10) != HAL_OK)
        {
            if ((HAL_GetTick() - start) > AT24_ACK_TIMEOUT_MS)
                return HAL_ERROR;   /**< EEPROM not responding */
        }

        /* Update pointers for next chunk */
        memAddr += chunk;
        data    += chunk;
        size    -= chunk;
    }

    return HAL_OK;
}

/**
  * @brief  Read data from AT24CM01 EEPROM memory
  * @param  memAddr Starting memory address (0..AT24CM01_MAX_ADDR)
  * @param  data    Pointer to destination buffer
  * @param  size    Number of bytes to read
  * @retval HAL_StatusTypeDef Operation status:
  *         - HAL_OK: Read successful
  *         - HAL_ERROR: Read failed
  * @note   Uses sequential read operation for optimal performance
  * @note   No delay needed between operations for reading
  */
HAL_StatusTypeDef AT24_Read(uint16_t memAddr, uint8_t *data, uint16_t size)
{
    uint8_t addr[2] = { memAddr >> 8, memAddr & 0xFF };

    /* Transmit memory address for read operation */
    if (HAL_I2C_Master_Transmit(&hi2c2, AT24_I2C_ADDR, addr, 2, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    /* Read data sequentially from specified address */
    return HAL_I2C_Master_Receive(&hi2c2, AT24_I2C_ADDR, data, size, HAL_MAX_DELAY);
}

/**
  * @brief  Save relay modules configuration to EEPROM
  * @retval HAL_StatusTypeDef Operation status:
  *         - HAL_OK: Save successful
  *         - HAL_ERROR: Save failed or EEPROM not ready
  * @note   Saves entire relay modules array to persistent storage
  * @note   Handles EEPROM page boundaries and write delays
  * @note   Uses EXTERN_EEPROM_ADDR as starting address
  */
HAL_StatusTypeDef Modules_SaveToEEPROM(void)
{
    /* Verify EEPROM is ready for communication */
    if (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 3, 10) != HAL_OK)
        return HAL_ERROR;

    uint32_t offset = 0;
    uint8_t *buf = (uint8_t*)modules;

    /* Write data in chunks respecting EEPROM page size */
    while (offset < MODULES_EEPROM_SIZE)
    {
        uint32_t remain = MODULES_EEPROM_SIZE - offset;
        uint32_t chunk  = (remain > EEPROM_PAGE_SIZE) ? EEPROM_PAGE_SIZE : remain;

        /* Write chunk to EEPROM */
        if (AT24_Write(EXTERN_EEPROM_ADDR + offset, buf + offset, chunk) != HAL_OK)
            return HAL_ERROR;

        /* Delay between write operations */
        HAL_Delay(AT24_WRITE_DELAY_MS);
        offset += chunk;
    }

    return HAL_OK;
}

/**
  * @brief  Load relay modules configuration from EEPROM
  * @retval HAL_StatusTypeDef Operation status:
  *         - HAL_OK: Load successful
  *         - HAL_ERROR: Load failed or EEPROM not ready
  * @note   Restores relay modules configuration from persistent storage
  * @note   Reads entire configuration in single operation
  * @note   Verifies EEPROM readiness before reading
  */
HAL_StatusTypeDef Modules_LoadFromEEPROM(void)
{
    /* Verify EEPROM is ready for communication */
    if (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 3, 10) != HAL_OK)
        return HAL_ERROR;

    /* Read entire modules array from EEPROM */
    if (AT24_Read(EXTERN_EEPROM_ADDR, (uint8_t *)modules, MODULES_EEPROM_SIZE) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}
/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/
