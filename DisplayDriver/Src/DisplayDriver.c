/**
******************************************************************************
* @file      DisplayDriver.c
* @author    OnWert
* @version   
* @brief     This file provides common functions for the  display Nexton 
*           
*/       
/* ------------------------Includes ----------------------------------*/
#include "DisplayDriver.h"
#include <stdarg.h>  // Для va_start, va_arg, va_end
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "cmsis_os.h"
#include "numberDevices.h"
#include "stdbool.h"
#include "UARTSlaveSettings.h"
#include "HoldingRegisterSlaveHandler.h"
#include "bsp.h"
/* ------------------------External variables -------------------------*/
extern UART_HandleTypeDef huart3;
extern osThreadId SlaveEventTaskHandle;
extern osThreadId SendToDispTaskHandle;
/* ------------------------Global variables----------------------------*/
 char arrDisplayTX[ARRAY_TX_SIZE] = {0};
 volatile uint8_t arrDisplayRX[ARRAY_RX_SIZE] = {0};
 volatile uint8_t startDisplayFlag = 0;
 
 uint8_t rxIdxDisplayUart = 0;
extern  uint8_t tx_buffer[ARRAY_TX_SIZE + 3]; // Основной буфер + 3 байта маркера конца
extern volatile uint16_t tx_index;
extern volatile uint16_t tx_size;
extern  uint8_t  numberOfDevices;

extern SensorState_t  SensorStateArray[NUMBER_SLAVE_DEVICES]; 
extern  SensorInfo_t  SensorInfo;
 uint8_t channelID = 0x00;
extern   uint32_t binary32;
 
 #define MAX_SIGNIFICANT_BYTES  10 // Максимум значимых байтов для сохранения 
 #define CHECK_3_ZEROS(arr) (arr[0] == 0 && arr[1] == 0 && arr[2] == 0)
 volatile uint16_t rx_index = 0;             
 volatile uint8_t displayResponse = 0;
 volatile uint32_t end_marker_counter = 0; 
 uint8_t significant_bytes_count = 0;
 uint8_t calibration_bytes_count = 0; 
 volatile uint8_t packet_ready = 0; 
 uint8_t significant_bytes[MAX_SIGNIFICANT_BYTES] = {0};
 float   updateThresholdWarning = 0.00;
 float   updateThresholdAlarm = 0.00;
 float   updateCalibrationValue = 0.00;
 

 DisplayCommand_t cmd;
extern  QueueHandle_t displayCommandQueue;

extern  volatile uint8_t disableThisFunctionForSetting;
/* ------------------------Locale variables----------------------------*/
 paramDev_t device[NUMBER_SLAVE_DEVICES]  = {0};
 
 struct
 {
	uint8_t addr;
	uint8_t dev_quan;
	uint16_t speed;
	char posit[NUMBER_SLAVE_DEVICES][20];
 } flash_struct;
 
/* ------------------------Functions-----------------------------------*/
 void Init_qDev(void){
	 flash_struct.dev_quan = 10;
   SendNextionCommand ("Init.qDev.txt=\"%d\"яяя", flash_struct.dev_quan);
 }
 
 void UART_Display_StartReceiving(void){
    
	 // Активируем прерывание по приёму первого байта
       HAL_UART_Receive_IT(&huart3, (uint8_t *)&arrDisplayRX[0], 1); 
 }

 void FixRussianEncodingForNextion(char *str) {
    for (; *str != '\0'; str++) {  // Проверка на конец строки
        if ((*str >= (char)0xC0) && (*str != (char)0xFF)) {
            *str -= 0x10;
        }
    }
}

