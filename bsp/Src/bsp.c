/**
******************************************************************************
* @file      bsp.c
* @author    OnWert
* @version   
* @brief     This file provides common functions for the   bsp    
*/       
/* ------------------------Includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "bsp.h"
#include "numberDevices.h"
#include "FlashDriver.h"
#include "UARTSlaveSettings.h"
#include "HoldingRegisterSlaveHandler.h"
#include "user_mb_app.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "RingBuffer.h"
#include "SensorLogs.h"
#include <stdlib.h>
#include <string.h>
/* ------------------------External variables -------------------------*/
extern uint16_t holdingRegsPart1[MAX_MODBUS_SLAVE_REGS_PART];  // Адреса 1-120
extern UART_HandleTypeDef huart1;
extern uint8_t ModBusSlaveDefaultDeviceAddr;
extern uint8_t is_active_rx_uart_buffer; 
extern SensorLogEvent_t sensorLog; 
extern osMessageQId queueSendLogsHandle;
extern RTC_HandleTypeDef hrtc;
/* ------------------------Global variables----------------------------*/
uint16_t calibrationProcesStatus = 0x00;

RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};

uint8_t NumberSlaveDevices       = 0x00;
SensorState_t  SensorStateArray[NUMBER_SLAVE_DEVICES] = {0};
SensorInfo_t   SensorInfo = {0};

volatile ModBusAddr_t  ModBusAddr = 
	    {
          .ModBuAddrSetFlag  =  0x00000000, 
	        .ModBusAddr        =  0x00000000,
				  .Reserved1         =  0x00000000,
				  .Reserved2         =  0x00000000
      };
volatile ModBusAddr_t  ModBusAddrDefault =  // default modbus addr
      {
          .ModBuAddrSetFlag  =  0x00000001, 
	        .ModBusAddr        =  0x00000001,
				  .Reserved1         =  0x00000000,
				  .Reserved2         =  0x00000000
      };	
			
volatile 	TimeStepReadingSensores_t TimeStepDefault =	
      {
			    .SetFlag  =  0x00000001, 
			    .Timestep =  TIME_STEP_DEFAULT_150_MS			
			};
			
volatile 	TimeStepReadingSensores_t TimeStep =	
      {
			    .SetFlag  =  0x00000000, 
			    .Timestep =  0x00000000		
			};		
/* ------------------------Locale variables----------------------------*/
 union ShortsToFloat converter;
			
 SensorCurrentState_t	writeParams = {0};	 
 SensorCurrentState_t	readParams  = {0};	

 bool checkParamsValue = false;
 extern uint8_t ControlCycleFlag; 
 
 
/* ------------------------Functions-----------------------------------*/
void initSensorStateArray(uint8_t numberdevices)
  {
		for(int i = 0; i < numberdevices; i++)
		{
			SensorStateArray[i].SensorModBudAddr     = 0x00;	
			  // Обнуление массива DeviceModelCode,строковое значение вещества 
     memset((void *)SensorStateArray[i].SensorSubstanceCode, 0, sizeof(SensorStateArray[i].SensorSubstanceCode));
		 memset((void *)SensorStateArray[i].DeviceModelCode, 0, sizeof(SensorStateArray[i].DeviceModelCode)); 
		 SensorStateArray[i].SensorScaleMax       = 0;
		 memset((void *)SensorStateArray[i].SensorGas, 0, sizeof(SensorStateArray[i].SensorGas));
		 memset((void *)SensorStateArray[i].SensorScaleDimension, 0, sizeof(SensorStateArray[i].SensorScaleDimension));
		 SensorStateArray[i].SensorWarning        = 0.00;
     SensorStateArray[i].SensorAlarm          = 0.00;
		 SensorStateArray[i].SensorAlarm2         = 0.00;
		 SensorStateArray[i].DeviceStatus         = 0;
		 SensorStateArray[i].Concentration_H      = 0;
		 SensorStateArray[i].Concentration_L      = 0;
		 SensorStateArray[i].Concentration        = 0.00;
		 SensorStateArray[i].NotResponsCounter    = 0;
		 SensorStateArray[i].ErrorState           = true;
		 SensorStateArray[i].CalibrationStatus    = 0x00;	
		}
  }
/**
 * @brief Reads and processes current sensor state from Modbus input registers
 * @param slaveaddr Slave device address (1-based index)
 * @param RegInputBuff 2D array containing Modbus input registers for all slaves
 */
