/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mb.h"
#include "mb_m.h"
#include "ModBusAddrConverter.h"
#include "bsp.h"
#include "HoldingRegisterSlaveHandler.h"
#include "stdbool.h"
#include "DisplayDriver.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "UARTSlaveSettings.h"
#include "RingBuffer.h"
#include "File_Handling.h"
#include "SensorLogs.h"
#include "RelayModule.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t SelectaFlag = 0;
volatile  bool requestConcetration1 = false; 
volatile  bool requestConcetration2 = false; 

extern volatile uint8_t startDisplayFlag;
extern volatile uint8_t packet_ready;   
extern UART_HandleTypeDef huart3;
extern RING_buffer_t ring_Rx;   /* RX ring buffer structur */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern USHORT   usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];
extern USHORT   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];
extern SensorState_t   SensorStateArray[NUMBER_SLAVE_DEVICES];

extern volatile char arrDisplayRX[ARRAY_RX_SIZE];
extern volatile uint8_t displayResponse;
extern volatile uint8_t displayStartedFlag ;
extern  uint8_t packets_received;

uint8_t  numberOfDevices = NUMBER_SLAVE_DEVICES;
uint16_t BaudRateID = 0;
extern SensorInfo_t SensorInfo;
extern  uint8_t channelID;

extern float  updateThresholdWarning;
extern float  updateThresholdAlarm ;
extern float  updateCalibrationValue;


extern  SensorCurrentState_t	writeParams;	 
extern  SensorCurrentState_t	readParams ;

 uint16_t registersTX[4] = {0};
 uint32_t binary32 = 0;
 uint8_t ModbusSensoraddr = 0;

eCase SelectRunFlag = CASE_DEVICE_MODEL_CODE;

uint8_t ControlCycleFlag = 0; 
uint8_t ControlCycleFlag2 = 0;  
 
extern bool sd_card_present; 
 
 
volatile uint8_t CmdIsReady = 0;
volatile uint8_t CmdWriteIsReady = 0;
volatile uint8_t PauseTaskCounter = 0;
 
 
   /* Глобальная очередь для команд */
QueueHandle_t displayCommandQueue = NULL;
 
 DisplayCommand_t cmd =
   {
	   .command     = 0x00,
     .deviceAddr  = 0x00,
     .binary32    = 0x00,
	   .channelID   = 0x00,
	   .sensorPOSITION   = 0x00
	 };

  /* Глобальная очередь для логов */
 
osMessageQId queueSendLogsHandle;
QueueHandle_t SD_CardMsgQueue = NULL;
 
SensorLogEvent_t sensorLog = {
	 .logType = SENSOR_LOG_TYPE_ERROR,
   .sensorID = 0,
   .deviceAddr = 0,
   .Value = 0
}; 

 /* Глобальная очередь событий для модлуей реле */
QueueHandle_t relaysCommandQueue = NULL;

 RelaysEvent_t relayEvent = {
	 .module_id  = 0,
   .relays_id  = 0,
   .channel_id = 0, 
	 .warning    = 0,
	 .alarm_1    = 0,
   .alarm_2    = 0,
   .error      = 0
}; 


 union {
     uint32_t i;
     float f;
 } converterFloat;

volatile uint8_t RdyWrittingFlag = 0;
float currentConcentration = 0.00;

volatile uint8_t  flagDisplayLogsBusy = 0;
 
 
static uint16_t Modbus_CRC16(uint8_t *buf, uint8_t len);
static void Send_Modbus_Command_DMA(uint8_t slave_addr, uint8_t data);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define mutexON  1 
#define mutexOFF 0 
volatile uint8_t additMutexMbFunctions = mutexOFF;

bool setErrorStatusFlag = false;

DisplayCommand_t displayCmd;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
 
void MasterModbus2TaskFunction(void const * argument); 
extern uint8_t ModBusSlaveDefaultDeviceAddr;
extern uint8_t ModBusSlaveCurrentDeviceAddr;
/* USER CODE END Variables */
osThreadId SlaveModbusTaskHandle;
uint32_t defaultTaskBuffer[ 256 ];
osStaticThreadDef_t defaultTaskControlBlock;
osThreadId MasterModbusTasHandle;
uint32_t MasterModbusTasBuffer[ 256 ];
osStaticThreadDef_t MasterModbusTasControlBlock;
osThreadId HoldingHandlerHandle;
uint32_t HoldingHandlerBuffer[ 1024 ];
osStaticThreadDef_t HoldingHandlerControlBlock;
osThreadId InputHandlerHandle;
uint32_t InputHandlerBuffer[ 512 ];
osStaticThreadDef_t InputHandlerControlBlock;
osThreadId SlaveEventTaskHandle;
uint32_t SlaveEventTaskBuffer[ 512];
osStaticThreadDef_t SlaveEventTaskControlBlock;
osThreadId DisplayTaskHandle;
uint32_t DisplayTaskBuffer[ 512];
osStaticThreadDef_t DisplayTaskControlBlock;
osThreadId SendToDispTaskHandle;
uint32_t SendToDispTaskBuffer[ 1600];
osStaticThreadDef_t SendToDispTaskControlBlock;
osMutexId myMutex01Handle;
osStaticMutexDef_t myMutex01ControlBlock;


