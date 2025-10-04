/**
******************************************************************************
* @file      SensorLogs.c
* @author    OnWert
* @version   
* @brief     This file provides common functions for the   SensorLogs
*           
*/       
/* ------------------------Includes ----------------------------------*/
#include "SensorLogs.h"
#include "cmsis_os.h"
uint16_t in_file_counter = 0;
/* ------------------------External variables -------------------------*/
extern FATFS fs;  // file system
extern FIL fil; // File
extern FILINFO fno;
extern FRESULT fresult;  // result
extern UINT br, bw;  // File read/write count
/* ------------------------Global variables----------------------------*/
DateTime_t current_time;

extern RTC_HandleTypeDef hrtc;

extern uint8_t RdyWrittingFlag ;
/* ------------------------Locale variables----------------------------*/
FRESULT resFILE;
/* ------------------------Functions-----------------------------------*/
void GetLogFilePath(char* path, uint32_t sensor_id, DateTime_t* time) {
    sprintf(path, "%lu/LOG_%lu.txt", 
            sensor_id,sensor_id);
}

void GetServiceFilePath(char* path) {
    sprintf(path, "SERVICE/SERVICE.txt");
}

void GetCurrentTime(DateTime_t* time) {
    RTC_DateTypeDef date;
    RTC_TimeTypeDef rtc_time;
    
    HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    
    time->year = date.Year + 2000;    // RTC обычно хранит год как 0-99
    time->month = date.Month;
    time->day = date.Date;
    time->hour = rtc_time.Hours;
    time->minute = rtc_time.Minutes;
    time->second = rtc_time.Seconds;
}

FRESULT CreateSensorDirs(uint32_t sensor_id, DateTime_t* time) {
    char dir_path[64];
    FRESULT res22;
    
    // Создаем каждый уровень отдельно
    sprintf(dir_path, "%lu", sensor_id);
    res22 = f_mkdir(dir_path);
    if (res22 != FR_OK && res22 != FR_EXIST) return res22;
    
   // sprintf(dir_path, "sensor_%lu/%04d", sensor_id, time->year);
   // res22 = f_mkdir(dir_path);
   // if (res22 != FR_OK && res22 != FR_EXIST) return res22;
    
   // sprintf(dir_path, "sensor_%lu/%04d/%02d", sensor_id, time->year, time->month);
  //  res22 = f_mkdir(dir_path);
  //  if (res22 != FR_OK && res22 != FR_EXIST) return res22;
    
    //sprintf(dir_path, "sensor_%lu/%04d/%02d/%02d", 
    //        sensor_id, time->year, time->month, time->day);
   // res22 = f_mkdir(dir_path);
    //if (res22 != FR_OK && res22 != FR_EXIST) return res22;
    
    return FR_OK;
}

FRESULT WriteSensorLog(SensorData_t* data,enSensorLog log_type ) {
	
    char filepath[64];
    FIL file;
    
    UINT bytes_written;
    char log_line[64];
	
	    // Проверка монтирования SD карты
    if (f_mount(&fs, "", 1) != FR_OK) {  // Проверка состояния
        return FR_NOT_READY;
    }
  
    // Создаем папки если нужно
    resFILE = CreateSensorDirs(data->sensor_id, &data->timestamp);
    
		if (resFILE != FR_OK) 
		{
			return resFILE;
    }
		
		 // Формируем путь к файлу
   GetLogFilePath(filepath, data->sensor_id, &data->timestamp);
	  
 
  resFILE = f_open(&file, filepath, FA_WRITE | FA_OPEN_APPEND | FA_OPEN_ALWAYS);
   if (resFILE != FR_OK) {
        return resFILE; 
  }
		const char* message  = get_message(log_type); 
    // Форматируем строку лога
	sprintf(log_line, "%04d.%02d.%02d %02d:%02d %s: %d\r\n",
	         data->timestamp.year, data->timestamp.month, data->timestamp.day,
           data->timestamp.hour, data->timestamp.minute, 
           message, data->value);
		
    // Записываем в файл
    resFILE = f_write(&file, log_line, strlen(log_line), &bytes_written);
    
    f_close(&file);
	 
		in_file_counter++;
		RdyWrittingFlag = 0;
		
    return resFILE;
}

