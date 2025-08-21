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
/* ------------------------Locale variables----------------------------*/
 paramDev_t device[NUMBER_SLAVE_DEVICES]  = {0};
 
 struct
 {
	uint8_t addr;
	uint8_t dev_quan;
	uint16_t speed;
	char posit[NUMBER_SLAVE_DEVICES][20];
 } flash_struct;
 
 struct
 {
	 uint8_t ch_number;
	 uint16_t addr_reg;
	 uint16_t data_int16;
	 uint8_t qReg;
	 uint8_t data_reg[10];
	 uint8_t cal_status;
	 float fdata;
 } MsgDisplay;
 
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
void HandleDisplayCommands(uint8_t* displayresponse, uint8_t *arrDisplayRX, uint8_t *packet_ready, UART_HandleTypeDef *huart) {
    
	if (*packet_ready) {
        switch (*displayresponse) {
            case 0x01: // Модель
                break;
            case 0x02: // Единицы измерения
                break;
            case 0x03: // Диапазон измерения
                break;
            case 0x04: // Порог 1
                break;
            case 0x05: // Порог 2
                break;
            case 0x06: // Газ
                break;
            case 0x88: // Первый ответ после старта дисплея (0x88 0xFF 0xFF 0xFF)
                SendNextionCommand("Init.qDev.txt=\"%d\"", numberOfDevices); // Тестовая строка
                break;
            case 0x10: // Второй ответ после старта дисплея (0x10 0xFF 0xFF 0xFF)
                InitNextionDisplayWithDeviceData(numberOfDevices);
                break;
            case 0xA0: // Смена скорости UART
							 *displayresponse = 0x00; // reset cmd for display
                if (arrDisplayRX[1] >= 1 && arrDisplayRX[1] <= 6) {
                    //MB_BaudRateValue = getBaudrate(arrDisplayRX[1]);
                    //xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_1, eSetValueWithOverwrite);
                }
                osDelay(1);
                break;
            case 0xBB: // Обновление числа устройств
							   *displayresponse = 0x00; // reset cmd for display
                 numberOfDevices = getIntFromChar((char *)&arrDisplayRX[0], 5);
                break;
						case 0x35:
							*displayresponse = 0x00; 
							break;
						
						case 0x64: /* for Calibration Primary Zero, Калибровка  <<0>>*/ 
							 *displayresponse = 0x00; // reset cmd for display
							 break;
						case 0x68:/* for Calibration, Калибровка  "Точка 1" */ 
							 *displayresponse = 0x00; // reset cmd for display
							 break;
						
        }

				
        // Сброс флага и буфера
        *packet_ready = 0;
        memset(arrDisplayRX, 0, ARRAY_RX_SIZE);

        // Перезапуск UART-приёма
        HAL_UART_Receive_IT(huart, arrDisplayRX, 1);
    }
} 
	  
 /************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