// вспомогательная функцию для отправки команд
 void SendNextionCommand(const char* format, ...){
    va_list args;
    va_start(args, format);
    uint8_t len = vsnprintf((char*)tx_buffer, ARRAY_TX_SIZE, format, args);
    va_end(args);
    
    if(len >= ARRAY_TX_SIZE) {
        return; // Обработка переполнения буфера
    }
    
    FixRussianEncodingForNextion((char*)tx_buffer);
    
    // Добавляем маркер конца (3 байта 0xFF)
    tx_buffer[len] = 0xFF;
    tx_buffer[len+1] = 0xFF;
    tx_buffer[len+2] = 0xFF;
    
    tx_size = len + 3;
    tx_index = 0;
    
    // Включаем прерывание передачи
    huart3.Instance->CR1 |= USART_CR1_TXEIE;
    
    osDelay(40); // Задержка между командами
}

 void InitNextionDisplayWithDeviceData(uint8_t numberOfdevices){
 		
		  // Отправляем данные для каждого устройства
    for(uint8_t i = 1; i <= numberOfdevices ; i++) {
			
        uint8_t page = (i-1)/4;
        uint8_t pos =  (i - 1) % 4 + 1;    // Позиция на странице (1-4)
        
        // Форматируем числа с запятой вместо точки
        char value_str[20], scale_max_str[20], por1_str[20], por2_str[20];
			
        snprintf(value_str, sizeof(value_str), "%.2f", device[i].value);
        snprintf(scale_max_str, sizeof(scale_max_str), "%.2f", device[i].scaleMax);
        snprintf(por1_str, sizeof(por1_str), "%.2f", device[i].Porog1);
        snprintf(por2_str, sizeof(por2_str), "%.2f", device[i].Porog2);
        for(char* p = value_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = scale_max_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = por1_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = por2_str; *p; p++) if(*p == '.') *p = ',';
        
        // Отправляем все параметры устройства
			  SendNextionCommand ("page page%d", page);
        SendNextionCommand("page%d.ch%d.txt=\"Канал %d\"", page, pos, i);         //pos вместо i-4*page
        SendNextionCommand("page%d.val%d.txt=\"%s\"", page, pos, value_str);
        SendNextionCommand("page%d.gas%d.txt=\"%s\"", page, pos, device[i].gas);
        SendNextionCommand("page%d.ran%d.txt=\"%s\"", page, pos, scale_max_str);
        SendNextionCommand("page%d.unit%d.txt=\"%s\"", page, pos, device[i].scaleDimension);
        SendNextionCommand("page%d.por1%d.txt=\"%s\"", page, pos, por1_str);
        SendNextionCommand("page%d.por2%d.txt=\"%s\"", page, pos, por2_str);
        SendNextionCommand("page%d.mod%d.txt=\"%s\"", page, pos, device[i].model);
    }
 }

 void UpdateNextionDisplayWithChannelData(uint8_t SensorInfo_count){
	 
	  static uint8_t nextChannel = 1;
	 
    const uint8_t page = (nextChannel - 1) / 4;  // 4 канала на страницу
    const uint8_t pos = nextChannel - 4 * page;  // Элемент на странице (1..4)
	 
	  // Форматируем числа с запятой вместо точки
     char value_str[20], scale_max_str[20], por1_str[20], por2_str[20];
	  
	 // получение очередного активного modbus addr
	  uint8_t currentModbusIdx = SensorInfo.modbusAddrs[nextChannel - 1];
	 
	  snprintf(value_str, sizeof(value_str), "%.2f", SensorStateArray[currentModbusIdx - 1].Concentration);
	  snprintf(scale_max_str, sizeof(scale_max_str), "%.2f", SensorStateArray[currentModbusIdx - 1].SensorScaleMax);
    snprintf(por1_str,  sizeof(por1_str),  "%.2f", SensorStateArray[currentModbusIdx - 1].SensorWarning);
    snprintf(por2_str,  sizeof(por2_str),  "%.2f", SensorStateArray[currentModbusIdx - 1].SensorAlarm);
	  for(char* p = value_str; *p; p++) if(*p == '.') *p = ',';
	  for(char* p = scale_max_str; *p; p++) if(*p == '.') *p = ',';
    for(char* p = por1_str; *p; p++) if(*p == '.') *p = ',';
    for(char* p = por2_str; *p; p++) if(*p == '.') *p = ',';
	 
	   SendNextionCommand("page%d.val%d.txt=\"%s\"", page, pos, value_str);
		 SendNextionCommand("page%d.gas%d.txt=\"%s\"", page, pos, SensorStateArray[currentModbusIdx - 1].SensorGas);
	   SendNextionCommand("page%d.ran%d.txt=\"%s\"", page, pos, scale_max_str);
		 SendNextionCommand("page%d.poz%d.txt=\"%s\"", page, pos,device[nextChannel].posit);
	   SendNextionCommand("page%d.por1%d.txt=\"%s\"", page, pos, por1_str);
		 SendNextionCommand("page%d.por2%d.txt=\"%s\"", page, pos, por2_str);	
		 SendNextionCommand("page%d.unit%d.txt=\"%s\"", page, pos, SensorStateArray[currentModbusIdx - 1].SensorScaleDimension);
		 SendNextionCommand("page%d.mod%d.txt=\"%s\"", page, pos, SensorStateArray[currentModbusIdx - 1].DeviceModelCode);
			
		 nextChannel++;
		 
		if( nextChannel > SensorInfo_count)
		{
		  nextChannel = 1;
		}
		 
 }
 