void readCurrentSensorState(uint8_t slaveaddr, uint16_t RegInputBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS],uint16_t RegHoldingBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS])
{
	  uint32_t combined;   // Объединённые 32 бита
	  float result;        // Результат
	
    // Validate slave address
    if (slaveaddr < 1 || slaveaddr > MB_MASTER_TOTAL_SLAVE_NUM) {
        return; // or handle error appropriately
    }
    const size_t slave_idx = slaveaddr - 1;
    SensorState_t* sensor = &SensorStateArray[slave_idx];
    
    // Reset buffer values before reading
    sensor->DeviceStatus =  0;
    /* *********************************  Read sensor data ********************************** */
		
			  //CalibrationProcesStatus
		 //calibrationProcesStatus = (uint16_t)RegHoldingBuff[slave_idx][CALIBRATION_PROCESS_STATUS_INTERN];
      
     //sensor->CalibrationStatus = calibrationProcesStatus; 
		
		
			// SensorModelCode
    const char* unit = getDeviceModelNameFromShorts(RegHoldingBuff[slave_idx][DEVICE_MODEL_CODE_INTERN],RegHoldingBuff[slave_idx][DEVICE_MODEL_CODE_INTERN_2]);
    snprintf((char*)sensor->DeviceModelCode, sizeof(sensor->DeviceModelCode), "%s", unit);
		
		//RegHoldingBuff[slave_idx][DEVICE_MODEL_CODE_INTERN] = 0x0000;
		//RegHoldingBuff[slave_idx][DEVICE_MODEL_CODE_INTERN_2] = 0x0000;
		// SensorGas
	  uint16_t *src_ptr = &RegHoldingBuff[slave_idx][SENSOR_SUBSTANCE_CODE_1_INTERN];
     for (int i = 0; i < 6; i++) {
       sensor->SensorGas[i] = src_ptr[i] & 0xFF;  // Берём только младший байт
     }
		// SensorWarning
    combined = ((uint32_t)(uint16_t)RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_WARNIGN_HIGN_INTERN ] ) << 16 | (uint16_t)RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_WARNIGN_LOW_INTERN ];
     
		RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_WARNIGN_HIGN_INTERN ] = 0x0000;
    RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_WARNIGN_LOW_INTERN ] = 0x0000;

		 // Копируем биты в float (аналог reinterpret_cast в C++)
    *(uint32_t*)&result = combined;
    sensor->SensorWarning = result;
		 
		 if(ControlCycleFlag){
	  	readParams.SensorWarning = (uint32_t)result;     // for check
		 } 
		// SensorAlarm
    combined = ((uint32_t)(uint16_t)RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_ALARM_HIGH_INTERN] ) << 16 | (uint16_t)RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_ALARM_LOW_INTERN];
    
		 RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_ALARM_HIGH_INTERN] = 0x0000;
		 RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_ALARM_LOW_INTERN]  = 0x0000;
		 
    *(uint32_t*)&result = combined;
    sensor->SensorAlarm = result;
		 
		 if(ControlCycleFlag){
		 readParams.SensorAlarm = (uint32_t)result ;       // for check
		 }
		// SensorAlarm2
    combined = ((uint32_t)(uint16_t)RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_ADDITIONAL_HIGH_INTERN ] ) << 16 | (uint16_t)RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_ADDITIONAL_LOW_INTERN];
   
		RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_ADDITIONAL_HIGH_INTERN ]  = 0x0000;
		RegHoldingBuff[slave_idx][SENSOR_THRESHOLD_ADDITIONAL_LOW_INTERN ]   = 0x0000;
		 
    *(uint32_t*)&result = combined;
    sensor->SensorAlarm2 = result;
		 if(ControlCycleFlag){
		 readParams.SensorAlarm2 = (uint32_t)result;       // for check
		 }
		// SensorScaleMax
    combined = ((uint32_t)(uint16_t)RegHoldingBuff[slave_idx][SENSOR_SCALE_MAX_HIGH_INTERN] ) << 16 | (uint16_t)RegHoldingBuff[slave_idx][SENSOR_SCALE_MAX_LOW_INTERN ];
     
		 // for set error 
		 RegHoldingBuff[slave_idx][SENSOR_SCALE_MAX_HIGH_INTERN] = 0x0000;
		 RegHoldingBuff[slave_idx][SENSOR_SCALE_MAX_LOW_INTERN] = 0x0000;
		 
    *(uint32_t*)&result = combined;
    sensor->SensorScaleMax = result;
		 
		 if(ControlCycleFlag){
		 readParams.SensorScaleMax =  (uint32_t)result;  // for check
		 }
		// SensorScaleDimension
    unit = getUnitStringByCode(RegHoldingBuff[slave_idx][SENSOR_SCALE_DIMENSTION_INTERN]);
		 
    RegHoldingBuff[slave_idx][SENSOR_SCALE_DIMENSTION_INTERN] = 0x0000;
		 
    snprintf((char*)sensor->SensorScaleDimension, sizeof(sensor->SensorScaleDimension), "%s",(const char *)unit);
		 
		 if(ControlCycleFlag){
		  readParams.SensorScaleDimensionID = RegHoldingBuff[slave_idx][SENSOR_SCALE_DIMENSTION_INTERN - 1] ; // for check
		 }
		//Concentration 
    sensor->DeviceStatus    = RegInputBuff[slave_idx][SENSOR_PRIMARY_STATUS_INTERN];
    sensor->Concentration_H = RegInputBuff[slave_idx][SENSOR_PRIMARY_VALUE_HIGH_INTERN];
    sensor->Concentration_L = RegInputBuff[slave_idx][SENSOR_PRIMARY_VALUE_LOW_INTERN];
		  // Собираем 32 бита из двух 16-битных short
    combined = ((uint32_t)(uint16_t)sensor->Concentration_H ) << 16 | (uint16_t)  sensor->Concentration_L;

    // Копируем биты в float (аналог reinterpret_cast в C++)
    *(uint32_t*)&result = combined;
    sensor->Concentration   = result;
		
    // Clear the input buffer
    RegInputBuff[slave_idx][SENSOR_PRIMARY_STATUS_INTERN]  = 0x0000;
    RegInputBuff[slave_idx][SENSOR_PRIMARY_VALUE_HIGH_INTERN] = 0x0000;
    RegInputBuff[slave_idx][SENSOR_PRIMARY_VALUE_LOW_INTERN]  = 0x0000;
    
    // Process device status
    if (sensor->DeviceStatus > 0) {
        // Device responded successfully
        sensor->NotResponsCounter = 0;
        sensor->ErrorState = false;
    } else {
        // Device didn't respond
        sensor->NotResponsCounter++;
        
        // Mark as error after 3 consecutive failures
        if (sensor->NotResponsCounter >= 3) {
            sensor->ErrorState = true;
            // Consider additional error handling here if needed
        }
    }
}