osThreadId MasterModbus2TasHandle;
uint32_t MasterModbus2TasBuffer[ 128 ];
osStaticThreadDef_t MasterModbus2TasControlBlock;


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
uint16_t dimensionArray[] = {SENSOR_SCALE_DIMENSTION,SENSOR_SCALE_DIMENSTION_2};
uint16_t scale_maxArray[] = {SENSOR_SCALE_MAX_HIGH, SENSOR_SCALE_MAX_HIGH_2};
uint16_t warningArray[]   = {SENSOR_THRESHOLD_WARNIGN_HIGN, SENSOR_THRESHOLD_WARNIGN_HIGN_2};
uint16_t alarmArray[]     = {SENSOR_THRESHOLD_ALARM_HIGH, SENSOR_THRESHOLD_ALARM_HIGH_2};
uint16_t additionalArray[]  = {SENSOR_THRESHOLD_ADDITIONAL_HIGH,SENSOR_THRESHOLD_ADDITIONAL_HIGH_2};
uint16_t calibrationArray[] = {CALIBRATION_PRIMARY_ZERO_VALUE_HIGH,CALIBRATION_SECONDARY_ZERO_VALUE_HIGH};
uint16_t calibrationArray2[] = {CALIBRATION_PRIMARY_SPAN_VALUE_HIGH,CALIBRATION_SECONDARY_SPAN_VALUE_HIGH};
/* USER CODE END FunctionPrototypes */

