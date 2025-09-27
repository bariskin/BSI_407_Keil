/**
******************************************************************************
* @file      SensorLog.h
* @author    OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the TemplateFile.c
******************************************************************************
*/
#ifndef _TEMPLATE_FILE_H
#define _TEMPLATE_FILE_H

#ifdef __cplusplus
extern "C" {
#endif
  
 /* ------------------------Includes ----------------------------------*/
 #include "stm32f4xx_hal.h"
 #include "ff.h"          // Основной заголовок FatFS
 #include "diskio.h"      // Функции работы с диском
 #include "stdio.h"
 #include "stdlib.h"
 #include <string.h>

/** @defgroup sensor_log_file
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
 typedef RTC_TimeTypeDef Time_t;
 typedef RTC_DateTypeDef Date_t;
 

 // Определяем структуру времени
typedef struct {
    uint16_t year;    // 2024, 2025...
    uint8_t month;    // 1-12
    uint8_t day;      // 1-31
    uint8_t hour;     // 0-23
    uint8_t minute;   // 0-59
    uint8_t second;   // 0-59
} DateTime_t;
 
 
  typedef struct {
    uint32_t sensor_id;
    uint32_t value;
    DateTime_t timestamp;
 } SensorData_t;

typedef struct {
    char filename[64];
    FIL file;
    uint8_t is_open;
} LogFile_t;


// Структура лога для 
typedef struct {
    uint8_t sensorID;
    uint8_t deviceAddr;
    uint32_t Value;
} SensorLog_t;

 /* ------------------------External variables -------------------------*/
  

  /** @addtogroup sensor_log_file
  * @{
  */

  void GetLogFilePath(char* path, uint32_t sensor_id, DateTime_t* time);
  void GetCurrentTime(DateTime_t* time);
  FRESULT CreateSensorDirs(uint32_t sensor_id, DateTime_t* time);
  FRESULT WriteSensorLog(SensorData_t* data) ;
  void SensorDataCallback(uint32_t sensor_id, uint32_t value); 
  /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _TEMPLATE_FILE_H */