/**
 * @brief Reads and processes current sensor state from Modbus input registers
 * @param slaveaddr Slave device address (1-based index)
 * @param RegInputBuff 2D array containing Modbus input registers for all slaves
 */
void readCurrentSensorValue(uint8_t slaveaddr, uint16_t RegInputBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS])
{
	  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	  uint32_t combined;   // Объединённые 32 бита
	  float result;        // Результат
    // Validate slave address
    if (slaveaddr < 1 || slaveaddr > MB_MASTER_TOTAL_SLAVE_NUM) {
        return; // or handle error appropriately
    }
    const size_t slave_idx = slaveaddr - 1;
    SensorState_t* sensor = &SensorStateArray[slave_idx];
    
		//Concentration 
    sensor->DeviceStatus    = RegInputBuff[slave_idx][SENSOR_PRIMARY_STATUS_INTERN];
    sensor->Concentration_H = RegInputBuff[slave_idx][SENSOR_PRIMARY_VALUE_HIGH_INTERN];
    sensor->Concentration_L = RegInputBuff[slave_idx][SENSOR_PRIMARY_VALUE_LOW_INTERN];
		  // Собираем 32 бита из двух 16-битных short
    combined = ((uint32_t)(uint16_t)sensor->Concentration_H ) << 16 | (uint16_t)  sensor->Concentration_L;

    // Копируем биты в float (аналог reinterpret_cast в C++)
    *(uint32_t*)&result = combined;
    sensor->Concentration   = result;
		
    // Clear the input buffer
    RegInputBuff[slave_idx][SENSOR_PRIMARY_STATUS_INTERN]  = 0x0000;
    RegInputBuff[slave_idx][SENSOR_PRIMARY_VALUE_HIGH_INTERN] = 0x0000;
    RegInputBuff[slave_idx][SENSOR_PRIMARY_VALUE_LOW_INTERN]  = 0x0000;
		
		 if(sensor->Concentration > sensor->SensorAlarm2)
		 {
			 sensorLog.sensorID = findSensoriD(SensorInfo.modbusAddrs,SensorInfo.count, slave_idx + 1);
		   sensorLog.Value =   sensor->Concentration; 
       sensorLog.logType = 	OVER_THRESHOLD_ADDITIONAL;
			 
			if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
                        }
      if (xHigherPriorityTaskWoken == pdTRUE) {
                          portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
              } 
		 }
		 else if(sensor->Concentration >   sensor->SensorAlarm) 
		 {
			sensorLog.sensorID = findSensoriD(SensorInfo.modbusAddrs,SensorInfo.count, slave_idx + 1);
		  sensorLog.Value =   sensor->Concentration; 
      sensorLog.logType = OVER_THRESHOLD_ALARM;	
			 
      if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
                        }
           if (xHigherPriorityTaskWoken == pdTRUE) {
                          portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
              }			 
		 }
		 else if(sensor->Concentration >   sensor->SensorWarning) 
		 {
			 sensorLog.sensorID = findSensoriD(SensorInfo.modbusAddrs,SensorInfo.count, slave_idx + 1);
		   sensorLog.Value =   sensor->Concentration;
       sensorLog.logType = OVER_THRESHOLD_WARNING;
			 
			if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
                        }
      if (xHigherPriorityTaskWoken == pdTRUE) {
                          portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
             }
		  }
}