void SlaveModbusTaskFunction(void const * argument);
void MasterModbusTaskFunction(void const * argument);
void HoldingHandlerFunction(void const * argument);
void InputHandlerFunction(void const * argument);
void SlaveEventFunction(void const * argument);
void DisplayTaskFunction(void const * argument);
void SendToDispTaskFunction(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
  osMutexStaticDef(myMutex01, &myMutex01ControlBlock);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of SlaveModbusTask */
  osThreadStaticDef(SlaveModbusTask, SlaveModbusTaskFunction, osPriorityNormal, 0, 256, defaultTaskBuffer, &defaultTaskControlBlock);
  SlaveModbusTaskHandle = osThreadCreate(osThread(SlaveModbusTask), NULL);

  /* definition and creation of MasterModbusTas */
  osThreadStaticDef(MasterModbusTas, MasterModbusTaskFunction, osPriorityAboveNormal, 0, 256, MasterModbusTasBuffer, &MasterModbusTasControlBlock);
  MasterModbusTasHandle = osThreadCreate(osThread(MasterModbusTas), NULL);

  /* definition and creation of HoldingHandler */
  osThreadStaticDef(HoldingHandler, HoldingHandlerFunction, osPriorityAboveNormal, 0, 1024, HoldingHandlerBuffer, &HoldingHandlerControlBlock);
  HoldingHandlerHandle = osThreadCreate(osThread(HoldingHandler), NULL);

  /* definition and creation of InputHandler */
  osThreadStaticDef(InputHandler, InputHandlerFunction, osPriorityBelowNormal, 0, 512, InputHandlerBuffer, &InputHandlerControlBlock);
  InputHandlerHandle = osThreadCreate(osThread(InputHandler), NULL);

  /* definition and creation of SlaveEventTask */
  osThreadStaticDef(SlaveEventTask, SlaveEventFunction, osPriorityBelowNormal, 0,512 , SlaveEventTaskBuffer, &SlaveEventTaskControlBlock);
  SlaveEventTaskHandle = osThreadCreate(osThread(SlaveEventTask), NULL);

  /* definition and creation of DisplayTask */
  osThreadStaticDef(DisplayTask, DisplayTaskFunction, osPriorityBelowNormal, 0, 512, DisplayTaskBuffer, &DisplayTaskControlBlock);
  DisplayTaskHandle = osThreadCreate(osThread(DisplayTask), NULL);

  /* definition and creation of SendToDispTask */
  osThreadStaticDef(SendToDispTask, SendToDispTaskFunction, osPriorityBelowNormal, 0, 1600 , SendToDispTaskBuffer, &SendToDispTaskControlBlock);
  SendToDispTaskHandle = osThreadCreate(osThread(SendToDispTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	
		  /* definition and creation of MasterModbus2Tas */
  osThreadStaticDef(MasterModbus2Tas, MasterModbus2TaskFunction, osPriorityBelowNormal, 0, 128, MasterModbus2TasBuffer, &MasterModbus2TasControlBlock);
  MasterModbus2TasHandle = osThreadCreate(osThread(MasterModbus2Tas), NULL);
	
	
	displayCommandQueue = xQueueCreate(20, sizeof(DisplayCommand_t));
	
	
  /* очередь для работы с логами */
	osMessageQDef(queueSenEvent, 24, SensorLogEvent_t);
  queueSendLogsHandle = osMessageCreate(osMessageQ(queueSenEvent), NULL);
	
		 /* очередь для работы с реле */
	relaysCommandQueue = xQueueCreate(4, sizeof(relayEvent));
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_SlaveModbusTaskFunction */
/**
  * @brief  Function implementing the SlaveModbusTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_SlaveModbusTaskFunction */
void SlaveModbusTaskFunction(void const * argument)
{
  /* USER CODE BEGIN SlaveModbusTaskFunction */
  /* Infinite loop */
  for(;;)
  {
		eMBPoll();
    osDelay(5);
  }
  /* USER CODE END SlaveModbusTaskFunction */
}

/* USER CODE BEGIN Header_MasterModbusTaskFunction */
/**
* @brief Function implementing the MasterModbusTas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MasterModbusTaskFunction */
void MasterModbusTaskFunction(void const * argument)
{
  /* USER CODE BEGIN MasterModbusTaskFunction */
  /* Infinite loop */
  for(;;)
  {
		// Пытаемся захватить мьютекс (ждём 100 мс)
     osStatus status = osMutexWait(myMutex01Handle, 10);
		 if (status == osOK) {		 
		     eMBMasterPoll();
			  // Освобождаем мьютекс
       osMutexRelease(myMutex01Handle);
		 }
    osDelay(1);
  }
  /* USER CODE END MasterModbusTaskFunction */
}

/* USER CODE BEGIN Header_HoldingHandlerFunction */
/**
* @brief Function implementing the HoldingHandler thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_HoldingHandlerFunction */
void HoldingHandlerFunction(void const * argument)
{
  /* USER CODE BEGIN HoldingHandlerFunction */
	static uint8_t HoldingPollsDone = 0;  // Счётчик выполненных опросов Holding-регистров
		 SendNextionCommand("gl_por1=0");
	   osDelay(50);
	   SendNextionCommand("gl_por2=0");
	   osDelay(50);
	   gl_por1 = 0;
     gl_por2 = 0;
     gl_NotConnected = 0;	
	
     osDelay(10000);
	/* Infinite loop */
  for(;;)
  {	// Пытаемся захватить мьютекс (ждём 50 мс)
     osStatus status = osMutexWait(myMutex01Handle, 10);
		 if (status == osOK) 
			  {
			  
			   if(HoldingPollsDone  < 3) // опрашиваем первые ТРИ РАЗА все регистры 
				 {	 
			
			     /* *********************************  Handling HOLDING registers *************************** */
				    if(SelectRunFlag == CASE_DEVICE_MODEL_CODE)
				     { // вычитывается модель прибора   
					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, DEVICE_MODEL_CODE,2, 200 );
					
							 SelectRunFlag = CASE_SENSOR_SCALE_MAX_HIGH;
				     }
				    else if(SelectRunFlag == CASE_SENSOR_SCALE_MAX_HIGH)
				     { // вычитываются значения дипазона  всей шкалы и единицы измерения
					     eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SCALE_MAX_HIGH,3, 200 );
					     
							 SelectRunFlag = CASE_SENSOR_SCALE_MAX_HIGH_2;
				     }	
						else if(SelectRunFlag == CASE_SENSOR_SCALE_MAX_HIGH_2)
				     { // вычитываются значения дипазона  всей шкалы и единицы измерения
					     eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SCALE_MAX_HIGH_2,3, 200 );
					     
							 SelectRunFlag = CASE_SENSOR_THRESHOLD_WARNIGN_HIGN;
				     }	
						  
				    else if (SelectRunFlag == CASE_SENSOR_THRESHOLD_WARNIGN_HIGN)
					   { //вычитываются пороги 1, 2 и 3
 					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_THRESHOLD_WARNIGN_HIGN, 6, 200 );
	           
					    SelectRunFlag = CASE_SENSOR_THRESHOLD_WARNIGN_HIGN_2;
					   }
						else if (SelectRunFlag == CASE_SENSOR_THRESHOLD_WARNIGN_HIGN_2)
					   { //вычитываются пороги 1, 2 и 3
 					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_THRESHOLD_WARNIGN_HIGN_2, 6, 200 );
	           
					    SelectRunFlag = CASE_SENSOR_SUBSTANCE_CODE_1;
					   } 
				    else if (SelectRunFlag == CASE_SENSOR_SUBSTANCE_CODE_1)
					    { //вычитывается тип газа
					     eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SUBSTANCE_CODE_1, 16, 200 );
					    
							 SelectRunFlag = CASE_SENSOR_SUBSTANCE_CODE_1_2;	
					    } 
            else if (SelectRunFlag == CASE_SENSOR_SUBSTANCE_CODE_1_2)
					    { //вычитывается тип газа
					     eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SUBSTANCE_CODE_1_2, 16, 200 );
					    
							 SelectRunFlag = CASE_SENSOR_PRIMARY_VALUE_HIGH;	
					    } 
							
			      /* ********************************* Handling INPUT registers *************************** */
				     else if (SelectRunFlag == CASE_SENSOR_PRIMARY_VALUE_HIGH)
					    {
							 requestConcetration1 = true;
					     eMBMasterReqReadInputRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_PRIMARY_VALUE_HIGH, 3, 200 );
               				    
							 SelectRunFlag = CASE_SENSOR_SECONDARY_VALUE_HIGH;
								
								if( ControlCycleFlag)
								 {
									 SelectRunFlag = CASE_SENSOR_SECONDARY_VALUE_HIGH; 
								 }
					    } 	
						 else if (SelectRunFlag == CASE_SENSOR_SECONDARY_VALUE_HIGH)
					    {			
								 requestConcetration2 = true;
								 eMBMasterReqReadInputRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SECONDARY_VALUE_HIGH, 3, 200 );
								
								 SelectRunFlag = CASE_READ_CURRENT_STATTE;
								
								if( ControlCycleFlag)
								 {
									 SelectRunFlag = CASE_READ_CURRENT_STATTE; 
								 }		
              }
				    /* ********************************* set next slave addr *************************** */	
				     else if (SelectRunFlag == CASE_READ_CURRENT_STATTE)
				      {
								if(ControlCycleFlag)
								{
								  readCurrentSensorState(ModBusSlaveCurrentDeviceAddr,usMRegInBuf,usMRegHoldBuf);
								  ControlCycleFlag = 0;
									HoldingPollsDone = 3;
									SelectRunFlag = CASE_READING_CONCENTRATION_1;
									checkParamsValue = false;
								  checkParamsValue	= compareParams((SensorCurrentState_t *)&writeParams,(SensorCurrentState_t *)&readParams);
									setErrorStatusFlag = true;
								}
								
						 else	if(ControlCycleFlag2)
								{
								  readCurrentSensorState(ModBusSlaveCurrentDeviceAddr,usMRegInBuf,usMRegHoldBuf);
								  ControlCycleFlag2 = 0;
									HoldingPollsDone = 3;
									SelectRunFlag = CASE_READING_CONCENTRATION_1;
									checkParamsValue = false;
								  checkParamsValue	= compareParams((SensorCurrentState_t *)&writeParams,(SensorCurrentState_t *)&readParams);
									setErrorStatusFlag = true;
								}	
									
							else{
								
					         readCurrentSensorState(ModBusSlaveCurrentDeviceAddr,usMRegInBuf,usMRegHoldBuf);
				           setNextDeviceAddr(&ModBusSlaveCurrentDeviceAddr);	       // set next device addr
                   SelectRunFlag = CASE_DEVICE_MODEL_CODE;
								
			
					         if(ModBusSlaveCurrentDeviceAddr == NUMBER_SLAVE_DEVICES)
					          {
							       HoldingPollsDone++; // следующий	 круг из трех, при старте девайса
								     ModBusSlaveCurrentDeviceAddr = 1; 
											
										 updateProgressBar(HoldingPollsDone);
											 
							      }
					         if(HoldingPollsDone == 3) // после трех проходов определяем наличие активных приборов
					          {
									    SendNextionCommand ("page page%d", 0);
						          SelectRunFlag = CASE_READING_CONCENTRATION_1;	               // переход на постояный цикл опроса значений концентрации
									
									   /* получение информации об активных датчиках их адресах */
									   GetActiveSensors(SensorStateArray, (SensorInfo_t *) &SensorInfo);
											/* ************************************* */
											sensorLog.Value = SensorInfo.count;
											sensorLog.logType = SERVICE;
											sensorLog.sensorID = 0;
											
											if(sd_card_present)
											{
												if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
													}
										  }
											/* ************************************* */	
											 
									   /* получение modbus адреса  первого активного датчика на линии */
								     ModBusSlaveCurrentDeviceAddr = SensorInfo.modbusAddrs[0];  
                    /* вывести окна активных дачтичиков  и перейти на постоянный опрос */
								   }				
				        }	
						 }								
			     }	

           else if (SelectRunFlag == CASE_WAITNG_CALBRATION_STATE)
					   { 
							  static uint8_t waitingTimeCounter = 0;
							 
							  osMutexRelease(myMutex01Handle);
                osDelay(100);
                osMutexWait(myMutex01Handle, 10);
							 
							  waitingTimeCounter++;
							 
							  if(waitingTimeCounter > 60) // прмиерно 8 секунда время ожидания проответа регистра состояния
								{
									 waitingTimeCounter = 0;
								}
							  if(waitingTimeCounter == 0)
							  {
							   SelectRunFlag = CASE_READING_CALBRATION_STATE;
								}
					   }
				  	else if (SelectRunFlag == CASE_READING_CALBRATION_STATE)
					   { 
 					      eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, CALIBRATION_PROCESS_STATUS, 1, 300 );
							 
							  SelectRunFlag = CASE_GET_CALBRATION_STATE;
					   } 
						 	 	 
					 else if (SelectRunFlag == CASE_GET_CALBRATION_STATE)
					   {  
							   checkParamsValue = false; 
							   readCurrentCalibrationState (ModBusSlaveCurrentDeviceAddr,usMRegHoldBuf);
							   checkParamsValue = getCalibrationProcessState (ModBusSlaveCurrentDeviceAddr);	
							   checkParamsValue = !checkParamsValue; 
							   setErrorStatusFlag = true;
					       SelectRunFlag = CASE_READING_CONCENTRATION_1;
					   }	 
						 
			      /* постоянный цикл опроса активных приборов */
				
			        /* ********************************* Handling INPUT registers *************************** */
				  else if (SelectRunFlag == CASE_READING_CONCENTRATION_1)
				  	    {
								 /*  отправка  запроса на считывания значение текущей концентрации */
								  /* !!!!! на период настройки параметров с дисплея  отключается запрос концентрации !!!!! */ 
									
                   SelectRunFlag = CASE_READING_CONCENTRATION_2;
									
									if(!CmdIsReady && !flagDisplayLogsBusy){ 
										 requestConcetration1 = true;
					           eMBMasterReqReadInputRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_PRIMARY_VALUE_HIGH, 3, 200 );
									 }
									else {
									   SelectRunFlag = CASE_WRITING_SETTING;
									}
					       
					      } 		
								
					else if(SelectRunFlag == CASE_READING_CONCENTRATION_2)
					      {
								 /*  отправка  запроса на считывания значение текущей концентрации */
								  /* !!!!! на период настройки параметров с дисплея  отключается запрос концентрации !!!!! */ 
									
                   SelectRunFlag = CASE_SET_NEXT_ADDR;
									
									if(!CmdIsReady && !flagDisplayLogsBusy){ 
										 requestConcetration2 = true;
					           eMBMasterReqReadInputRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SECONDARY_VALUE_HIGH, 3, 200 );
									 }
									else {
									   SelectRunFlag = CASE_WRITING_SETTING;
									}
								
								}
					
				      /* ********************************* set next slave addr *************************** */	
				   else if (SelectRunFlag == CASE_SET_NEXT_ADDR)
				        {
										
									 SelectRunFlag = CASE_WRITING_SETTING;
									
									 if(!CmdIsReady && !flagDisplayLogsBusy){ 
						    	   /* значение концентрации текущее */
					            readCurrentSensorValue(ModBusSlaveCurrentDeviceAddr,usMRegInBuf);
									
										 
					           /* выбираем только адреса активных приборов */
				            setNextActiveDeviceAddr_(&ModBusSlaveCurrentDeviceAddr,SensorInfo.count);	       // set next active sdevice addr
										 
									   // Отправляем только если значение ИЗМЕНИЛОСЬ
										 
										
	   			       }	
						 }									 
		else if (SelectRunFlag == CASE_WRITING_SETTING)	
		{		
      uint8_t shouldChangeFlag = 1;	 // для обычного цикла, когда команды не прилетают

			/* *************** ОБРАБОТКА КОМАНД ОТ ДИСПЛЕЯ *************** */
    // Проверяем, есть ли команды от дисплея в очереди
	 if(CmdIsReady)
	 { 
     //uint8_t sensorPosition = displayCmd.sensorPOSITION; // FIRST or SECONDARY
		 
    if(xQueueReceive(displayCommandQueue, &displayCmd, 0) == pdTRUE)
    {  	                                       
			    if(displayCmd.command ==DISPLAY_SCALE_DIMENSION){	 
						
						 registersTX[0] = displayCmd.binary32 & 0xFFFF;
              //registersTX[1] = displayCmd.binary32 & 0xFFFF;
						
					   osMutexRelease(myMutex01Handle);
             osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
             osMutexWait(myMutex01Handle, 10);
				    		 
						 eMBMasterReqWriteMultipleHoldingRegister(  displayCmd.deviceAddr, 
						                                            dimensionArray[displayCmd.sensorPOSITION], 
						                                             1, 
						                                             (USHORT *)&registersTX[0], 
						                                             300); 
											
						osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_PACKET);
            osMutexWait(myMutex01Handle, 10);
						
						shouldChangeFlag = 0;
				    SelectRunFlag = CASE_WRITING_SETTING;
					 }
			
				/* ******************  DISPLAY_SCALE_MAX *************************** */	
			 else		if(displayCmd.command ==DISPLAY_SCALE_MAX){		
				 
				   registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
           registersTX[1] = displayCmd.binary32 & 0xFFFF;
 
           osMutexRelease(myMutex01Handle);
           osDelay(TIME_DELAY_PACKET);
           osMutexWait(myMutex01Handle, 10);
					 	
           eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr, 
				                                            scale_maxArray[displayCmd.sensorPOSITION], 
				                                            2, 
				                                            (USHORT *)&registersTX[0], 
				                                            300); 

           osMutexRelease(myMutex01Handle);
           osDelay(TIME_DELAY_PACKET);
           osMutexWait(myMutex01Handle, 10);

				   shouldChangeFlag = 0;
				   SelectRunFlag = CASE_WRITING_SETTING; 
		    }
				/* ******************  DISPLAY_CALIBRATION_PRIMARY_ZERO ********************** */	  
				else	if(displayCmd.command == DISPLAY_CALIBRATION_PRIMARY_ZERO){
					 /* ******************************************************************* */
					
					sensorLog.sensorID = displayCmd.channelID;
					converterFloat.i = displayCmd.binary32;
					sensorLog.Value = converterFloat.f ;
					sensorLog.logType = CALIBRATION_0;
					
						if(sd_card_present)		{				
								if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
															}
						}
					 /* ******************************************************************* */ 
					registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
          registersTX[1] = displayCmd.binary32 & 0xFFFF;
					
					osMutexRelease(myMutex01Handle);
          osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
          osMutexWait(myMutex01Handle, 10);
					
          eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr, 
					                                             calibrationArray[displayCmd.sensorPOSITION], 
					                                             2, 
					                                             (USHORT *)&registersTX[0], 
					                                             300);	 

          osMutexRelease(myMutex01Handle);
          osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
          osMutexWait(myMutex01Handle, 10);

			    shouldChangeFlag = 0;
			    
					
				
			      CmdWriteIsReady = 1;
					
					 /* пройти один цикл опроса состояния калибровки  */
				    SelectRunFlag = CASE_WAITNG_CALBRATION_STATE;
						ModBusSlaveCurrentDeviceAddr = displayCmd.deviceAddr;	
	     }
        /* ******************  DISPLAY_CALIBRATION_POINT_1 *************************** */	  
	  else   if(displayCmd.command == DISPLAY_CALIBRATION_POINT_1){	
			      
			    /* ******************************************************************* */
			     sensorLog.sensorID = displayCmd.channelID;
			     converterFloat.i = displayCmd.binary32;
					 sensorLog.Value = converterFloat.f; 
					 sensorLog.logType = CALIBRATION_1;
			
						if(sd_card_present)		{						
							 if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
														}
						}
			      /* ******************************************************************* */
	
			      registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
            registersTX[1] = displayCmd.binary32 & 0xFFFF;
			
			      osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
            osMutexWait(myMutex01Handle, 10);
							    
            eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr, 
			                                               calibrationArray2[displayCmd.sensorPOSITION], 
			                                               2, 
			                                               (USHORT *)&registersTX[0], 
			                                               300);
            osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
            osMutexWait(myMutex01Handle, 10);

			      shouldChangeFlag = 0;
			      CmdWriteIsReady = 1;   
						
						/* пройти один цикл опроса состояния калибровки  */
				    SelectRunFlag = CASE_WAITNG_CALBRATION_STATE;
						ModBusSlaveCurrentDeviceAddr = displayCmd.deviceAddr;
		    }  
         /* ******************  DISPLAY_THRESHOLD_WARNING************************ */	 
			 else	 if(displayCmd.command == DISPLAY_THRESHOLD_WARNING){
				   
				    /* ******************************************************************* */
				    sensorLog.sensorID = displayCmd.channelID;
				    converterFloat.i = displayCmd.binary32;
					  sensorLog.Value = converterFloat.f ; 
					  sensorLog.logType = THRESHOLD_WARNING;
						
				    if(sd_card_present)		{						
								if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
														}
						}
				    /* ******************************************************************* */
				 
             registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
             registersTX[1] = displayCmd.binary32 & 0xFFFF;
				 
				 
				     osMutexRelease(myMutex01Handle);
             osDelay(TIME_DELAY_PACKET);
             osMutexWait(myMutex01Handle, 10);
				 
             eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr, 
				                                              warningArray[displayCmd.sensorPOSITION], 
				                                              2, 
				                                              (USHORT *)&registersTX[0], 
				                                              300);
				     osMutexRelease(myMutex01Handle);
             osDelay(TIME_DELAY_PACKET);
             osMutexWait(myMutex01Handle, 10);
				   
             shouldChangeFlag = 0;
				     SelectRunFlag = CASE_WRITING_SETTING;
	      }
          /* ******************  DISPLAY_THRESHOLD_ALARM ************************ */	  
       else if(displayCmd.command == DISPLAY_THRESHOLD_ALARM){
				 
				    osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_PACKET);
            osMutexWait(myMutex01Handle, 10);
				  
				    /* ******************************************************************* */
				    sensorLog.sensorID = displayCmd.channelID;
				    converterFloat.i = displayCmd.binary32;
					  sensorLog.Value = converterFloat.f; 
					  sensorLog.logType = THRESHOLD_ALARM;
						
				    if(sd_card_present)		{						
							if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
													}
						
						}
				     /* ******************************************************************* */
            registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
            registersTX[1] = displayCmd.binary32 & 0xFFFF;	
				 
            eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr,
				                                             alarmArray[displayCmd.sensorPOSITION], 
				                                             2, 
				                                             (USHORT *)&registersTX[0], 
			                                               300);	
            osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_PACKET);
            osMutexWait(myMutex01Handle, 10);

				 
				    shouldChangeFlag = 0;
				    SelectRunFlag = CASE_WRITING_SETTING;
        }
				/* ******************  DISPLAY_THRESHOLD_ADDITIONAL ************************ */	 
			 else	if(displayCmd.command == DISPLAY_THRESHOLD_ADDITIONAL){
				    
				    /* ******************************************************************* */
				    sensorLog.sensorID = displayCmd.channelID;
				    converterFloat.i = displayCmd.binary32;
					  sensorLog.Value = converterFloat.f; 
					  sensorLog.logType = THRESHOLD_ADDITIONAL;
						
				    if(sd_card_present)		{						
								if (xQueueSend(queueSendLogsHandle, &sensorLog, portMAX_DELAY) != pdPASS) {
														}
						}
				     /* ******************************************************************* */
				    registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
            registersTX[1] = displayCmd.binary32 & 0xFFFF;	
				 
				    osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_PACKET);
            osMutexWait(myMutex01Handle, 10);
				 
            eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr,
				                                             additionalArray[displayCmd.sensorPOSITION], 
				                                             2, 
				                                             (USHORT *)&registersTX[0], 
				                                             300); 
				    osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
            osMutexWait(myMutex01Handle, 10);
				 
				    shouldChangeFlag = 0;
				    CmdWriteIsReady = 1;
				   /* Для вычичитки записанных данных  пройти один цикл опроса */
				    SelectRunFlag = CASE_DEVICE_MODEL_CODE;
						
						if(displayCmd.sensorPOSITION == FIRST_SENSOR)
						{
							ControlCycleFlag = 1;
							ControlCycleFlag2 = 0;
						}
						else 
						{
							ControlCycleFlag = 0;
							ControlCycleFlag2 = 1;
						}	
						
						HoldingPollsDone = 0;
						
						ModBusSlaveCurrentDeviceAddr = displayCmd.deviceAddr;
				 	 /* ******************************************************** */
				    
        }
      }	
		}
	   if(shouldChangeFlag)
		   {	    
		    SelectRunFlag = CASE_READING_CONCENTRATION_1;
	    }		 
	 }	
			//Освобождаем мьютекс
       osMutexRelease(myMutex01Handle);
		 }
		  
	 /* ************* osDelay()****************** */
		  
		  if(CmdIsReady)
			{	 
			 if(CmdWriteIsReady)
				 {
					CmdIsReady = 0;
					CmdWriteIsReady = 0;
					osDelay(1);
				 }
			} 
		  else if(HoldingPollsDone == 3) 
		   { 
	  			osDelay(TIME_STEP_DEFAULT_150_MS); 	    // 150 ms 
		   } 
		  else
		   {
		   osDelay(TIME_DEFAULT_1);            // для первых трех опросов всех датчкиков
		   }
		/* *************************************** */
					 		 
  }
  /* USER CODE END HoldingHandlerFunction */
}