void initDeviceData(uint8_t numberOfdevices)

   {
		 for(uint8_t i = 0; i <= numberOfdevices ; i++)
		  {
			  device[i].value = 00.00;
			  strncpy(device[i].gas, "gas", sizeof(device[i].gas));
				device[i].scaleMax = 50.00;
			  strncpy(device[i].scaleDimension, "dimension", sizeof(device[i].scaleDimension));
				device[i].Porog1 = 10.00;
				device[i].Porog2 = 20.00;
				strncpy(device[i].model, "---", sizeof(device[i].model));
			}
			
	 }
	 
// Пропускаем нули и находим первую цифру
 uint8_t getIntFromChar( char *inputString, uint8_t stringSize)
 {
	 int number = 0;
	for (int i = 0; i < stringSize; i++) {
			
		  if (inputString[i] == 0x00) continue; // Пропускаем нули
			 osDelay(1);
			// Если байт в диапазоне ASCII-цифр ('0'-'9')
			if (inputString[i] >= 0x30 && inputString[i] <= 0x39) {
					// Первая цифра: data[i] - '0', вторая: data[i+1] - '0'
					number = (inputString[i] - 0x30) * 10 + (inputString[i+1] - 0x30);
					break; // Выходим после обработки
			}
	 }
  return number;
}	 
	 

	/**
 * @brief Обрабатывает команды, полученные от дисплея.
 * @param displayResponse Код команды от дисплея.
 * @param arrDisplayRX Буфер с данными команды.
 * @param packet_ready Флаг готовности пакета (1 — данные получены, 0 — нет).
 * @param huart Указатель на UART-интерфейс для перезапуска приёма.
 */
