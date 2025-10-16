/**
******************************************************************************
* @file      SensorLogs.h
* @author    OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the TemplateFile.c
******************************************************************************
*/
#ifndef _SENSOR_LOG_H
#define _SENSOR_LOG_H

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
    uint32_t value;
    DateTime_t timestamp;
 } ServiceData_t;
 
 
 typedef struct {
    char filename[64];
    FIL file;
    uint8_t is_open;
 } LogFile_t;


 
 
 typedef enum{
    SERVICE = 1 ,
    CALIBRATION_0    ,
    CALIBRATION_1    ,
    ERROR_485,
	  THRESHOLD_WARNING,
	  THRESHOLD_ALARM,
	  THRESHOLD_ADDITIONAL,
	  OVER_THRESHOLD_WARNING,
	  OVER_THRESHOLD_ALARM,
	  OVER_THRESHOLD_ADDITIONAL,
	  NORMAL_LEVEL,
    SENSOR_LOG_TYPE_ERROR,
    REQUEST_LOGS	 
}  enSensorLog;


// Структура лога для 
typedef struct {
	  enSensorLog logType;
    uint8_t sensorID;
    uint8_t deviceAddr;
    uint32_t Value;
} SensorLogEvent_t;

 /* ------------------------External variables -------------------------*/
  

  /** @addtogroup sensor_log_file
  * @{
  */

  void GetLogFilePath(char* path, uint32_t sensor_id, DateTime_t* time);
  void GetServiceFilePath(char* path);
  void GetCurrentTime(DateTime_t* time);
  FRESULT CreateSensorDirs(uint32_t sensor_id);
  FRESULT WriteSensorLog(SensorData_t* data,enSensorLog log_type ) ;
  void SensorDataCallback(uint32_t sensor_id, uint32_t value,enSensorLog log_type);
  void ServiceDataCallback(uint32_t sensor_id, uint16_t value,enSensorLog log_type);
  FRESULT  CreateServiceDir(void);
  FRESULT WriteServiceLog(uint32_t sensor_id,ServiceData_t* data, enSensorLog log_type);
  const char* get_message(enSensorLog type); 
  void sendLogToQueue(float value, uint32_t sensor_id);
  FRESULT ReadServiceLine(char* buffer, uint16_t buffer_size, uint16_t line_number);
  FRESULT ReadLastServiceLine(char* buffer, uint16_t buffer_size);
  uint16_t GetServiceLinesCount(void);
  /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _SENSOR_LOG_H */