/**
 * @brief Get the number of connected slave devices
 * @return uint8_t - Current number of slave devices
 */
uint8_t getNumberDevices(void)
{
   return NumberSlaveDevices;
}

/**
 * @brief Set the number of slave devices in the system
 * @param numberDevices Pointer to store the number of devices
 * @param number The new number of devices to set
 */
void setNumberDevices(uint8_t *numberDevices, uint8_t number)
{
  *numberDevices = number;
}

/**
 * @brief Set the address of the next device to communicate with
 * @param currentAddr Pointer to the current device address
 * @note This function performs the following steps:
 *       1. Increments the address (with wrap-around if needed)
 *       2. Temporarily disables Modbus communication
 *       3. Sets the new destination address
 *       4. Re-enables Modbus communication
 *       5. Includes small delays for hardware stabilization
 */
void setNextDeviceAddr(uint8_t *currentAddr)
{ 
    // Increment address with wrap-around if needed
    if(*currentAddr < NumberSlaveDevices)
    {
        (*currentAddr)++;
    }
    else 
    {
        // Wrap around to first device if we've reached the end
        *currentAddr = 1;
    }
    
    // Temporarily disable Modbus communication
    eMBMasterDisable();
    osDelay(5);  // Short delay for hardware stabilization
    
    // Set new destination address
    vMBMasterSetDestAddress(*currentAddr);
    osDelay(5);  // Short delay for hardware stabilization
    
    // Re-enable Modbus communication
    eMBMasterEnable();
    osDelay(5);  // Short delay for hardware stabilization
}
 
	void setModBusSlaveSetting(void)
	{
		/* 1. reading setting for UART from flash */
		Flash_Read_Data(FLASH_SETTING_UART, (uint32_t *)&UartSlaveSetting,5);
		
		/* 2. set uart setting */	
		if(UartSlaveSetting.UartSetFlag == 1)    /* checking available uart setting flag */
		{
			 huart1.Instance = USART1;
			 huart1.Init.BaudRate = getBaudrate(UartSlaveSetting.BaudRateID);
			 huart1.Init.StopBits = getStopBits(UartSlaveSetting.StopBitsID);
			 huart1.Init.Parity = getParity(UartSlaveSetting.ParityID) ;
			 huart1.Init.Mode = UART_MODE_TX_RX;
			 huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			 huart1.Init.OverSampling = UART_OVERSAMPLING_16;
			 	
		   /* FOR PARITY */
			 if(UartSlaveSetting.ParityID == 1 || UartSlaveSetting.ParityID == 2)
			 {
			  huart1.Init.WordLength = getUartWordLength(1); //9 bits working with   8 bit host (even ,odd)
			 }
			 
			 else if (UartSlaveSetting.ParityID == 0)
			 {
			   huart1.Init.WordLength = getUartWordLength(0); //8 bits
			 }
			 
			
			 if (HAL_UART_Init(&huart1) != HAL_OK)
			 {
				 Error_Handler();
			 }
			 
			 HAL_Delay(5);
		}
		
		else 
		{
	 	 Flash_Write_Data(FLASH_SETTING_UART,(uint32_t *)&UartSettingDefault, 5);
		
			HAL_Delay(5);
			
			UartSlaveSetting.UartSetFlag = UartSettingDefault.UartSetFlag;
			UartSlaveSetting.BaudRateID = UartSettingDefault.BaudRateID;
			UartSlaveSetting.WordLengthID = UartSettingDefault.WordLengthID;
			UartSlaveSetting.StopBitsID = UartSettingDefault.StopBitsID;
			UartSlaveSetting.ParityID = UartSettingDefault.ParityID;
		
			
		   huart1.Instance = USART1;
			 huart1.Init.BaudRate = getBaudrate(UartSettingDefault.BaudRateID) ;
			 huart1.Init.StopBits = getStopBits(UartSettingDefault.StopBitsID);
			 huart1.Init.Parity = getParity(UartSettingDefault.ParityID) ;
			 huart1.Init.Mode = UART_MODE_TX_RX;
			 huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			 huart1.Init.OverSampling = UART_OVERSAMPLING_16;
			
				           /* FOR PARITY */
			 if(UartSettingDefault.ParityID == 1 || UartSettingDefault.ParityID == 2)
			 {
			  huart1.Init.WordLength = getUartWordLength(1); //9 bits working with   8 bit host (even ,odd)
			 }
			 
			 else if (UartSettingDefault.ParityID == 0)
			 {
			   huart1.Init.WordLength = getUartWordLength(0); //8 bits
			 }
			 
			 if (HAL_UART_Init(&huart1) != HAL_OK)
			 {
				 Error_Handler();
			 }
			 
			 HAL_Delay(10);		 	 
		}
		
		MB_BaudRateValue =  huart1.Init.BaudRate;
		MB_ParityValue   =  huart1.Init.Parity;
		MB_StopBitsValue =  huart1.Init.StopBits;	
		
		holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_1] = UartSlaveSetting.BaudRateID ;
		holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_2] = UartSlaveSetting.ParityID ;
		holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_3] = UartSlaveSetting.StopBitsID;
					
	}
	
	void setModBusSlaveAddr(void)
	{
		
     Flash_Read_Data(FLASH_SLAVE_MODBUS_ID, (uint32_t *)&ModBusAddr,4);
		
		if(ModBusAddr.ModBuAddrSetFlag == 1)
		  {
				MB_AddresseValue = ModBusAddr.ModBusAddr;	   
			}	
		else
			{	
			  Flash_Write_Data(FLASH_SLAVE_MODBUS_ID, (uint32_t *)&ModBusAddrDefault, 4);
				
				HAL_Delay(2);
				
				ModBusAddr.ModBuAddrSetFlag = ModBusAddrDefault.ModBuAddrSetFlag;
				ModBusAddr.ModBusAddr = ModBusAddrDefault.ModBusAddr;
			  ModBusAddr.Reserved1  = 0x00;
				ModBusAddr.Reserved2  = 0x00;
				
		   	MB_AddresseValue = ModBusAddr.ModBusAddr;
			}
     			
		  holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_4] = MB_AddresseValue;
			
	    HAL_Delay(5);		
	}	
	
	void setTimeStepReadingSensores(void)	
	 {
	    Flash_Read_Data(FLASH_TIME_STEP_READING, (uint32_t *)&TimeStep,2);
		 
	   if(TimeStep.SetFlag == 1)
		 {
		   timeStep = TimeStep.Timestep;
		 }
	   else
		 {	 
			  Flash_Write_Data(FLASH_TIME_STEP_READING, (uint32_t *)&TimeStepDefault, 2);
				
				HAL_Delay(2);
			 
			  TimeStep.SetFlag	= TimeStepDefault.SetFlag;
			  TimeStep.Timestep = TimeStepDefault.Timestep;
			 
			  timeStep = TimeStep.Timestep;
		 } 
		 
		 holdingRegsPart1[HOLDING_REGISTER_SLAVE_IDX_5] = timeStep;
			
	   HAL_Delay(5);			 
	 }
	 