void SensorDataCallback(uint32_t sensor_id, uint32_t value,enSensorLog log_type ) {
    SensorData_t sensor_data;
    
    // Заполняем структуру данных
    sensor_data.sensor_id = sensor_id;
    sensor_data.value = value;
    
    // Получаем текущее время
    GetCurrentTime(&sensor_data.timestamp);
    
    // Записываем лог
    FRESULT res = WriteSensorLog(&sensor_data, log_type);
    if (res != FR_OK) {
 
    }
}


FRESULT  CreateServiceDir(void)
 {
   char dir_path[32];
   FRESULT res;
	 
	 snprintf(dir_path, sizeof(dir_path), "SERVICE");
   res = f_mkdir(dir_path);
	 
   if (res == FR_OK ||res == FR_EXIST) 
	 {  
	  return FR_OK;
	 }
	 // Возвращаем ошибку только в случае других проблем
    return res;
 }

FRESULT WriteServiceLog(ServiceData_t* data)
 {
    char filepath[64];
    FIL file;
    UINT bytes_written;
    char log_line[128];
	
	    // Проверка монтирования SD карты
    if (f_mount(&fs, "", 1) != FR_OK) {  // Проверка состояния
        return FR_NOT_READY;
    }
   // Создаем папкe если нужно
    resFILE = CreateServiceDir();
    
		if (resFILE != FR_OK) 
		{
			return resFILE;
    }
		/* путь к папке SERVICE */
   GetServiceFilePath(filepath);
		
	 resFILE = f_open(&file, filepath, FA_WRITE | FA_OPEN_APPEND | FA_OPEN_ALWAYS);
   if (resFILE != FR_OK) {
        return resFILE;
    }
	 
		 // Форматируем строку лога
    sprintf(log_line, "%04d.%02d.%02d %02d:%02d Количество датчиков: %d\r\n",
	         data->timestamp.year, data->timestamp.month, data->timestamp.day,
           data->timestamp.hour, data->timestamp.minute, 
           data->value);
		
		resFILE = f_write(&file, log_line, strlen(log_line), &bytes_written);
    
    f_close(&file);
	 
		in_file_counter++;
		RdyWrittingFlag = 0;
		
    return resFILE;
 }

void ServiceDataCallback(uint16_t value)
 {
	
	ServiceData_t servicedata;
	
  GetCurrentTime(&servicedata.timestamp);
	servicedata.value = value;
	 
	    // Записываем лог
   FRESULT res = WriteServiceLog(&servicedata);
    if (res != FR_OK) {
        //printf("Error writing log: %d\n", res);
    }  
 }

const char* get_message(enSensorLog type) {
	switch(type) {
	 case DEVICE_POWER:                return "test";
	 case CALIBRATION_0:               return "Калибрование 0";
	 case CALIBRATION_1:               return "Калибрование 1";
	 case ERROR_485:                   return "ERROR_484";
	 case THRESHOLD_WARNING:           return "Порог1";
	 case THRESHOLD_ALARM:             return "Порог2";
	 case THRESHOLD_ADDITIONAL:        return "Порог3";
	 case OVER_THRESHOLD_WARNING:      return "Превышение 1";
	 case OVER_THRESHOLD_ALARM:        return "Превышение 2";
	 case OVER_THRESHOLD_ADDITIONAL:   return "Превышение 3";
	 
	 case SENSOR_LOG_TYPE_ERROR:  return "unknown";		 
	}
}
void sendLogToQueue(float value, uint32_t sensor_id)
 {
 
 
 
 }	
/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