void HandleDisplayCommands(uint8_t* displayresponse, uint8_t *arrDisplayRX, uint8_t *packet_ready) {
    
	if (*packet_ready) {
		
        switch (*displayresponse) {
            case DISPLAY_MODEL:
							  //disableThisFunctionForSetting = 1;
                break;
            case DISPLAY_SCALE_DIMENSION: 
							  //disableThisFunctionForSetting = 1;
                break; 
            case  DISPLAY_SCALE_MAX:   
								// disableThisFunctionForSetting = 1;	   
                break;
            case  DISPLAY_THRESHOLD_WARNING_TASK:
						  disableThisFunctionForSetting = 1;
              cmd.command = DISPLAY_THRESHOLD_WARNING_TASK;
              cmd.deviceAddr = SensorInfo.modbusAddrs[channelID - 1];
              cmd.binary32 = binary32;

              xQueueSend(displayCommandQueue, &cmd, portMAX_DELAY);
							
						break;	
            case  DISPLAY_THRESHOLD_ALARM: 
					    disableThisFunctionForSetting = 1;
						  cmd.command = DISPLAY_THRESHOLD_ALARM;
              cmd.deviceAddr = SensorInfo.modbusAddrs[channelID - 1];
              cmd.binary32 = binary32;

              xQueueSend(displayCommandQueue, &cmd, portMAX_DELAY);
								
							 break;	
            case DISPLAY_SUBSTANCE_CODE: 
                //disableThisFunctionForSetting = 1;						
                break;
            case 0x88: // Первый ответ после старта дисплея (0x88 0xFF 0xFF 0xFF)
							
                SendNextionCommand("Init.qDev.txt=\"%d\"", numberOfDevices); // Тестовая строка
                break;
            case 0x10: // Второй ответ после старта дисплея (0x10 0xFF 0xFF 0xFF)
                InitNextionDisplayWithDeviceData(numberOfDevices);
                break;
            case DISPLAY_BAUD_RATE_CMD: // Смена скорости UART
							
                if (arrDisplayRX[1] >= 1 && arrDisplayRX[1] <= 6) {
                    MB_BaudRateValue = getBaudrate(arrDisplayRX[1]);
                    xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_1, eSetValueWithOverwrite);
								    osDelay(1);	
                  } 
                break;
            case 0xBB: // Обновление числа устройств
							   //*displayresponse = 0x00; // reset cmd for display
                 numberOfDevices = getIntFromChar((char *)&arrDisplayRX[0], 5);
                break;
						case 0x35:
							
							break;		
						case DISPLAY_CALIBRATION_PRIMARY_ZERO: /* for Calibration Primary Zero, Калибровка  <<0>>*/ 
							
						    //xTaskNotify(SendToDispTaskHandle,DISPLAY_CALIBRATION_PRIMARY_ZERO, eSetValueWithOverwrite);
						    
							 break;
						case  DISPLAY_CALIBRATION_POINT_1:/* for Calibration, Калибровка  "Точка 1" */ 
							
						   //xTaskNotify(SendToDispTaskHandle,DISPLAY_CALIBRATION_POINT_1 , eSetValueWithOverwrite);
							 break;			
        }
        // Сброс флага и буфера
				*displayresponse = 0x00;    
        *packet_ready = 0x00;
         memset(arrDisplayRX, 0, ARRAY_RX_SIZE);
    }
} 
	  