void setNextActiveDeviceAddr(uint8_t *currentAddr) 
{     
    static uint8_t slave_idx = 0;
    
    // Iterate through all possible devices (1, 2, 3)
    for (int attempt = 0; attempt < NUMBER_SLAVE_DEVICES; attempt++) 
    {
        // Move to next device (wrap around if needed)
        slave_idx = (slave_idx % NUMBER_SLAVE_DEVICES) + 1;
        
        SensorState_t* sensor = &SensorStateArray[slave_idx - 1];
        
        // If device is error-free, use it
        if (sensor->ErrorState == false) 
        {
            *currentAddr = slave_idx;
            return;
        }
    }  
    // If all devices have errors, return 0 or some error code
    *currentAddr = 1;
}

void setNextActiveDeviceAddr_(uint8_t *currentAddr, uint8_t countsensores) 
{    
    static uint8_t currentIdx	= 0x00;
	
    if(countsensores == 0x00) // если датчиков нет вылетаем 
		{
			// If all devices have errors, return 0 or some error code
      *currentAddr = 1;
		  return;
		}      
		
		//берем следующий modbus адрес из списка 
     *currentAddr = SensorInfo.modbusAddrs[currentIdx];
     currentIdx++;
		// если весь список прошли начинаем сначала
    if(currentIdx ==  countsensores)	
 		{
		 currentIdx = 0;
		}			
}
 
 uint8_t GetActiveSensors(SensorState_t SensorStateArray[NUMBER_SLAVE_DEVICES], SensorInfo_t *sensorinfo)
  {	   
		uint8_t IdxActiveAddr = 0x00;
	 // Iterate through all possible devices (1, 2, 3)
    for (int i = 0; i < NUMBER_SLAVE_DEVICES - 1; i++) 
    {
			 SensorState_t *sensor = &SensorStateArray[i];	 
			if (sensor->ErrorState == false) 
        {   
					sensor->SensorModBudAddr = i + 1; // modbus адрес активного датчика 		
					/* формирование массива адресов активных датчиков и количества датчиков*/
					sensorinfo->count++;	
					sensorinfo->modbusAddrs[IdxActiveAddr] = sensor->SensorModBudAddr; 
					IdxActiveAddr++;
				  osDelay(1);	
        }
	  } 	
		return sensorinfo->count;
	}
	 
	const char* getUnitStringByCode(uint8_t code) {
    switch (code) {
        case 0x8B: return "ppm";
        case 0xAA: return "mg/m3";
        case 0xA1: return "%НКПР";
        case 0x6A: return "% Об.д";
       
        default:   return "unknown";  // Если код не найден
    }
 }
 uint8_t getCodeByUnitString(const char* unitStr) {
    if (strcmp(unitStr, "ppm") == 0) return 0x8B;
    if (strcmp(unitStr, "mg/m3") == 0) return 0xAA;
    if (strcmp(unitStr, "%ЅєїА") == 0) return 0xA1;
    if (strcmp(unitStr, "% ѕС.Ф.") == 0) return 0x6A;
    return 0xFF; // Код для неизвестной единицы измерения
 }	

