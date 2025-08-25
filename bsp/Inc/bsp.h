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
 #include "RingBuffer.h"
  /** @defgroup board_support
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
  
	#define TIME_DEFAULT_1       60
	#define TIME_STEP_MIN_2      40
  #define TIME_STEP_DEFAULT_2  40
	
	
	union ShortsToFloat {
    struct {
        short a;
        short b;
    } shorts;
    float f;
};
	
  typedef	struct
  {
    uint32_t ModBuAddrSetFlag;
    uint32_t ModBusAddr;
		uint32_t Reserved1;
		uint32_t Reserved2;
 } ModBusAddr_t ;
	
  typedef struct
	  {
			uint8_t  SensorModBudAddr;
			uint8_t DeviceModelCode[10];
			float SensorScaleMax;
			char SensorGas[10];
			char SensorScaleDimension[10];
			float SensorWarning;            // Порог 1
			float SensorAlarm;              // Порог 2
			uint8_t SensorSubstanceCode[16]; //тип газа
			uint16_t Concentration_H;
			uint16_t Concentration_L;
		  uint16_t DeviceStatus;
			float  Concentration;
		  uint16_t NotResponsCounter;
			_Bool ErrorState;
		}SensorState_t;
		
		
typedef struct {
    uint8_t count;
	  uint8_t realChannelNum;
    uint8_t modbusAddrs[NUMBER_SLAVE_DEVICES];
} SensorInfo_t; 
		
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
	  void readCurrentSensorState(uint8_t slaveaddr, uint16_t RegInputBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS],uint16_t RegHoldingBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS]);
  	void setModBusSlaveSetting(void);
		void setModBusSlaveAddr(void);
		void setNextActiveDeviceAddr(uint8_t *currentAddr);
	  uint8_t GetActiveSensors(SensorState_t SensorStateArray[NUMBER_SLAVE_DEVICES], SensorInfo_t *sensorinfo);
		void setNextActiveDeviceAddr_(uint8_t *currentAddr, uint8_t countsensores); 
		const char* getUnitStringByCode(uint8_t code);
		void readCurrentSensorValue(uint8_t slaveaddr, uint16_t RegInputBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS]);
		const char* getDeviceModelNameFromShorts(short part1, short part2);
		RX_Buffer_State Uart_Get_Byte(RING_buffer_t* buf, uint8_t* a);
		uint8_t getCodeByUnitString(const char* unitStr);
 /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _BOARD_SUPPORT_FILE_H */
