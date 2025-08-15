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
#include <stdarg.h>  // ��� va_start, va_arg, va_end
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "cmsis_os.h"
#include "numberDevices.h"
#include "stdbool.h"
#include "UARTSlaveSettings.h"
#include "HoldingRegisterSlaveHandler.h"
/* ------------------------External variables -------------------------*/
extern UART_HandleTypeDef huart3;
extern osThreadId SlaveEventTaskHandle;
/* ------------------------Global variables----------------------------*/
 char arrDisplayTX[ARRAY_TX_SIZE] = {0};
 volatile uint8_t arrDisplayRX[ARRAY_RX_SIZE] = {0};
 volatile uint8_t startDisplayFlag = 0;
 
 uint8_t rxIdxDisplayUart = 0;
extern  uint8_t tx_buffer[ARRAY_TX_SIZE + 3]; // �������� ����� + 3 ����� ������� �����
extern volatile uint16_t tx_index;
extern volatile uint16_t tx_size;
extern  uint8_t  numberOfDevices;

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
	float current4_20;									  	// ����� 4-20 mA
	float currentConsumptoin;								// ��� �����������
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
   SendNextionCommand ("Init.qDev.txt=\"%d\"���", flash_struct.dev_quan);
 }
 
 void UART_Display_StartReceiving(void){
    
	 // ���������� ���������� �� ����� ������� �����
       HAL_UART_Receive_IT(&huart3, (uint8_t *)&arrDisplayRX[0], 1); 
 }

 void FixRussianEncodingForNextion(char *str) {
    for (; *str != '\0'; str++) {  // �������� �� ����� ������
        if ((*str >= (char)0xC0) && (*str != (char)0xFF)) {
            *str -= 0x10;
        }
    }
}

// ��������������� ������� ��� �������� ������
 void SendNextionCommand(const char* format, ...){
    va_list args;
    va_start(args, format);
    uint8_t len = vsnprintf((char*)tx_buffer, ARRAY_TX_SIZE, format, args);
    va_end(args);
    
    if(len >= ARRAY_TX_SIZE) {
        return; // ��������� ������������ ������
    }
    
    FixRussianEncodingForNextion((char*)tx_buffer);
    
    // ��������� ������ ����� (3 ����� 0xFF)
    tx_buffer[len] = 0xFF;
    tx_buffer[len+1] = 0xFF;
    tx_buffer[len+2] = 0xFF;
    
    tx_size = len + 3;
    tx_index = 0;
    
    // �������� ���������� ��������
    huart3.Instance->CR1 |= USART_CR1_TXEIE;
    
    osDelay(40); // �������� ����� ���������
}

 void InitNextionDisplayWithDeviceData(uint8_t numberOfdevices){
 		
		  // ���������� ������ ��� ������� ����������
    for(uint8_t i = 1; i <= numberOfdevices ; i++) {
			
        uint8_t page = (i-1)/4;
        uint8_t pos =  (i - 1) % 4 + 1;    // ������� �� �������� (1-4)
        
        // ����������� ����� � ������� ������ �����
        char value_str[20], scale_max_str[20], por1_str[20], por2_str[20];
			
        snprintf(value_str, sizeof(value_str), "%.2f", device[i].value);
        snprintf(scale_max_str, sizeof(scale_max_str), "%.2f", device[i].scaleMax);
        snprintf(por1_str, sizeof(por1_str), "%.2f", device[i].Porog1);
        snprintf(por2_str, sizeof(por2_str), "%.2f", device[i].Porog2);
        for(char* p = value_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = scale_max_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = por1_str; *p; p++) if(*p == '.') *p = ',';
        for(char* p = por2_str; *p; p++) if(*p == '.') *p = ',';
        
        // ���������� ��� ��������� ����������
			  SendNextionCommand ("page page%d", page);
        SendNextionCommand("page%d.ch%d.txt=\"����� %d\"", page, pos, i);         //pos ������ i-4*page
        SendNextionCommand("page%d.val%d.txt=\"%s\"", page, pos, value_str);
        SendNextionCommand("page%d.gas%d.txt=\"%s\"", page, pos, device[i].gas);
        SendNextionCommand("page%d.ran%d.txt=\"%s\"", page, pos, scale_max_str);
        SendNextionCommand("page%d.unit%d.txt=\"%s\"", page, pos, device[i].scaleDimension);
        SendNextionCommand("page%d.por1%d.txt=\"%s\"", page, pos, por1_str);
        SendNextionCommand("page%d.por2%d.txt=\"%s\"", page, pos, por2_str);
        SendNextionCommand("page%d.mod%d.txt=\"%s\"", page, pos, device[i].model);
    }
 }

 uint8_t UpdateNextionDisplayWithChannelData(uint8_t channel_num){
	 
    static uint8_t step = 0;
    const uint8_t page = (channel_num - 1) / 4;  // 4 ������ �� ��������
    const uint8_t pos = channel_num - 4 * page;  // ������� �� �������� (1..4)
	 
	  // ����������� ����� � ������� ������ �����
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
		  SendNextionCommand("page%d.gas%d.txt=\"%s\"���", page, pos, device[channel_num].gas);
			step = 3;
		}
		else if(step == 3)
		{
	    SendNextionCommand("page%d.ran%d.txt=\"%s\"���", page, pos, scale_max_str);
			step = 4;
		}
		else if(step == 4)
		{
		  SendNextionCommand("page%d.poz%d.txt=\"%s\"���", page, pos,device[channel_num].posit);
			step = 5;
		}
		else if(step == 5)
		{
	    SendNextionCommand("page%d.por1%d.txt=\"%s\"���", page, pos, por1_str);
	  	step = 6;
		}
		else if(step == 6)
		{ 
			SendNextionCommand("page%d.por2%d.txt=\"%s\"���", page, pos, por2_str);
			step = 7;
		}
		else if(step == 7)
		{

			step = 8;
		}
		else if(step == 8)
		{
		  SendNextionCommand("page%d.unit%d.txt=\"%s\"���", page, pos, device[channel_num].scaleDimension);
			step = 0;
			return 0;
		}
	  
	return   channel_num;
        				
 }
 