/* USER CODE BEGIN Header_InputHandlerFunction */
/**
* @brief Function implementing the InputHandler thread.
* @param argument: Not used3
* @retval None
*/
/* USER CODE END Header_InputHandlerFunction */
void InputHandlerFunction(void const * argument)
{
  /* USER CODE BEGIN InputHandlerFunction */
	
	volatile uint16_t timeCounter = 0;
  /* Infinite loop */
  for(;;)
  {			
	 	if(SensorInfo.count && !flagDisplayLogsBusy)   
			{ 
				
				if(setErrorStatusFlag) // отправить на дисплей успешность записи паhаметров и калибровки
				{
			    setErrorStatusFlag = false;
					setErrorStatus(checkParamsValue);
				}
				else
				{		
				   // обновление конценатрации, если датчики есть
		         UpdateNextionDisplayWithChannelData(SensorInfo.count);
				}
		  }
		
     timeCounter++;
     if(timeCounter >= 120 )  // это примерно раз в одну минуту 
		 {	 
			 timeCounter = 0;
   		 UpdateDisplayTime();
		 }
    osDelay(50);
  }
  /* USER CODE END InputHandlerFunction */
}

/* USER CODE BEGIN Header_SlaveEventFunction */
/**
* @brief Function implementing the SlaveEventTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SlaveEventFunction */
void SlaveEventFunction(void const * argument)
{
  /* USER CODE BEGIN SlaveEventFunction */
  /* Infinite loop */
  for(;;)
  {
    ModBusSlaveEventHoldingRegHandler();
    osDelay(4);
  }
  /* USER CODE END SlaveEventFunction */
}

