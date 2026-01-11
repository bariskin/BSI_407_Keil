/**
******************************************************************************
* @file      at24cm01.c
* @author    OnWert
* @version   
* @brief     This file provides common functions for the  at24cm01
*           
*/       
/* ------------------------Includes ----------------------------------*/
#include "RelaySystem.h"
#include "at24cm01.h"
#include <string.h>

/* ------------------------External variables -------------------------*/
extern RelayModule modules[RELAY_MODULE_COUNT];
/* ------------------------Global variables----------------------------*/
/* ------------------------Locale variables----------------------------*/
/* ================== WRITE ================== */
HAL_StatusTypeDef AT24_Write(uint16_t memAddr, uint8_t *data, uint16_t size)
{
    HAL_StatusTypeDef status;

    while (size > 0)
    {
        uint16_t pageRemain = AT24_PAGE_SIZE - (memAddr % AT24_PAGE_SIZE);
        uint16_t chunk = (size < pageRemain) ? size : pageRemain;

        uint8_t txBuf[AT24_PAGE_SIZE + 2];
        txBuf[0] = memAddr >> 8;        // MSB адреса
        txBuf[1] = memAddr & 0xFF;      // LSB адреса
        memcpy(&txBuf[2], data, chunk);

        /* Передача */
        status = HAL_I2C_Master_Transmit(&hi2c2, AT24_I2C_ADDR, txBuf, chunk + 2, HAL_MAX_DELAY);
        if (status != HAL_OK)
            return status;

        /* Задержка на внутренний цикл записи EEPROM */
        HAL_Delay(AT24_WRITE_DELAY_MS);
        /* ACK polling с таймаутом */
        uint32_t start = HAL_GetTick();
        while (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 1, 10) != HAL_OK)
        {
            if ((HAL_GetTick() - start) > AT24_ACK_TIMEOUT_MS)
                return HAL_ERROR;   // EEPROM не отвечает
        }

        memAddr += chunk;
        data    += chunk;
        size    -= chunk;
    }

    return HAL_OK;
}

/* ================== READ ================== */
HAL_StatusTypeDef AT24_Read(uint16_t memAddr, uint8_t *data, uint16_t size)
{
    uint8_t addr[2] = { memAddr >> 8, memAddr & 0xFF };

    /* Передача адреса памяти */
    if (HAL_I2C_Master_Transmit(&hi2c2, AT24_I2C_ADDR, addr, 2, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    /* Чтение данных */
    return HAL_I2C_Master_Receive(&hi2c2, AT24_I2C_ADDR, data, size, HAL_MAX_DELAY);
}


HAL_StatusTypeDef Modules_SaveToEEPROM(void)
{
    if (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 3, 10) != HAL_OK)
        return HAL_ERROR;

    uint32_t offset = 0;
    uint8_t *buf = (uint8_t*)modules;

    while (offset < MODULES_EEPROM_SIZE)
    {
        uint32_t remain = MODULES_EEPROM_SIZE - offset;
        uint32_t chunk  = (remain > EEPROM_PAGE_SIZE) ? EEPROM_PAGE_SIZE : remain;

        if (AT24_Write(EXTERN_EEPROM_ADDR + offset, buf + offset, chunk) != HAL_OK)
            return HAL_ERROR;

        HAL_Delay(AT24_WRITE_DELAY_MS);
        offset += chunk;
    }

    return HAL_OK;
}

HAL_StatusTypeDef Modules_LoadFromEEPROM(void)
{
    /* Проверка готовности EEPROM */
    if (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 3, 10) != HAL_OK)
        return HAL_ERROR;

    /* Чтение массива */
    if (AT24_Read(EXTERN_EEPROM_ADDR,(uint8_t *)modules,MODULES_EEPROM_SIZE) != HAL_OK)
		{
			return HAL_ERROR;
		}
    return HAL_OK;
}

/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



