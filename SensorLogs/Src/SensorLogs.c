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

FRESULT CreateSensorDirs(uint32_t sensor_id) {
    char dir_path[64];
    FRESULT res;
    
    // Создаем каждый уровень отдельно
    sprintf(dir_path, "%lu", sensor_id);
    res = f_mkdir(dir_path);
    if (res != FR_OK && res != FR_EXIST){
   			return res;
    }
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
    resFILE = CreateSensorDirs(data->sensor_id);
    
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


void ServiceDataCallback(uint32_t sensor_id, uint16_t value, enSensorLog log_type)
 {
	
	ServiceData_t servicedata;
	
  GetCurrentTime(&servicedata.timestamp);
	servicedata.value = value;
	 
	    // Записываем лог
   FRESULT res = WriteServiceLog(sensor_id, &servicedata, log_type);
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

 FRESULT WriteServiceLog(uint32_t sensor_id, ServiceData_t* data, enSensorLog log_type)
 {
    char filepath[64];
    FIL file;
    UINT bytes_written;
    char log_line[128];
	
	    // Проверка монтирования SD карты
    if (f_mount(&fs, "", 1) != FR_OK) {  // Проверка состояния
        return FR_NOT_READY;
    }
   // Создаем папку если нужно
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
	 
				const char* message  = get_message(log_type); 
    // Форматируем строку лога
		sprintf(log_line, "%04d.%02d.%02d %02d:%02d %s Канал %d:   %d\r\n",
	         data->timestamp.year, data->timestamp.month, data->timestamp.day,
           data->timestamp.hour, data->timestamp.minute, 
           message, sensor_id , data->value);
		
		resFILE = f_write(&file, log_line, strlen(log_line), &bytes_written);
    
    f_close(&file);
	 
		in_file_counter++;
		RdyWrittingFlag = 0;
		
    return resFILE;
 }



const char* get_message(enSensorLog type) {
	switch((uint8_t)type) {
	 case DEVICE_POWER:                return "Power";
	 case CALIBRATION_0:               return "Калибрование 0";
	 case CALIBRATION_1:               return "Калибрование 1";
	 case ERROR_485:                   return "ERROR_485";
	 case THRESHOLD_WARNING:           return "Порог1";
	 case THRESHOLD_ALARM:             return "Порог2";
	 case THRESHOLD_ADDITIONAL:        return "Порог3";
	 case OVER_THRESHOLD_WARNING:      return "Превышение 1";
	 case OVER_THRESHOLD_ALARM:        return "Превышение 2";
	 case OVER_THRESHOLD_ADDITIONAL:   return "Превышение 3";
	 
	 case SENSOR_LOG_TYPE_ERROR:  return "unknown";		 
	}
	return "unknown";
}

/**
  * @brief  Чтение строки из файла SERVICE/SERVICE.txt
  * @param  buffer: буфер для хранения прочитанной строки
  * @param  buffer_size: размер буфера
  * @param  line_number: номер строки для чтения (начиная с 1)
  * @retval FRESULT: результат операции
  */
FRESULT ReadServiceLine(char* buffer, uint16_t buffer_size, uint16_t line_number)
{
    char filepath[64];
    FIL file;
    FRESULT res;
    UINT bytes_read;
    char ch;
    uint16_t current_line = 1;
    uint16_t buf_pos = 0;
    uint32_t start_time = HAL_GetTick();
    
    // Проверка параметров
    if (buffer == NULL || buffer_size == 0 || line_number == 0) {
        return FR_INVALID_PARAMETER;
    }
    
    memset(buffer, 0, buffer_size);
    
    // Проверка SD карты
    if (f_mount(&fs, "", 1) != FR_OK) {
        return FR_NOT_READY;
    }
    
    GetServiceFilePath(filepath);
    
    res = f_open(&file, filepath, FA_READ);
    if (res != FR_OK) {
        return res;
    }
    
    // Читаем файл посимвольно
    while (current_line <= line_number) {
        // Проверка таймаута
        if (HAL_GetTick() - start_time > 1000) {
            f_close(&file);
            return FR_TIMEOUT;
        }
        
        // Читаем один символ
        res = f_read(&file, &ch, 1, &bytes_read);
        
        if (res != FR_OK || bytes_read == 0) {
            // Конец файла или ошибка
            f_close(&file);
            return FR_NO_FILE;
        }
        
        if (current_line == line_number) {
            // Записываем символ в буфер (исключая символы новой строки)
            if (ch != '\r' && ch != '\n' && buf_pos < buffer_size - 1) {
                buffer[buf_pos++] = ch;
            }
        }
        
        // Переход на новую строку
        if (ch == '\n') {
            current_line++;
            
            // Если достигли нужной строки и она закончилась
            if (current_line > line_number) {
                buffer[buf_pos] = '\0';
                f_close(&file);
                return FR_OK;
            }
            
            // Сбрасываем позицию буфера для новой строки
            buf_pos = 0;
        }
    }
    //RdyWrittingFlag = 0;
    f_close(&file);
    return FR_NO_FILE;
}
/**
  * @brief  Чтение последней строки из файла SERVICE/SERVICE.txt
  * @param  buffer: буфер для хранения прочитанной строки
  * @param  buffer_size: размер буфера
  * @retval FRESULT: результат операции
  */
FRESULT ReadLastServiceLine(char* buffer, uint16_t buffer_size)
{
    char filepath[64];
    FIL file;
    FRESULT res;
    char line_buf[128];
    char last_line[128] = "";
    
    // Проверка монтирования SD карты
    if (f_mount(&fs, "", 1) != FR_OK) {
        return FR_NOT_READY;
    }
    
    // Получаем путь к файлу
    GetServiceFilePath(filepath);
    
    // Открываем файл для чтения
    res = f_open(&file, filepath, FA_READ);
    if (res != FR_OK) {
        return res;
    }
    
    // Читаем все строки, сохраняя последнюю
    while (f_gets(line_buf, sizeof(line_buf), &file) != 0) {
        if (strlen(line_buf) > 0) {
            strncpy(last_line, line_buf, sizeof(last_line) - 1);
            last_line[sizeof(last_line) - 1] = '\0';
        }
				osDelay(1);
    }
    
    f_close(&file);
    
    // Если нашли строку, копируем в буфер
    if (strlen(last_line) > 0) {
        // Убираем символы возврата каретки и новой строки
        char* newline_pos = strchr(last_line, '\r');
        if (newline_pos) *newline_pos = '\0';
        
        newline_pos = strchr(last_line, '\n');
        if (newline_pos) *newline_pos = '\0';
        
        strncpy(buffer, last_line, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
        return FR_OK;
    }
    
    return FR_NO_FILE;
}

/**
  * @brief  Получение количества строк в файле SERVICE/SERVICE.txt
  * @retval uint16_t: количество строк
  */
uint16_t GetServiceLinesCount(void)
{
    char filepath[64];
    FIL file;
    FRESULT res;
    char line_buf[128];
    uint16_t line_count = 0;
    
    // Проверка монтирования SD карты
    if (f_mount(&fs, "", 1) != FR_OK) {
        return 0;
    }
    
    // Получаем путь к файлу
    GetServiceFilePath(filepath);
    
    // Открываем файл для чтения
    res = f_open(&file, filepath, FA_READ);
    if (res != FR_OK) {
        return 0;
    }
    
    // Считаем строки
    while (f_gets(line_buf, sizeof(line_buf), &file) != 0) {
        line_count++;
    }
    
    f_close(&file);
		
		//RdyWrittingFlag = 0;
    
		return line_count;
}
	
/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



