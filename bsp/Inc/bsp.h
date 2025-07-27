/**
******************************************************************************
* @file      bsp.h
* @author    OnWert
* @version   
* @brief     This file contains defines and all the functions prototypes for the bsp.c
******************************************************************************
*/
#ifndef _BOARD_SUPPORT_FILE_H
#define _BOARD_SUPPORT_FILE_H

#ifdef __cplusplus
extern "C" {
#endif
  
 /* ------------------------Includes ----------------------------------*/
 #include "stm32f4xx_hal.h"
 #include "user_mb_app_m.h"
 #include "ModBusAddrConverter.h"
  /** @defgroup board_support
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
 
  typedef	struct
  {
    uint32_t ModBuAddrSetFlag;
    uint32_t ModBusAddr;
		uint32_t Reserved1;
		uint32_t Reserved2;
 } ModBusAddr_t ;
	
  typedef struct
	  {
		  uint16_t DeviceStatus;
			uint16_t Concentration_H;
			uint16_t Concentration_L;
		  uint16_t NotResponsCounter;
			_Bool ErrorState;
		}SensorState_t;
		
	typedef struct
	  {
		  uint32_t SetFlag;
			uint32_t Timestep;
		}TimeStepReadingSensores_t;	
			
 /* ------------------------External variables -------------------------*/
extern  uint8_t NumberSlaveDevices;

  /** @addtogroup board_support
  * @{
  */
	
		uint8_t getNumberDevices(void);
    void setNumberDevices(uint8_t *numberDevices, uint8_t number);
		void setNextDeviceAddr(uint8_t *currentAddr);
		void initSensorStateArray(uint8_t numberdevices);
		void readCurrentSensorState( uint8_t slaveaddr, uint16_t RegInputBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS]);
  	void setModBusSlaveSetting(void);
		void setModBusSlaveAddr(void);
		void setNextActiveDeviceAddr(uint8_t *currentAddr);
		/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _BOARD_SUPPORT_FILE_H */