/* USER CODE BEGIN Header_DisplayTaskFunction */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DisplayTaskFunction */
void DisplayTaskFunction(void const * argument)
{
  /* USER CODE BEGIN DisplayTaskFunction */
	uint8_t InputByte = 0x00;
  /* Infinite loop */
  for(;;)
  {		
		if(Uart_Get_Byte(&ring_Rx, (uint8_t *)&InputByte) == RX_BUF_DONE)
		{
		  GetDisplayCmd(InputByte);
		}
		
		if(packet_ready)
		{
	   HandleDisplayCommands((uint8_t *)&displayResponse, (uint8_t *)&arrDisplayRX[0], (uint8_t *)&packet_ready);	
    }
    osDelay(20);  // 500 ms
  }
  /* USER CODE END DisplayTaskFunction */
}

/* USER CODE BEGIN Header_SendToDispTaskFunction */
/**
* @brief Function implementing the SendToDispTask thread.
* @param argument: Not used
* @retval None
*/
  
/* USER CODE END Header_SendToDispTaskFunction */
void SendToDispTaskFunction(void const * argument)
{
  /* USER CODE BEGIN SendToDispTaskFunction */

	  int first_line = 0;
		int last_line = 0; 
	  char log_string[64];
	  uint8_t line_count = 0;
  	SensorLogEvent_t LogMsg = {
	   .logType = SENSOR_LOG_TYPE_ERROR,
     .sensorID = 0,
     .deviceAddr = 0,
     .Value = 0
 };

	osDelay(10000);  // 500 ms
  
 /* Отравить в дисплей текущую дату */
 
 
  /* Infinite loop */
  for(;;)
  { 	
 	      if(!RdyWrittingFlag && sd_card_present)
				{
					 /* постоянно ждем новое сообщение */
				  if(xQueueReceive(queueSendLogsHandle,&LogMsg,osWaitForever) == pdTRUE){ 
					
					 RdyWrittingFlag = 1;
					
				    switch ((uint8_t)LogMsg.logType)
						{
							case SERVICE:	
								ServiceDataCallback(LogMsg.sensorID, LogMsg.Value,SERVICE);
								break;
							case CALIBRATION_0:
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value, CALIBRATION_0);
								break;
							case CALIBRATION_1:
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value,CALIBRATION_1);
								break;
							case ERROR_485:
							  	HAL_GPIO_WritePin(RY_GPIO_Port, RY3_Pin, GPIO_PIN_SET);
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value,ERROR_485);
								break;
							case THRESHOLD_WARNING:
								
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value,THRESHOLD_WARNING);
									break;
	            case THRESHOLD_ALARM:
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value, THRESHOLD_ALARM);
									break;
	            case THRESHOLD_ADDITIONAL:
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value, THRESHOLD_ADDITIONAL);
									break;	
							case OVER_THRESHOLD_WARNING:
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value, OVER_THRESHOLD_WARNING);	
									break;
	            case OVER_THRESHOLD_ALARM:
							    ServiceDataCallback(LogMsg.sensorID,LogMsg.Value, OVER_THRESHOLD_ALARM);
									break;
	            case OVER_THRESHOLD_ADDITIONAL:
								
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value, OVER_THRESHOLD_ADDITIONAL);
									break;
							 case NORMAL_LEVEL:
									ServiceDataCallback(LogMsg.sensorID,LogMsg.Value, NORMAL_LEVEL);
									break;
							
							case REQUEST_LOGS:
								   osDelay(200);
									 if(LogMsg.Value == 0x00)
									 {		 
											 line_count = GetServiceLinesCount();
											 flagDisplayLogsBusy = 1;
											 osDelay(900);
										    if (line_count <= 10 )
											  	  {
												  	 first_line = line_count;
											 			 last_line = 1;
											  		}													
									      else
												  {
														first_line = line_count;
														last_line = line_count - 10;
													}
												for(int i = first_line; i >= last_line; i--) // читаем первую 10 строку 
												 { 
													 osDelay(5);
													 FRESULT res = ReadServiceLine((char *)log_string, 64, i);
													 osDelay(5);
														if (res == FR_OK)
														 { /*first_line
															    вывод в обратном порядке: 
															      10 строка --> будет 1
															      1 строка --> будет 10
															 */
															 int display_line = first_line - i + 1;
															 SendNextionCommand("t%d.txt=\"%s\"", display_line,(const char* )log_string);
														 }
												 }
								     }
									 
										else if (LogMsg.Value > 0x09) // 10, 20 ,30 ....
											{	 
												 line_count = GetServiceLinesCount();
												 flagDisplayLogsBusy = 1;
												 osDelay(900);
												
												 // Проверяем, что запрошенное количество строк ИМЕННО равно 10, 20, 30 и т.д.
                          // и что общее количество строк >= запрошенному количеству
                         if((LogMsg.Value % 10 == 0) && (line_count >= LogMsg.Value))
                          {
                             first_line = line_count - LogMsg.Value;
												
														 
														  if (first_line < 0 )
															 {
															   first_line = 11;
															 }	
															   /*Определяем целевую границу для уменьшения
                                    Например: для значения 20 целевая граница = 10
                                   для значения 30 целевая граница = 20
															 */ 
                              int target_line = first_line - 10;
														  if (target_line < 0)
															 {
															   target_line = 1;
															 }
														  // Проверяем, что текущее количество строк превышает целевую границу
                               if(line_count > target_line)
                                  {
																		  //int display_line = 1; // начинаем с позиции 1
																		     for(int i = first_line; i >= target_line; i--)
                                              {
																						   osDelay(2);
													                     FRESULT res = ReadServiceLine((char *)log_string, sizeof(log_string), i);
													                     osDelay(2);
														                   if (res == FR_OK)
														                     {
																									 int display_line = first_line - i + 1;
															                      SendNextionCommand("t%d.txt=\"%s\"", display_line,(const char* )log_string);
														                     }
              
                                              }
                                   }
													  }	
		                        else
														{
														
														
														}
			
											}
										osDelay(250);
										flagDisplayLogsBusy = 0;
									  RdyWrittingFlag = 0;
							   break;
						 }							
	        }
			  
				}
		 osDelay(50);
  }
  /* USER CODE END SendToDispTaskFunction */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


