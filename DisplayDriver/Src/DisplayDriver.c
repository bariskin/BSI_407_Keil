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
#include <stdarg.h>  // ƒл€ va_start, va_arg, va_end
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "cmsis_os.h"
#include "numberDevices.h"
#include "stdbool.h"
/* ------------------------External variables -------------------------*/
extern UART_HandleTypeDef huart3;
/* ------------------------Global variables----------------------------*/
 char arrDisplayTX[ARRAY_TX_SIZE] = {0};
 volatile char arrDisplayRX[ARRAY_RX_SIZE] = {0};
 uint8_t rxIdxDisplayUart = 0;
 
/* ------------------------Locale variables----------------------------*/
 enum {
	FALSE,
	TRUE,
	CRC_OK,
	RX_OK = 2,
//	CHANNEL_LINE,
//	DEVICE_LINE,
//	RX_OK = 2,
	RX_FAULT = 1
};
 
struct paramDev{
	uint8_t channelIndex;
	uint16_t status;
	char model[20];
	char posit[20];
	char gas[20];
	float scaleMax;
	char scaleDimension[20];
	float value;
	float current4_20;									  	// петл€ 4-20 mA
	float currentConsumptoin;								// ток потреблени€
	float Porog1;
	float Porog2;
	uint8_t error;
	uint8_t errorRS;
	uint32_t status_GA;
	uint32_t value_uint32;
	uint32_t scaleMax_uint32;					
	uint8_t scaleDimension_hex;					
	uint32_t Porog1_int32;							
	uint32_t Porog2_int32;							
	uint32_t model_hex;								  
}device[NUMBER_SLAVE_DEVICES];
 
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
   SendNextionCommand ("Init.qDev.txt=\"%d\"€€€", flash_struct.dev_quan);
 }
 
 void UART_Display_StartReceiving(void){
    rxIdxDisplayUart = 0;          // —брос индекса
    HAL_UART_Receive_IT(&huart3, (uint8_t *)&arrDisplayRX[rxIdxDisplayUart], 1);  // «апуск приЄма 1 байта
 }
 
 /**
  * @brief Processes incoming UART messages from Nextion display
  * @param data Pointer to received data buffer
  * @return int8_t - 0 if processed successfully, message byte for short messages
  */