/**
 * @brief Получает название модели устройства по двум short (4 байта ASCII).
 * @param part1 Первые 2 ASCII-символа (например, 0x4531 для "E1").
 * @param part2 Следующие 2 ASCII-символа (например, 0x3938 для "98").
 * @return Название модели или "Неизвестная модель".
 */
const char* getDeviceModelNameFromShorts(short part1, short part2) {
    // Преобразуем два short в массив из 4 байт (с учётом little-endian)
    uint8_t device_code[4];
	
	  device_code[0] = (uint8_t)((part1 >> 8) & 0xFF); // Старший байт part1
    device_code[1] = (uint8_t)(part1 & 0xFF);       // Младший байт part1
	  device_code[2] = (uint8_t)((part2 >> 8) & 0xFF); // Старший байт part2
    device_code[3] = (uint8_t)(part2 & 0xFF);       // Младший байт part2
	
    // Известные коды моделей (4 ASCII-символа)
    static const struct {
        uint8_t code[4];
        const char* name;
    } device_mappings[] = {
        {{0x45, 0x31, 0x39, 0x38}, "Бинар-6X"},  // E198
        {{0x45, 0x31, 0x39, 0x37}, "Бинар-7X"},  // E197
        {{0x45, 0x31, 0x39, 0x44}, "Бинар-2Д"},  // E19D
    };

    // Сравниваем с известными моделями
    for (size_t i = 0; i < sizeof(device_mappings) / sizeof(device_mappings[0]); i++) {
        if (memcmp(device_code, device_mappings[i].code, 4) == 0) {
            return device_mappings[i].name;
        }
    }
    return "Неизвестная модель";
}
	
/**    
* @brief   Receive a byte of data from circular Rx buffer
* @param   buf: pointer of RING_buffer_t
* @param   a: symbol to receive
* @retval  RX_Buffer_State                                          
*/ 
RX_Buffer_State Uart_Get_Byte(RING_buffer_t* buf, uint8_t* a)
{  
  
  uint8_t i = 3;                                  /* numbers of tries */
  
  if(is_active_rx_uart_buffer == 1)                /* to prevent a simultaneuous manipulation with the circular RX buffer */
    while ((i) && (is_active_rx_uart_buffer))
    { i--;  }
  
  if(is_active_rx_uart_buffer == 1)   /* if  rx buffer not yet available, then return error */
  { 
    return RX_BUF_FAIL;
  }
  
  is_active_rx_uart_buffer = 1;       /* mutex  */
  
  if(buf->idxIn!=buf->idxOut)        /* if  rx buffer is not  empty */
  {
    *a = RING_Pop(buf);
    
    is_active_rx_uart_buffer = 0;    /* mutex  release */
    
    return RX_BUF_DONE;
  }
  else                              /* rx buffer is empty */
  {
    return RX_BUF_EMPTY;
  }
} 


bool compareParams(SensorCurrentState_t *writeParams, SensorCurrentState_t *readParams )
  {
  
    if (writeParams->SensorWarning == readParams->SensorWarning) return false;
    if (writeParams->SensorAlarm == readParams->SensorAlarm) return false;
		if (writeParams->SensorAlarm2 == readParams->SensorAlarm2) return false;
    if (writeParams->SensorScaleMax == readParams->SensorScaleMax) return false;
		if (writeParams->SensorScaleDimensionID == readParams->SensorScaleDimensionID) return false;
		
		
		 return true;
  }
	
	
