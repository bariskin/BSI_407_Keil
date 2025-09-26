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

/* ------------------------External variables -------------------------*/
extern FATFS fs;  // file system
extern FIL fil; // File
extern FILINFO fno;
extern FRESULT fresult;  // result
extern UINT br, bw;  // File read/write count
/* ------------------------Global variables----------------------------*/
DateTime_t current_time;

extern RTC_HandleTypeDef hrtc;
/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/
void GetLogFilePath(char* path, uint32_t sensor_id, DateTime_t* time) {
    sprintf(path, "/sensor_logs/sensor_%03d/%04d_%02d/%04d_%02d_%02d.log", 
            sensor_id, time->year, time->month, time->year, time->month, time->day);
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
    char dir_path[128];
    FRESULT res;
    
    // Создаем папку для датчика
    sprintf(dir_path, "/sensor_logs/sensor_%03d", sensor_id);
    res = f_mkdir(dir_path);
    if (res != FR_OK && res != FR_EXIST) return res;
    
    // Создаем папку для месяца
    sprintf(dir_path, "/sensor_logs/sensor_%03d/%04d_%02d", 
            sensor_id, time->year, time->month);
    res = f_mkdir(dir_path);
    if (res != FR_OK && res != FR_EXIST) return res;
    
    return FR_OK;
}

FRESULT WriteSensorLog(SensorData_t* data) {
    char filepath[128];
    FIL file;
    FRESULT res;
    UINT bytes_written;
    char log_line[256];
	
	    // Проверка монтирования SD карты
    if (f_mount(&fs, "", 1) != FR_OK) {  // Проверка состояния
        return FR_NOT_READY;
    }
    // Формируем путь к файлу
    GetLogFilePath(filepath, data->sensor_id, &data->timestamp);
    
    // Создаем папки если нужно
    res = CreateSensorDirs(data->sensor_id, &data->timestamp);
    if (res != FR_OK) return res;
    
    // Открываем файл для добавления (создаем если не существует)
    res = f_open(&file, filepath, FA_WRITE | FA_OPEN_ALWAYS);
    if (res != FR_OK) return res;
    
    // Перемещаемся в конец файла
    f_lseek(&file, f_size(&file));
    
    // Форматируем строку лога
    sprintf(log_line, "[%04d-%02d-%02d %02d:%02d:%02d] %.3f\r\n",
            data->timestamp.year, data->timestamp.month, data->timestamp.day,
            data->timestamp.hour, data->timestamp.minute, data->timestamp.second,
            data->value);
    
    // Записываем в файл
    res = f_write(&file, log_line, strlen(log_line), &bytes_written);
    
    f_close(&file);
    return res;
}

void SensorDataCallback(uint32_t sensor_id, float value) {
    SensorData_t sensor_data;
    
    // Заполняем структуру данных
    sensor_data.sensor_id = sensor_id;
    sensor_data.value = value;
    
    // Получаем текущее время
    GetCurrentTime(&sensor_data.timestamp);
    
    // Записываем лог
    FRESULT res = WriteSensorLog(&sensor_data);
    if (res != FR_OK) {
        printf("Error writing log: %d\n", res);
    }
}


/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