uint8_t Nextion_ParseMsgAndExecute(uint8_t* data){
    uint8_t len;
	 	uint16_t data_int16;
	  uint32_t fdata_int32;
	 
    // обработка сообщени€ от диспле€
    for (uint8_t i = 0; i < ARRAY_RX_SIZE; i++)
    {   
			// Find message end marker (0xFF 0xFF 0xFF)
        if ((data[i] == 0xFF) && (data[i + 1] == 0xFF) && (data[i + 2] == 0xFF))
        {
		    	 // Clear remaining bytes (checksum/error fields)
			     for(uint8_t j = i; j < ARRAY_RX_SIZE; j++)
			       {
			        data[j] = 0x00;
			       }
        // Short message processing (single byte)
            if (i == 1)
            {
                return data[0];
            }
					 // long message processing 
        	else 
        	 {
        		
     
           	uint8_t ch_number = data[0];
           	uint8_t comand_number = data[1];
           	uint8_t addr_reg = data[2];

        		uint8_t data_reg[10];
        		char* data_end[10];
        		float fdata;
						 
            // Extract float value if present
            if (i > 3)  // Ensure there's data to parse
            {          
           		fdata = strtof((char*)&data[3], data_end);
						}
						
						switch (comand_number) {

							case 0x00:  // Set device quantity
								flash_struct.dev_quan = (uint8_t)fdata;
								//HAL_I2C_Mem_Write(&hi2c2, 0x56 << 1, 1,I2C_MEMADD_SIZE_16BIT, (uint8_t*)&flash_struct.dev_quan, 1, 100);
								break;

							case 0x01:  // Device configuration
							 
								switch (addr_reg) {
									case 0x01:
							
										len = sizeof(device[ch_number].posit);
										for(uint8_t i = 0; i < len; i++)				device[ch_number].posit[i] = 0;
										for(uint8_t i = 0; data[3+i]!='\0'; i++)	device[ch_number].posit[i] = data[3+i];
										break;
									case 0x02:
										len = sizeof(device[ch_number].scaleDimension);
										for(uint8_t i = 0; i < len; i++)				device[ch_number].scaleDimension[i] = 0;
										for(uint8_t i = 0; data[3+i]!='\0'; i++)	device[ch_number].scaleDimension[i] = data[3+i];
										break;
									case 0x03:
										device[ch_number].scaleMax = fdata;
										break;
									case 0x04:
										device[ch_number].Porog1 = fdata;
										break;
									case 0x05:
										device[ch_number].Porog2 = fdata;
										break;
									case 0x06:
										len = sizeof(device[ch_number].gas);
										for(uint8_t i=0; i < len; i++)				device[ch_number].gas[i] = 0;
										for(uint8_t i=0; data[3+i]!='\0'; i++)	device[ch_number].gas[i] = data[3+i];
										break;
									case 0xfe:
										// команда записи данных в модуль канала
										MsgDisplay.cal_status = 0xfe;
										MsgDisplay.ch_number = ch_number;
										break;
									default:
										break;
								}
								break;

							case 0x06:
							
								data_int16 = fdata * 100;

								MsgDisplay.ch_number = (device[ch_number].channelIndex-1)/4+1;
								MsgDisplay.addr_reg = addr_reg + 1000*((device[ch_number].channelIndex-1)%4);
								MsgDisplay.data_int16 = data_int16;
								MsgDisplay.cal_status = 0x06;
								break;

							case 0x10:
								
								fdata_int32 = *(uint32_t*) &fdata;

								data_reg[0] = fdata_int32 >> 24;
								data_reg[1] = fdata_int32 >> 16;
								data_reg[2] = fdata_int32 >> 8;
								data_reg[3] = fdata_int32;

								if(device[ch_number].channelIndex != 0)
								{
									MsgDisplay.ch_number = (device[ch_number].channelIndex-1)/4+1;
									MsgDisplay.addr_reg = addr_reg + 1000*((device[ch_number].channelIndex-1)%4);
									for(uint8_t j=0; j<4; j++)
									{
										MsgDisplay.data_reg[j] = data_reg[j];
									}
								}
								else
								{
									MsgDisplay.ch_number = ch_number;
									MsgDisplay.addr_reg = addr_reg;
									MsgDisplay.fdata = fdata;
								}
								MsgDisplay.qReg = 2;
								MsgDisplay.cal_status = 0x10;

								break;

							default:
								break;
						}
        	}
        }
    }

		#ifdef	DEBUGING
		  uint8_t a
		  a = snprintf(arrDisplay, DISP_TX_SIZE, "Display code: %d \n", arrDisplayRX[0]);
		  USART_Transmit(USART_DEBUG, 6, (uint8_t*)arrDisplay, a);
		#endif

	return 0;
 
 }
 
 /**
  * @brief ќтправка данных через UART (неблокирующий режим с прерывани€ми)
  * @param huart ”казатель на структуру UART_HandleTypeDef
  * @param data ”казатель на массив данных дл€ отправки
  * @param size  оличество байт дл€ отправки
  * @retval HAL_StatusTypeDef —татус операции
  */
HAL_StatusTypeDef USART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size){
    if(huart == NULL || data == NULL || size == 0) {
        return HAL_ERROR;
    }
    
    return HAL_UART_Transmit_IT(huart, data, size);
}

 
 void FixRussianEncodingForNextion(char *str) {
    for (; *str != '\0'; str++) {  // ѕроверка на конец строки
        if ((*str >= (char)0xC0) && (*str != (char)0xFF)) {
            *str -= 0x10;
        }
    }
}

