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
#include "FlashDriver.h"
#include "UARTSlaveSettings.h"
#include "HoldingRegisterSlaveHandler.h"
#include "user_mb_app.h"
#include "cmsis_os.h"
#include "stdbool.h"
/* ------------------------External variables -------------------------*/
extern uint16_t holdingRegsPart1[MAX_MODBUS_REGS_PART];  // Адреса 1-120
extern UART_HandleTypeDef huart1;
extern uint8_t ModBusSlaveDefaultDeviceAddr;
/* ------------------------Global variables----------------------------*/
uint8_t NumberOffDevices = 0;

SensorState_t   SensorStateArray[5];

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

/* ------------------------Locale variables----------------------------*/

/* ------------------------Functions-----------------------------------*/

void initSensorStateArray(uint8_t numberdevices)
  {
		for(int i = 0; i < numberdevices; i++)
		{
     SensorStateArray[i].DeviceStatus = 0x0000;
		 SensorStateArray[i].AlarmStatus  = 0x0000;
		 SensorStateArray[i].Concentration_H = 0x0000;
		 SensorStateArray[i].Concentration_L = 0x0000;
		 SensorStateArray[i].NotResponsCounter = 0x0000;
		 SensorStateArray[i].ErrorState = false;
		}
  }

/**
 * @brief Reads and processes current sensor state from Modbus input registers
 * @param slaveaddr Slave device address (1-based index)
 * @param RegInputBuff 2D array containing Modbus input registers for all slaves
 */
void readCurrentSensorState(uint8_t slaveaddr, uint16_t RegInputBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS])
{
    // Validate slave address
    if (slaveaddr < 1 || slaveaddr > MB_MASTER_TOTAL_SLAVE_NUM) {
        return; // or handle error appropriately
    }

    const size_t slave_idx = slaveaddr - 1;
    SensorState_t* sensor = &SensorStateArray[slave_idx];
    
    // Reset buffer values before reading
    sensor->DeviceStatus = 0x0000;
    
    // Read sensor data
    sensor->DeviceStatus     = RegInputBuff[slave_idx][DEVICE_STATUS_INTERN - 1];
    sensor->Concentration_H = RegInputBuff[slave_idx][SENSOR_PRIMARY_VAUE_HIGH_INTERN - 1];
    sensor->Concentration_L = RegInputBuff[slave_idx][SENSOR_PRIMARY_VAUE_LOW_INTERN - 1];
    
    // Clear the input buffer
    RegInputBuff[slave_idx][DEVICE_STATUS_INTERN - 1]        = 0x0000;
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

uint8_t getNumberDevices(void)
 {
   return NumberOffDevices;
 }
 
void setNumberDevices(uint8_t *numberDevices, uint8_t number)
 {
  *numberDevices = number;
 }
 
void setNextDeviceAddr(void)
 {
	 eMBErrorCode   eStatus;
	 
	 if(ModBusSlaveDefaultDeviceAddr < NumberOffDevices)
	 {
	   ModBusSlaveDefaultDeviceAddr++;
	 }
	 else 
	 {
	   ModBusSlaveDefaultDeviceAddr = 1;
	 
	 }
 		 eMBMasterDisable();
	   osDelay(5);
		 vMBMasterSetDestAddress(ModBusSlaveDefaultDeviceAddr);
		 osDelay(5);
	   eMBMasterEnable();
	   osDelay(5);
	}
 
	void setModBusSetting(void)
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
	
	void setModBusAddr(void)
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
	
/************************ (C) COPYRIGHT ONWERT *****END OF FILE****/