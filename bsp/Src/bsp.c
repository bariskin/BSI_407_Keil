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
#include "cmsis_os.h"
#include "stdbool.h"
/* ------------------------External variables -------------------------*/
extern uint8_t ModBusSlaveDefaultDeviceAddr;
/* ------------------------Global variables----------------------------*/
uint8_t NumberOffDevices = 0;

SensorState_t   SensorStateArray[5];
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
 
/************************ (C) COPYRIGHT ONWERT *****END OF FILE****/