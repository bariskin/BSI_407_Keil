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

#define MODULES_EEPROM_ADDR   0x0000

#define RELAY_REACTION_SIZE_BYTES  (EVENT_COUNT * MAX_CHANNELS)

#define RELAY_ID_SIZE 1
#define RELAY_SIZE_BYTES  (RELAY_ID_SIZE + RELAY_REACTION_SIZE_BYTES)

#define RELAY_MODULE_ID_SIZE 1
#define RELAY_MODULE_SIZE_BYTES  (RELAY_MODULE_ID_SIZE + (RELAY_SIZE_BYTES * RELAY_PER_MODULE))


#define MODULES_EEPROM_SIZE   (RELAY_MODULE_SIZE_BYTES * MODULE_COUNT)
uint16_t debugSIZE = MODULES_EEPROM_SIZE;
/* ------------------------External variables -------------------------*/
extern RelayModule modules[MODULE_COUNT];
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
        //vTaskDelay(pdMS_TO_TICKS(AT24_WRITE_DELAY_MS));
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

/* ================== SELF-TEST ================== */

//static const uint8_t testPattern[TEST_SIZE] = {
//    0x55, 0xAA, 0x11, 0x22, 0x33, 0x44, 0x77, 0x88,
//    0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
//    0xFE, 0xED, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE,
//    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
//};

//HAL_StatusTypeDef AT24_SelfTest(void)
//{
//    uint8_t rx[TEST_SIZE];

//    /* 1. Проверка ACK */
//    if (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 3, 10) != HAL_OK)
//        return HAL_ERROR;

//    /* 2. Запись тестового шаблона */
//    if (AT24_Write(TEST_ADDR, (uint8_t *)testPattern, TEST_SIZE) != HAL_OK)
//        return HAL_ERROR;

//    /* 3. Небольшая пауза */
//    HAL_Delay(AT24_WRITE_DELAY_MS);

//    /* 4. Чтение */
//    if (AT24_Read(TEST_ADDR, rx, TEST_SIZE) != HAL_OK)
//        return HAL_ERROR;

//    /* 5. Сравнение */
//    if (memcmp(testPattern, rx, TEST_SIZE) != 0)
//        return HAL_ERROR;

//    return HAL_OK;
//}

#define EEPROM_PAGE_SIZE      64

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

        if (AT24_Write(TEST_ADDR + offset, buf + offset, chunk) != HAL_OK)
            return HAL_ERROR;

        HAL_Delay(AT24_WRITE_DELAY_MS);
        offset += chunk;
    }

    return HAL_OK;
}



//HAL_StatusTypeDef Modules_SaveToEEPROM(void)
//{
//    /* Проверка готовности EEPROM */
//    if (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 3, 10) != HAL_OK)
//        return HAL_ERROR;

//    /* Запись массива */
//    if (AT24_Write(TEST_ADDR,(uint8_t *)modules,MODULES_EEPROM_SIZE) != HAL_OK)
//	 	{    
//   		return HAL_ERROR;
//		}
//    /* Ожидание завершения записи */
//    HAL_Delay(AT24_WRITE_DELAY_MS);

//    return HAL_OK;
//}

HAL_StatusTypeDef Modules_LoadFromEEPROM(void)
{
    /* Проверка готовности EEPROM */
    if (HAL_I2C_IsDeviceReady(&hi2c2, AT24_I2C_ADDR, 3, 10) != HAL_OK)
        return HAL_ERROR;

    /* Чтение массива */
    if (AT24_Read(TEST_ADDR,(uint8_t *)modules,MODULES_EEPROM_SIZE) != HAL_OK)
		{
			return HAL_ERROR;
		}
    return HAL_OK;
}

/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



