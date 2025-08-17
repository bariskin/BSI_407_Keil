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
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
/* ------------------------External variables -------------------------*/
extern uint16_t holdingRegsPart1[MAX_MODBUS_SLAVE_REGS_PART];  // Адреса 1-120
extern UART_HandleTypeDef huart1;
extern uint8_t ModBusSlaveDefaultDeviceAddr;
/* ------------------------Global variables----------------------------*/
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
			    .Timestep =  TIME_STEP_DEFAULT_2			
			};
			
volatile 	TimeStepReadingSensores_t TimeStep =	
      {
			    .SetFlag  =  0x00000000, 
			    .Timestep =  0x00000000		
			};		
/* ------------------------Locale variables----------------------------*/
 union ShortsToFloat converter;
/* ------------------------Functions-----------------------------------*/

void initSensorStateArray(uint8_t numberdevices)
  {
		for(int i = 0; i < numberdevices; i++)
		{
			  // Обнуление массива DeviceModelCode,строковое значение вещества 
    memset((void *)SensorStateArray[i].SensorSubstanceCode, 0, sizeof(SensorStateArray[i].SensorSubstanceCode));
			
		 SensorStateArray[i].SensorModBudAddr     = 0x00;	
		 SensorStateArray[i].DeviceModelCode      = 0x00000000;
		 SensorStateArray[i].SensorScaleMax       = 0x00000000;
		 SensorStateArray[i].SensorScaleDimension = 0x0000;
		 SensorStateArray[i].SensorWarning        = 0x00000000;
     SensorStateArray[i].SensorAlarm          = 0x00000000;
		 SensorStateArray[i].DeviceStatus         = 0x0000;
		 SensorStateArray[i].Concentration_H      = 0x0000;
		 SensorStateArray[i].Concentration_L      = 0x0000;
		 SensorStateArray[i].Concentration        = 0.00;
		 SensorStateArray[i].NotResponsCounter    = 0x0000;
		 SensorStateArray[i].ErrorState           = true;
		}
  }

/**
 * @brief Reads and processes current sensor state from Modbus input registers
 * @param slaveaddr Slave device address (1-based index)
 * @param RegInputBuff 2D array containing Modbus input registers for all slaves
 */
void readCurrentSensorState(uint8_t slaveaddr, uint16_t RegInputBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS])
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
    sensor->DeviceStatus = 0x0000;
    
    // Read sensor data
    sensor->DeviceStatus    = RegInputBuff[slave_idx][SENSOR_PRIMARY_STATUS_INTERN - 1];
    sensor->Concentration_H = RegInputBuff[slave_idx][SENSOR_PRIMARY_VAUE_HIGH_INTERN - 1];
    sensor->Concentration_L = RegInputBuff[slave_idx][SENSOR_PRIMARY_VAUE_LOW_INTERN - 1];
		  // Собираем 32 бита из двух 16-битных short
    combined = ((uint32_t)(uint16_t)sensor->Concentration_H ) << 16 | (uint16_t)  sensor->Concentration_L;

    // Копируем биты в float (аналог reinterpret_cast в C++)
    *(uint32_t*)&result = combined;
    sensor->Concentration   = result;
		
    // Clear the input buffer
    RegInputBuff[slave_idx][SENSOR_PRIMARY_STATUS_INTERN - 1]  = 0x0000;
    RegInputBuff[slave_idx][SENSOR_PRIMARY_VAUE_HIGH_INTERN - 1] = 0x0000;
    RegInputBuff[slave_idx][SENSOR_PRIMARY_VAUE_LOW_INTERN - 1]  = 0x0000;
    
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
	 
/************************ (C) COPYRIGHT ONWERT *****END OF FILE****/