void Send_Modbus_Command_DMA(uint8_t slave_addr , uint8_t data)

 {
    static uint8_t tx[8];     // ДОЛЖЕН быть static, чтобы буфер не исчез до завершения DMA!
    uint16_t crc;

    // --- Формируем Modbus RTU пакет ---
    tx[0] = slave_addr;   // Адрес устройства
    tx[1] = 0x06;         // Write Single Register
    tx[2] = 0x00;         // Адрес регистра Hi
    tx[3] = 0x0A;         // Адрес регистра Lo
    tx[4] = data;         // Значение Hivoid Send_Modbus_Command_DMA(uint8_t slave_addr)
    tx[5] = data;         // Значение Lo

    crc = Modbus_CRC16(tx, 6); // CRC по первым 6 байтам
    tx[6] = crc & 0xFF;        // CRC Lo
    tx[7] = (crc >> 8) & 0xFF; // CRC Hi

    // --- Отправка по DMA ---
    HAL_UART_Transmit_DMA(&huart4, tx, 8);
 }
uint16_t Modbus_CRC16(uint8_t *buf, uint8_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint8_t pos = 0; pos < len; pos++)
    {
        crc ^= buf[pos];

        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}


void MasterModbus2TaskFunction(void const * argument)
{
  /* USER CODE BEGIN MasterModbusTaskFunction */
  /* Infinite loop */
	uint8_t txt[] = "UART4 DMA send OK\r\n";
	for(;;)
  {
		// HAL_UART_Transmit_DMA(&huart4, txt, sizeof(txt)-1);
    RS485_RD_HIGH_MASTER2; 
    Send_Modbus_Command_DMA(0x01 , 0xff);
		
    osDelay(2000);
		RS485_RD_HIGH_MASTER2; 
		Send_Modbus_Command_DMA(0x01 , 0x00);
		
    osDelay(2000);
  }
  /* USER CODE END MasterModbusTaskFunction */
}





/* USER CODE END Application */