void readCurrentCalibrationState(uint8_t slaveaddr,uint16_t RegHoldingBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS])
{
	
    // Validate slave address
    if (slaveaddr < 1 || slaveaddr > MB_MASTER_TOTAL_SLAVE_NUM) {
        return; // or handle error appropriately
    }
    const size_t slave_idx = slaveaddr - 1;
    SensorState_t* sensor = &SensorStateArray[slave_idx];
    
    // Reset buffer values before reading
    sensor->CalibrationStatus =  0;
    /* *********************************  Read sensor data ********************************** */
	
		 sensor->CalibrationStatus  = (uint16_t)RegHoldingBuff[slave_idx][CALIBRATION_PROCESS_STATUS_INTERN];  // так работает 
		
		 RegHoldingBuff[slave_idx][CALIBRATION_PROCESS_STATUS_INTERN] = 0x0000;
      
	}	
	
bool getCalibrationProcessState(uint8_t slaveaddr)
 {
    const size_t slave_idx = slaveaddr - 1;
    SensorState_t* sensor = &SensorStateArray[slave_idx];
    
    if( sensor->CalibrationStatus ==  CALIBRATION_STATUS_SUCCESFUL_COMPLETED)
		{
		  return true;
		}
		{
		 return false;
		}
 }	
 
 // Функция для установки только года
HAL_StatusTypeDef Set_RTC_Year(uint8_t year)
{
    RTC_DateTypeDef sDate;
    
    // Сначала читаем текущую дату
    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Меняем только год
    sDate.Year = year;  // 0-99, где 0 = 2000 год
    
    // Устанавливаем обновленную дату
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    return HAL_OK;
}
 
uint16_t Get_RTC_Year(void)
 {
    RTC_DateTypeDef sDate;
    
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    
    return sDate.Year;  // Возвращает год (0-99)
 }
 
// Функция для установки только месяца
HAL_StatusTypeDef Set_RTC_Month(uint8_t month)
{
    RTC_DateTypeDef sDate;
    
    // Проверка корректности месяца
    if (month < 1 || month > 12) {
        return HAL_ERROR;
    }
    
    // Сначала читаем текущую дату
    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Меняем только месяц
    sDate.Month = month;
    
    // Устанавливаем обновленную дату
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    return HAL_OK;
} 
 
// Функция для чтения текущего месяца
uint8_t Get_RTC_Month(void)
{
    RTC_DateTypeDef sDate;
    
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    
    return sDate.Month;  // Возвращает месяц (1-12)
}
 
// Функция для установки только дня
HAL_StatusTypeDef Set_RTC_Day(uint8_t day)
{
    RTC_DateTypeDef sDate;
    
    // Проверка корректности дня
    if (day < 1 || day > 31) {
        return HAL_ERROR;
    }
    
    // Сначала читаем текущую дату
    if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Дополнительная проверка в зависимости от месяца
    uint8_t current_month = sDate.Month;
    
    // Проверка для месяцев с 30 днями
    if ((current_month == 4 || current_month == 6 || current_month == 9 || current_month == 11) && day > 30) {
        return HAL_ERROR;
    }
    
    // Проверка для февраля (упрощенная, без учета високосных лет)
    if (current_month == 2 && day > 28) {
        return HAL_ERROR;
    }
    
    // Меняем только день
    sDate.Date = day;
    
    // Устанавливаем обновленную дату
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    return HAL_OK;
}

// Функция для чтения текущего дня
uint8_t Get_RTC_Day(void)
{
    RTC_DateTypeDef sDate;
    
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    
    return sDate.Date;  // Возвращает день (1-31)
}

// Функция для установки только часа
HAL_StatusTypeDef Set_RTC_Hour(uint8_t hour)
{
    RTC_TimeTypeDef sTime;
    
    // Проверка корректности часа (24-часовой формат)
    if (hour > 23) {
        return HAL_ERROR;
    }
    
    // Сначала читаем текущее время
    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Меняем только час
    sTime.Hours = hour;
    
    // Устанавливаем обновленное время
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    return HAL_OK;
}
// Функция для чтения текущего часа
uint8_t Get_RTC_Hour(void)
{
    RTC_TimeTypeDef sTime;
    
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    
    return sTime.Hours;  // Возвращает час (0-23)
}

// Функция для установки только минут
HAL_StatusTypeDef Set_RTC_Minute(uint8_t minute)
{
    RTC_TimeTypeDef sTime;
    
    // Проверка корректности минут
    if (minute > 59) {
        return HAL_ERROR;
    }
    
    // Сначала читаем текущее время
    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Меняем только минуты
    sTime.Minutes = minute;
    
    // Устанавливаем обновленное время
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    return HAL_OK;
}