void GetDisplayCmd(uint8_t inputByte){
           // Если буфер не переполнен
         if (rx_index < ARRAY_RX_SIZE - 1) {
					
              arrDisplayRX[rx_index++] = inputByte;  // Сохраняем байт в буфер

            // Проверяем, является ли текущий байт частью маркера конца (0xFF)
            if (inputByte == 0xFF) {
                end_marker_counter++;  // Увеличиваем счётчик подряд идущих 0xFF
            } 
						else {
                end_marker_counter = 0; // Сброс, если байт не 0xFF
							
							  // Сохраняем первые 5 значимых байтов
                if (significant_bytes_count < MAX_SIGNIFICANT_BYTES) {
                   significant_bytes[significant_bytes_count++] = inputByte;
                  }	
            }
							switch(significant_bytes_count) {
											case 1:
													displayResponse = significant_bytes[0];
													break;
													
											case 2:
													displayResponse = (significant_bytes[0] == 0x00) 
																				 ? significant_bytes[1] 
																				 : significant_bytes[0];
													break;
													
											case 5:
													if (CHECK_3_ZEROS(significant_bytes)) {
															displayResponse = 0xBB; // Количество устройств
													}
													break;
									}	
						
								// Обнаружение конца сообщения (3 0xFF подряд)
					if (end_marker_counter >= 3) {
							arrDisplayRX[rx_index - 3] = '\0';  // Удаляем маркер конца
							
							// Обновляем displayResponse
							if (significant_bytes_count > 0) {
									// Обработка специальных случаев
								
									if (arrDisplayRX[0] == DISPLAY_BAUD_RATE_CMD) {
											if (arrDisplayRX[1] >= 0x01 && arrDisplayRX[1] <= 0x06) { // Baud Rate change
													displayResponse = DISPLAY_BAUD_RATE_CMD;
											}
									}
						/* for Calibration Primary Zero, Калибровка  <<0>>*/ 
									else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x10 && arrDisplayRX[2] == DISPLAY_CALIBRATION_PRIMARY_ZERO)
									{
										channelID = arrDisplayRX[0];
										displayResponse = DISPLAY_CALIBRATION_PRIMARY_ZERO; // Calibration Primary Zero 	
									}
						/* for Calibration, Калибровка  "Точка 1" */ 
									else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x10 && arrDisplayRX[2] == DISPLAY_CALIBRATION_POINT_1 )
									{
										 displayResponse = DISPLAY_CALIBRATION_POINT_1 ; 		
										/*1. channel ID: arrDisplayRX[0]*/	
										 channelID = arrDisplayRX[0];
										/*2. significant_bytes_count with calibration value: "Точка 1"*/		
										 calibration_bytes_count = significant_bytes_count - 3;
									   uint8_t input[10] = {0};
										 memcpy(input,(void *)&arrDisplayRX[3], calibration_bytes_count);
										 sscanf((const char *)input, "%f", &updateCalibrationValue);
										 
									}	
						    /* Модель */			
								 else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x01 && arrDisplayRX[2] ==  DISPLAY_MODEL )
								  { 	
										 displayResponse =  DISPLAY_MODEL ; 										
										/*1. channel ID: arrDisplayRX[0]*/	
										 channelID = arrDisplayRX[0];	
									}				
					      /*         */				
                 else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x01 && arrDisplayRX[2] == DISPLAY_SCALE_DIMENSION )
									{
										 displayResponse = DISPLAY_SCALE_DIMENSION ; 										
										/*1. channel ID: arrDisplayRX[0]*/	
										 channelID = arrDisplayRX[0];		
									}					
					      /*         */						
								else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x01 && arrDisplayRX[2] == DISPLAY_SCALE_MAX )
									{
										 displayResponse = DISPLAY_SCALE_MAX ; 	
										
										/*1. channel ID: arrDisplayRX[0]*/	
										 channelID = arrDisplayRX[0];			
									}
									/*         */		
							  else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x01 && arrDisplayRX[2] == DISPLAY_THRESHOLD_WARNING )
									{
										 displayResponse = DISPLAY_THRESHOLD_WARNING_TASK ; 				
										/*1. channel ID: arrDisplayRX[0]*/	
										 channelID = arrDisplayRX[0];			
										 calibration_bytes_count = significant_bytes_count - 3;
									   uint8_t input[10] = {0};
										 memcpy(input,(void *)&arrDisplayRX[3], calibration_bytes_count);
										 sscanf((const char *)input, "%f", &updateThresholdWarning);
										  memcpy(&binary32, &updateThresholdWarning, sizeof(float));
									}
								/*         */			
								 else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x01 && arrDisplayRX[2] == DISPLAY_THRESHOLD_ALARM )
									{
										 displayResponse = DISPLAY_THRESHOLD_ALARM ; 		
										/*1. channel ID: arrDisplayRX[0]*/	
										 channelID = arrDisplayRX[0];
										 calibration_bytes_count = significant_bytes_count - 3;
									   uint8_t input[10] = {0};
										 memcpy(input,(void *)&arrDisplayRX[3], calibration_bytes_count);
										 sscanf((const char *)input, "%f", &updateThresholdAlarm);											
									   memcpy(&binary32, &updateThresholdAlarm, sizeof(float));
									}	
								/*         */			
									else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x01 && arrDisplayRX[2] == DISPLAY_SUBSTANCE_CODE )
									{
										 displayResponse = DISPLAY_SUBSTANCE_CODE ; 				
										/*1. channel ID: arrDisplayRX[0]*/	
										 channelID = arrDisplayRX[0];			
									}				
							}		
							packet_ready = 1;  // Флаг готовности пакета
							rx_index = 0;
							end_marker_counter = 0;
							significant_bytes_count = 0;
					}
        } else {
            // Переполнение буфера — сбрасываем
            rx_index = 0;
            end_marker_counter = 0;
        }
			}
/************************ (C) COPYRIGHT  OnWert *****END OF FILE****/