// ќбъ€вл€ем вспомогательную функцию дл€ отправки команд
 void SendNextionCommand(const char* format, ...){
    va_list args;
    va_start(args, format);
    uint8_t len = vsnprintf(arrDisplayTX, ARRAY_TX_SIZE, format, args);
    va_end(args);
    
    if(len >= ARRAY_TX_SIZE) {
        // ќбработка переполнени€ буфера
        return;
    }
    
    FixRussianEncodingForNextion(arrDisplayTX);
    //USART_Transmit(USART_DISPLAY, 3, (uint8_t*)arrDisplay, len);
		USART_Transmit_IT(&huart3, (uint8_t *)arrDisplayTX, len);
    osDelay(50);  // —тандартна€ задержка между командами
}

 void InitNextionDisplayWithDeviceData(void){
 
	  while( arrDisplayRX[0] !=  0x30) // ???
	  {
		  SendNextionCommand("page page0€€€");
		  osDelay(500);
	  }
		
		  // ќтправл€ем данные дл€ каждого устройства
    for(uint8_t i = 1; i <= flash_struct.dev_quan; i++) {
			
        if(device[i].status != RX_OK) 
				 {
					 continue;
         }
        uint8_t page = (i-1)/4;
        uint8_t pos =  i - 4 * page;
        
        // ‘орматируем числа с зап€той вместо точки
        char value_str[20], scale_max_str[20], por1_str[20], por2_str[20];
			
        snprintf(value_str, sizeof(value_str), "%.2f", device[i].value);
        snprintf(scale_max_str, sizeof(scale_max_str), "%.2f", device[i].scaleMax);
        snprintf(por1_str, sizeof(por1_str), "%.2f", device[i].Porog1);
        snprintf(por2_str, sizeof(por2_str), "%.2f", device[i].Porog2);
        for(char* p = value_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = scale_max_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = por1_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = por2_str; *p; p++) if(*p == '.') *p = ',';
        
        // ќтправл€ем все параметры устройства
        SendNextionCommand("page%d.ch%d.txt=\" анал %d\"€€€", page, pos, i);         //pos вместо i-4*page
        SendNextionCommand("page%d.val%d.txt=\"%s\"€€€", page, pos, value_str);
        SendNextionCommand("page%d.gas%d.txt=\"%s\"€€€", page, pos, device[i].gas);
        SendNextionCommand("page%d.ran%d.txt=\"%s\"€€€", page, pos, scale_max_str);
        SendNextionCommand("page%d.unit%d.txt=\"%s\"€€€", page, pos, device[i].scaleDimension);
        SendNextionCommand("page%d.por1%d.txt=\"%s\"€€€", page, pos, por1_str);
        SendNextionCommand("page%d.por2%d.txt=\"%s\"€€€", page, pos, por2_str);
        SendNextionCommand("page%d.mod%d.txt=\"%s\"€€€", page, pos, device[i].model);
    }
 }

 uint8_t UpdateNextionDisplayWithChannelData(uint8_t channel_num){
	 
    static uint8_t step = 0;
    const uint8_t page = (channel_num - 1) / 4;  // 4 канала на страницу
    const uint8_t pos = channel_num - 4 * page;  // Ёлемент на странице (1..4)
	 
	  // ‘орматируем числа с зап€той вместо точки
		 char  scale_max_str[20], por1_str[20], por2_str[20];
	 
	  snprintf(scale_max_str, sizeof(scale_max_str), "%.2f", device[channel_num].scaleMax);
    snprintf(por1_str,  sizeof(por1_str),  "%.2f", device[channel_num].Porog1);
    snprintf(por2_str,  sizeof(por2_str),  "%.2f", device[channel_num].Porog2);
	  for(char* p = scale_max_str; *p; p++) if(*p == '.') *p = ',';
    for(char* p = por1_str; *p; p++) if(*p == '.') *p = ',';
    for(char* p = por2_str; *p; p++) if(*p == '.') *p = ',';
	 
	  if(step == 0)
		{
			step = 1;
		}
		else if(step == 1)
		{
			step = 2;
		}
		else if(step == 2)
		{
		  SendNextionCommand("page%d.gas%d.txt=\"%s\"€€€", page, pos, device[channel_num].gas);
			step = 3;
		}
		else if(step == 3)
		{
	    SendNextionCommand("page%d.ran%d.txt=\"%s\"€€€", page, pos, scale_max_str);
			step = 4;
		}
		else if(step == 4)
		{
		  SendNextionCommand("page%d.poz%d.txt=\"%s\"€€€", page, pos,device[channel_num].posit);
			step = 5;
		}
		else if(step == 5)
		{
	    SendNextionCommand("page%d.por1%d.txt=\"%s\"€€€", page, pos, por1_str);
	  	step = 6;
		}
		else if(step == 6)
		{ 
			SendNextionCommand("page%d.por2%d.txt=\"%s\"€€€", page, pos, por2_str);
			step = 7;
		}
		else if(step == 7)
		{

			step = 8;
		}
		else if(step == 8)
		{
		  SendNextionCommand("page%d.unit%d.txt=\"%s\"€€€", page, pos, device[channel_num].scaleDimension);
			step = 0;
			return 0;
		}
	  
	return   channel_num;
        				
 }
 /************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