// Функция для чтения текущих минут
uint8_t Get_RTC_Minute(void)
{
    RTC_TimeTypeDef sTime;
    
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    
    return sTime.Minutes;  // Возвращает минуты (0-59)
}
// Функция для установки только секунд
HAL_StatusTypeDef Set_RTC_Second(uint8_t second)
{
    RTC_TimeTypeDef sTime;
    
    // Проверка корректности секунд
    if (second > 59) {
        return HAL_ERROR;
    }
    
    // Сначала читаем текущее время
    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    // Меняем только секунды
    sTime.Seconds = second;
    
    // Устанавливаем обновленное время
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        return HAL_ERROR;
    }
    
    return HAL_OK;
}

// Функция для чтения текущих секунд
uint8_t Get_RTC_Second(void)
{
    RTC_TimeTypeDef sTime;
    
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    
    return sTime.Seconds;  // Возвращает секунды (0-59)
}

/**
  * @brief  Устанавливает время и дату в RTC из HEX-строки формата "HH:MM/DD.MM.YYYY"
  * @param  hex_str: Указатель на строку с HEX-данными (разделители - пробелы)
  * @retval HAL status: HAL_OK при успехе, HAL_ERROR при ошибке
  */
 char ascii_buffer[17] = {0}; // Буфер для ASCII строки (16 символов + нуль-терминатор)
 
/**
  * @brief  Полная функция установки времени в RTC из HEX-строки
  */
HAL_StatusTypeDef RTC_SetFromHexString(char* hex_str, uint8_t size)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    // 1. Преобразование HEX-строки в ASCII
		//char hex_str_[] = "32 31 3A 35 30 2F 30 34 2E 31 30 2E 32 30 32 37";
		
		  char hex_str_[64]; // Буфер для результирующей строки
		  int pos = 0;
    
       for (int i = 0; i < size; i++) {
        // Преобразуем каждый байт в два HEX символа
        pos += sprintf(&hex_str_[pos], "%02X", hex_str[i]);
        osDelay(1);;
        // Добавляем пробел (кроме последнего элемента)
        if (i < size - 1) {
            hex_str_[pos++] = ' ';
        }
      }
     hex_str_[pos] = '\0'; // Завершаем строку
		
		 osDelay(1);
    if (hex_to_ascii_minimal(hex_str_, ascii_buffer) != HAL_OK) {
        return HAL_ERROR;
    }
		 osDelay(1);
    // 2. Парсинг ASCII строки формата "HH:MM/DD.MM.YYYY"
    int hours, minutes, day, month, year;
    if (sscanf(ascii_buffer, "%02d:%02d/%02d.%02d.%04d", 
               &hours, &minutes, &day, &month, &year) != 5) {
        return HAL_ERROR;
    }
     osDelay(1);
    // 3. Валидация полученных значений
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 ||
        day < 1 || day > 31 || month < 1 || month > 12 || year < 2000) {
        return HAL_ERROR;
    } 
    // 4. Настройка структуры времени
    sTime.Hours = hours;
    sTime.Minutes = minutes;
    sTime.Seconds = 0;
    sTime.TimeFormat = RTC_HOURFORMAT_24;
 
    // 5. Настройка структуры даты
    sDate.Month = month;
    sDate.Date = day;
    sDate.Year = year - 2000; // Преобразование года в формат RTC (0-99)
    
    // 6. Установка времени и даты в RTC
    HAL_StatusTypeDef status;
     osDelay(1);
    status = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    if (status != HAL_OK) return status;
     osDelay(1);
    status = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    if (status != HAL_OK) return status;
    
    return HAL_OK;
}

HAL_StatusTypeDef hex_to_ascii_minimal(char* hex_str, char* ascii_buf)
{
    // Просто игнорируем пробелы и преобразуем подряд
    int ascii_index = 0;
    int hex_index = 0;
    
    while (ascii_index < 16) {
        // Пропускаем пробелы
        while (hex_str[hex_index] == ' ') hex_index++;
        
        if (hex_str[hex_index] == '\0') return HAL_ERROR;
        
        // Берем два символа подряд
        char hex_byte[3] = {hex_str[hex_index], hex_str[hex_index + 1], '\0'};
        ascii_buf[ascii_index++] = (char)strtoul(hex_byte, NULL, 16);
        hex_index += 2;
    }
    
    ascii_buf[16] = '\0';
    return HAL_OK;
}


uint8_t findSensoriD(uint8_t arr[], int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) {
            return i + 1;
        }
    }
    return 0; // элемент не найден
}

/************************ (C) COPYRIGHT ONWERT *****END OF FILE****/