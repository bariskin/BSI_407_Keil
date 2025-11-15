/**
******************************************************************************
* @file      SensorLogs.c
* @author    OnWert
* @version   
* @brief     This file provides common functions for the   SensorLogs          
*/       
/* ------------------------Includes ----------------------------------*/
#include "SensorLogs.h"
#include "cmsis_os.h"
#include "stdbool.h"
uint16_t in_file_counter = 0;
/* ------------------------External variables -------------------------*/
 FATFS fs;  // file system
 FIL fil; // File
 FILINFO fno;
 FRESULT fresult;  // result
 UINT br, bw;  // File read/write count
/* ------------------------Global variables----------------------------*/
DateTime_t current_time;

extern RTC_HandleTypeDef hrtc;

extern uint8_t RdyWrittingFlag ;

bool sd_card_present = false;
/* ------------------------Locale variables----------------------------*/
FRESULT resFILE;
/* ------------------------Functions-----------------------------------*/
void GetLogFilePath(char* path, uint32_t sensor_id, DateTime_t* time) {
    sprintf(path, "%lu/LOG_%lu.txt", 
            sensor_id,sensor_id);
}

void GetServiceFilePath(char* path) {
    RTC_DateTypeDef date;
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
    
    sprintf(path, "%02d_%02d_%02d.txt", date.Year, date.Month, date.Date);
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
void ServiceDataCallback(uint32_t sensor_id, uint16_t value, enSensorLog log_type)
 {
	
	ServiceData_t servicedata;
	servicedata.value = value;
	 
	    // Записываем лог
   FRESULT res = WriteServiceLog(sensor_id, &servicedata, log_type);
    if (res != FR_OK) {

    }  
 }

const char* get_message(enSensorLog type) {
	switch((uint8_t)type) {
	 case SERVICE:                     return "Сервис";
	 case CALIBRATION_0:               return "Калибрование 0";
	 case CALIBRATION_1:               return "Калибрование 1";
	 case ERROR_485:                   return "ERROR_485";
	 case THRESHOLD_WARNING:           return "Порог1";
	 case THRESHOLD_ALARM:             return "Порог2";
	 case THRESHOLD_ADDITIONAL:        return "Порог3";
	 case OVER_THRESHOLD_WARNING:      return "Превышение 1";
	 case OVER_THRESHOLD_ALARM:        return "Превышение 2";
	 case OVER_THRESHOLD_ADDITIONAL:   return "Превышение 3";
	 case NORMAL_LEVEL:                return "Нормальный уровень";
	 case SENSOR_LOG_TYPE_ERROR:       return "unknown";		 
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
  * @brief  Получение количества строк в файле
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
		
		return line_count;
}
	

/**
 * @brief Проверка наличия SD карты
 * @return true - карта присутствует, false - отсутствует
 */
bool check_sd_card(void) {
    if (f_mount(&fs, "0:/", 1) == FR_OK) {
        f_mount(NULL, "0:/", 0); // Размонтировать
        return true;
    }
    return false;
}

FRESULT CreateDatedFile(FIL* file)
{
    char file_path[64];
    FRESULT res;
    
		    /* ВСЕГДА получаем актуальную дату при каждой записи */
    RTC_TimeTypeDef time;
	  RTC_DateTypeDef date;
	  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  

    // Формируем путь с текущей датой
    snprintf(file_path, sizeof(file_path), "%02d_%02d_%02d.txt", 
             date.Year, date.Month, date.Date);
		
		
       // Пытаемся открыть файл для добавления (если существует)
    res = f_open(file, file_path, FA_OPEN_APPEND | FA_WRITE);
    
    // Если файл не существует, создаем новый
    if (res == FR_NO_FILE) {
        res = f_open(file, file_path, FA_CREATE_NEW | FA_WRITE);
    } 
    return res;
}

FRESULT WriteServiceLog(uint32_t sensor_id, ServiceData_t* data, enSensorLog log_type)
{
    char file_path[64];
    FIL file;
    UINT bytes_written;
    char log_line[128];
    FRESULT res;
    static uint8_t last_hour = 0;
    // Проверка монтирования SD карты
    if (f_mount(&fs, "", 1) != FR_OK) {
        return FR_NOT_READY;
    }
    /* ВСЕГДА получаем актуальную дату при каждой записи */
		RTC_TimeTypeDef sTime;
    RTC_DateTypeDef date;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	
    /* ИСПРАВЛЕНИЕ ВРЕМЕНИ 24:00 -> 00:00 */
    uint8_t corrected_hours = sTime.Hours;
    if (corrected_hours >= 24) {
        corrected_hours = 0;
        sTime.Hours = 0;
			  sTime.TimeFormat = RTC_HOURFORMAT_24;
        // Принудительно обновляем время в RTC
        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
			  osDelay(2);
    }
		 /* АВТОМАТИЧЕСКАЯ СМЕНА ДАТЫ В 00:00 */
    if (corrected_hours == 0 && last_hour == 23) {
        date.Date++;
        if (date.Date > 31)date.Date = 1;
			
			  HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
			  osDelay(2);
    }
    last_hour = corrected_hours;
		
    // Формируем путь с текущей датой
    snprintf(file_path, sizeof(file_path), "%02d_%02d_%02d.txt", 
             date.Year, date.Month, date.Date);
    
    // Открываем файл текущей даты
    res = f_open(&file, file_path, FA_OPEN_APPEND | FA_WRITE);
    
		  // Если файл не существует, создаем новый
    if (res == FR_NO_FILE) {
        res = f_open(&file, file_path, FA_CREATE_NEW | FA_WRITE);
    }
    
    
    // Формируем строку лога
    const char* message = get_message(log_type);
    
    if(log_type == SERVICE) {
        sprintf(log_line, "%02d.%02d.%02d %02d:%02d Приборов: %d\r\n",
			            date.Year,date.Month,date.Date,
                  sTime.Hours, sTime.Minutes, data->value);  // ? Используем time от RTC ?
    }
    else if (log_type == ERROR_485) {
        sprintf(log_line, "%02d.%02d.%02d %02d:%02d %s Канал %d \r\n",
			         date.Year,date.Month,date.Date,
               sTime.Hours, sTime.Minutes, message, sensor_id); 
    }
    else {
        sprintf(log_line, "%02d.%02d.%02d %02d:%02d %s Канал %d: %d\r\n",
			          date.Year,date.Month,date.Date,
                sTime.Hours, sTime.Minutes, message, sensor_id, data->value); 
    }
    
    // Пишем в файл
		if (res == FR_OK){
      res = f_write(&file, log_line, strlen(log_line), &bytes_written);
	  }
    f_close(&file);
    
    in_file_counter++;
    RdyWrittingFlag = 0;
    
    return res;
}
/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/