void initDeviceData(uint8_t numberOfdevices)

   {
		 for(uint8_t i = 0; i <= numberOfdevices ; i++)
		  {
			  device[i].value = 11.56;
			  strncpy(device[i].gas, "O2", sizeof(device[i].gas));
				device[i].scaleMax = 50.00;
			  strncpy(device[i].scaleDimension, "ppm", sizeof(device[i].scaleDimension));
				device[i].Porog1 = 10;
				device[i].Porog2 = 20;
				strncpy(device[i].model, "QT1020", sizeof(device[i].model));
			}
			
	 }
	 
// ���������� ���� � ������� ������ �����
 uint8_t getIntFromChar( char *inputString, uint8_t stringSize)
 {
	 int number = 0;
	for (int i = 0; i < stringSize; i++) {
			
		  if (inputString[i] == 0x00) continue; // ���������� ����
			 osDelay(1);
			// ���� ���� � ��������� ASCII-���� ('0'-'9')
			if (inputString[i] >= 0x30 && inputString[i] <= 0x39) {
					// ������ �����: data[i] - '0', ������: data[i+1] - '0'
					number = (inputString[i] - 0x30) * 10 + (inputString[i+1] - 0x30);
					break; // ������� ����� ���������
			}
	 }
  return number;
}	 
	 

	/**
 * @brief ������������ �������, ���������� �� �������.
 * @param displayResponse ��� ������� �� �������.
 * @param arrDisplayRX ����� � ������� �������.
 * @param packet_ready ���� ���������� ������ (1 � ������ ��������, 0 � ���).
 * @param numberOfDevices ��������� �� ���������� � ������ ��������� (���� ����� ���������).
 * @param huart ��������� �� UART-��������� ��� ����������� �����.
 */
void HandleDisplayCommands(uint8_t displayResponse, uint8_t *arrDisplayRX, uint8_t *packet_ready, UART_HandleTypeDef *huart) {
    
	if (*packet_ready) {
        switch (displayResponse) {
            case 0x01: // ������
                break;
            case 0x02: // ������� ���������
                break;
            case 0x03: // �������� ���������
                break;
            case 0x04: // ����� 1
                break;
            case 0x05: // ����� 2
                break;
            case 0x06: // ���
                break;
            case 0x88: // ������ ����� ����� ������ ������� (0x88 0xFF 0xFF 0xFF)
                SendNextionCommand("Init.qDev.txt=\"%d\"", numberOfDevices); // �������� ������
                break;
            case 0x10: // ������ ����� ����� ������ ������� (0x10 0xFF 0xFF 0xFF)
                InitNextionDisplayWithDeviceData(numberOfDevices);
                break;
            case 0xA0: // ����� �������� UART
                if (arrDisplayRX[1] >= 1 && arrDisplayRX[1] <= 6) {
                    MB_BaudRateValue = getBaudrate(arrDisplayRX[1]);
                    xTaskNotify(SlaveEventTaskHandle, HOLDING_REGISTER_SLAVE_IDX_1, eSetValueWithOverwrite);
                }
                osDelay(1);
                break;
            case 0x33: // ���������� ����� ���������
                numberOfDevices = getIntFromChar((char *)&arrDisplayRX[0], 5);
                break;
        }

        // ����� ����� � ������
        *packet_ready = 0;
        memset(arrDisplayRX, 0, ARRAY_RX_SIZE);

        // ���������� UART-�����
        HAL_UART_Receive_IT(huart, arrDisplayRX, 1);
    }
} 
	  
 /************************ (C) COPYRIGHT  OnWert *****END OF FILE****/



