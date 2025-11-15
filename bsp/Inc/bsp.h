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
 #include "stdbool.h"
  /** @defgroup board_support
  * @{
  */
  
  /**
  * @}
  */
 /* ------------------------Defines ----------------------------------*/
  
	#define TIME_DEFAULT_1               160
	#define TIME_STEP_MIN_2              160
  #define TIME_STEP_DEFAULT_150_MS     58
	
	#define TIME_DELAY_PACKET            280
	
	#define TIME_DELAY_BEFORE_AFTER_CMD  470
	
	#define CALIBRATION_STATUS_PREPARE                     0x00
	#define CALIBRATION_STATUS_IN_PROGRESS                 0x01
	#define CALIBRATION_STATUS_PUMPING                     0x02
	#define CALIBRATION_STATUS_SUCCESFUL_COMPLETED         0x07
	
	#define CALIBRATION_STATUS_SENSOR_BROKEN               0x03
	#define CALIBRATION_STATUS_POINT_VALUE_NOT_VALID       0x04
	#define CALIBRATION_STATUS_SENSETIVITY_NOT_VALID       0x05
	#define CALIBRATION_STATUS_CALIBRATION_ID_NOT_ACTIVE   0x06
	#define CALIBRATION_STATUS_CORRELATION_TYPE_NOT_MATCH  0x09
	
	
	
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
			uint8_t SensorModBudAddr;
			
			bool WasConnected;
			uint16_t NotResponsCounter;
			bool ErrorState;
			uint8_t CalibrationStatus;
			
			bool WasConnected_2;
			uint16_t NotResponsCounter_2;
			bool ErrorState_2;
			uint8_t CalibrationStatus_2;
			
		/* Для первого  датчика по адресу modbus  */	  	
			uint8_t DeviceModelCode[10];
			float SensorScaleMax;
			char SensorGas[10];
			char SensorScaleDimension[10];
			float SensorWarning;            // Порог 1
			float SensorAlarm;              // Порог 2
			float SensorAlarm2;             // Порог 3
			uint8_t SensorSubstanceCode[16]; //тип газа
			uint16_t Concentration_H;
			uint16_t Concentration_L;
		  uint16_t DeviceStatus;
			float  Concentration;
	/* Для второго датчика по тому же адресу modbus  */	  
			uint8_t DeviceModelCode_2[10];
			float SensorScaleMax_2;
			char SensorGas_2[10];
			char SensorScaleDimension_2[10];
			float SensorWarning_2;            // Порог 1
			float SensorAlarm_2;              // Порог 2
			float SensorAlarm2_2;             // Порог 3
			uint8_t SensorSubstanceCode_2[16]; //тип газа
			uint16_t Concentration_H_2;
			uint16_t Concentration_L_2;
		  uint16_t DeviceStatus_2;
			float  Concentration_2;		
		}SensorState_t;
		
			
  typedef struct
	  {
			uint8_t SensorScaleDimensionID;
			uint32_t SensorScaleMax;
			uint32_t SensorWarning;            
			uint32_t SensorAlarm;              
			uint32_t SensorAlarm2;             
		}SensorCurrentState_t;
		
		
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
 

typedef struct {
    bool alarm2_triggered;
    bool alarm_triggered; 
    bool warning_triggered;
	  bool alarm2_triggered2;
    bool alarm_triggered2; 
    bool warning_triggered2;	
} ThresholdState;
			
 /* ------------------------External variables -------------------------*/
extern  uint8_t NumberSlaveDevices;
		
extern bool checkParamsValue ;

typedef enum 
  {
    CASE_DEVICE_MODEL_CODE                =    0,
		CASE_SENSOR_SCALE_MAX_HIGH            =    1,
    CASE_SENSOR_SCALE_MAX_HIGH_2          =    2,
		CASE_SENSOR_THRESHOLD_WARNIGN_HIGN    =    3,
		CASE_SENSOR_THRESHOLD_WARNIGN_HIGN_2  =    4,
		CASE_SENSOR_SUBSTANCE_CODE_1          =    5,
		CASE_SENSOR_SUBSTANCE_CODE_1_2        =    6,
		CASE_SENSOR_PRIMARY_VALUE_HIGH        =    7, 
		CASE_SENSOR_SECONDARY_VALUE_HIGH      =    8,
		CASE_READ_CURRENT_STATTE              =    9,
		CASE_WAITNG_CALBRATION_STATE          =    10,
		CASE_READING_CALBRATION_STATE         =    11,
		CASE_GET_CALBRATION_STATE             =    12,
		CASE_READING_CONCENTRATION_1          =    13,
		CASE_READING_CONCENTRATION_2          =    14,
		CASE_SET_NEXT_ADDR                    =    15,
	  CASE_WRITING_SETTING                  =    16
  } eCase;

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
		bool compareParams(SensorCurrentState_t *writeParams, SensorCurrentState_t *reasParams );
		void readCurrentCalibrationState(uint8_t slaveaddr,uint16_t RegHoldingBuff[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS]);
    bool getCalibrationProcessState(uint8_t slaveaddr);
		HAL_StatusTypeDef Set_RTC_Year(uint8_t year);
		uint16_t Get_RTC_Year(void);
		HAL_StatusTypeDef Set_RTC_Month(uint8_t month);
		uint8_t Get_RTC_Month(void);
		HAL_StatusTypeDef Set_RTC_Day(uint8_t day);
		uint8_t Get_RTC_Day(void);
		HAL_StatusTypeDef Set_RTC_Hour(uint8_t hour);
		uint8_t Get_RTC_Hour(void);
		HAL_StatusTypeDef Set_RTC_Minute(uint8_t minute);
		uint8_t Get_RTC_Minute(void);
		HAL_StatusTypeDef Set_RTC_Second(uint8_t second);
		uint8_t Get_RTC_Second(void);
		HAL_StatusTypeDef RTC_SetFromHexString(char* hex_str, uint8_t size);
    HAL_StatusTypeDef hex_to_ascii_minimal(char* hex_str, char* ascii_buf);
		void SendTimeToNextion(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute);
		void UpdateDisplayTime(void);
		uint8_t findSensorID(uint8_t addrArr[], int size, uint8_t addrValue, uint8_t sensorNumber); 
 /**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif  /* _BOARD_SUPPORT_FILE_H */
