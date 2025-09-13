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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t SelectaFlag = 0;
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

uint8_t SelectRunFlag = 0;
uint8_t SelectRunFlag1 = 0;
 
uint8_t ControlCycleFlag = 0; 
// Глобальная очередь для команд
QueueHandle_t displayCommandQueue = NULL;

volatile uint8_t CmdIsReady = 0;
volatile uint8_t CmdWriteIsReady = 0;
volatile uint8_t PauseTaskCounter = 0;
//volatile uint8_t PauseAfterTaskCounter = 100;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define mutexON  1 
#define mutexOFF 0 
volatile uint8_t additMutexMbFunctions = mutexOFF;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
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
uint32_t InputHandlerBuffer[ 256 ];
osStaticThreadDef_t InputHandlerControlBlock;
osThreadId SlaveEventTaskHandle;
uint32_t SlaveEventTaskBuffer[ 256 ];
osStaticThreadDef_t SlaveEventTaskControlBlock;
osThreadId DisplayTaskHandle;
uint32_t DisplayTaskBuffer[ 512 ];
osStaticThreadDef_t DisplayTaskControlBlock;
osThreadId SendToDispTaskHandle;
//uint32_t SendToDispTaskBuffer[ 256 ];
//osStaticThreadDef_t SendToDispTaskControlBlock;
osMutexId myMutex01Handle;
osStaticMutexDef_t myMutex01ControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */


void wait_for_modbus_response(uint32_t timeout_ms)
{
    uint32_t start_time = osKernelSysTick();
    
    while ((osKernelSysTick() - start_time) < timeout_ms) {
        //eMBMasterPoll(); // Обрабатываем Modbus стек
        
        // Проверяем, завершился ли запрос
        if (eMBMasterGetErrorType() != EV_ERROR_EXECUTE_FUNCTION) {
            break; // Запрос завершён
        }
        
        osDelay(1);
    }
    
    // Небольшая пауза между запросами
    osDelay(1);
}





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
  osThreadStaticDef(SlaveModbusTask, SlaveModbusTaskFunction, osPriorityBelowNormal, 0, 256, defaultTaskBuffer, &defaultTaskControlBlock);
  SlaveModbusTaskHandle = osThreadCreate(osThread(SlaveModbusTask), NULL);

  /* definition and creation of MasterModbusTas */
  osThreadStaticDef(MasterModbusTas, MasterModbusTaskFunction, osPriorityAboveNormal, 0, 512, MasterModbusTasBuffer, &MasterModbusTasControlBlock);
  MasterModbusTasHandle = osThreadCreate(osThread(MasterModbusTas), NULL);

  /* definition and creation of HoldingHandler */
  osThreadStaticDef(HoldingHandler, HoldingHandlerFunction, osPriorityAboveNormal, 0, 1024, HoldingHandlerBuffer, &HoldingHandlerControlBlock);
  HoldingHandlerHandle = osThreadCreate(osThread(HoldingHandler), NULL);

  /* definition and creation of InputHandler */
  osThreadStaticDef(InputHandler, InputHandlerFunction, osPriorityBelowNormal, 0, 256, InputHandlerBuffer, &InputHandlerControlBlock);
  InputHandlerHandle = osThreadCreate(osThread(InputHandler), NULL);

  /* definition and creation of SlaveEventTask */
  osThreadStaticDef(SlaveEventTask, SlaveEventFunction, osPriorityBelowNormal, 0, 256, SlaveEventTaskBuffer, &SlaveEventTaskControlBlock);
  SlaveEventTaskHandle = osThreadCreate(osThread(SlaveEventTask), NULL);

  /* definition and creation of DisplayTask */
  osThreadStaticDef(DisplayTask, DisplayTaskFunction, osPriorityBelowNormal, 0, 512, DisplayTaskBuffer, &DisplayTaskControlBlock);
  DisplayTaskHandle = osThreadCreate(osThread(DisplayTask), NULL);

  /* definition and creation of SendToDispTask */
  //osThreadStaticDef(SendToDispTask, SendToDispTaskFunction, osPriorityRealtime, 0, 256, SendToDispTaskBuffer, &SendToDispTaskControlBlock);
  //SendToDispTaskHandle = osThreadCreate(osThread(SendToDispTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	
	displayCommandQueue = xQueueCreate(20, sizeof(DisplayCommand_t));
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
    osDelay(2);
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
DisplayCommand_t displayCmd;

void HoldingHandlerFunction(void const * argument)
{
  /* USER CODE BEGIN HoldingHandlerFunction */
		
	static uint8_t HoldingPollsDone = 0;  // Счётчик выполненных опросов Holding-регистров
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
				    if(SelectRunFlag == 0)
				     { // вычитывается модель прибора   
					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, DEVICE_MODEL_CODE - 1,2, 200 );
					
							 SelectRunFlag = 1;
				     }
				    else if(SelectRunFlag == 1)
				     { // вычитываются значения дипазона  всей шкалы и единицы измерения
					     eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SCALE_MAX_HIGH - 1 ,3, 200 );
					     
							 SelectRunFlag = 2;
				     }				 
				    else if (SelectRunFlag == 2)
					   { //вычитываются пороги 1, 2 и 3
 					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_THRESHOLD_WARNIGN_HIGN - 1, 6, 200 );
	           
					    SelectRunFlag = 3;
					   }
						 
				    else if (SelectRunFlag == 3)
					    { //вычитывается тип газа
					     eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SUBSTANCE_CODE_1 - 1, 16, 200 );
					    
							 SelectRunFlag = 4;	
					    } 	 
			      /* ********************************* Handling INPUT registers *************************** */
				     else if (SelectRunFlag == 4)
					    {
					     eMBMasterReqReadInputRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_PRIMARY_VALUE_HIGH  - 1, 3, 200 );
               				    
							 SelectRunFlag = 5;
								
								if( ControlCycleFlag)
								 {
									 SelectRunFlag = 5;
									 
								 }
					    } 	
				    /* ********************************* set next slave addr *************************** */	
				     else if (SelectRunFlag == 5)
				      {
								if(ControlCycleFlag)
								{
								  readCurrentSensorState(ModBusSlaveCurrentDeviceAddr,usMRegInBuf,usMRegHoldBuf);
								  ControlCycleFlag = 0;
									HoldingPollsDone = 3;
									SelectRunFlag = 6;
									checkParamsValue = false;
								  checkParamsValue	= compareParams((SensorCurrentState_t *)&writeParams,(SensorCurrentState_t *)&readParams);
								  setErrorStatus(checkParamsValue);
								}
								
							else{
								
					         readCurrentSensorState(ModBusSlaveCurrentDeviceAddr,usMRegInBuf,usMRegHoldBuf);
				           setNextDeviceAddr(&ModBusSlaveCurrentDeviceAddr);	       // set next device addr
                   SelectRunFlag = 0;
								
			
					         if(ModBusSlaveCurrentDeviceAddr == NUMBER_SLAVE_DEVICES)
					          {
							       HoldingPollsDone++; // следующий	 круг из трех, при старте девайса
								     ModBusSlaveCurrentDeviceAddr = 1; 
											
										 updateProgressBar(HoldingPollsDone);
											 
							      }
					         if(HoldingPollsDone == 3) // после трех проходов определяем наличие активных приборов
					          {
									    SendNextionCommand ("page page%d", 0);
						          SelectRunFlag = 6;	               // переход на постояный цикл опроса значений концентрации
									
									   /* получение информации об активных датчиках их адресах */
									   GetActiveSensors(SensorStateArray, (SensorInfo_t *) &SensorInfo);
									   /* получение modbus адреса  первого активного датчика на линии */
								     ModBusSlaveCurrentDeviceAddr = SensorInfo.modbusAddrs[0];  
                    /* вывести окна активных дачтичиков  и перейти на постоянный опрос */
								   }				
				        }	
						 }								
			     }	

           else if (SelectRunFlag == 21)
					   { 
 					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, CALIBRATION_PROCESS_STATUS - 1, 1, 200 );
	          	
							 osMutexRelease(myMutex01Handle);
               osDelay(400);
               osMutexWait(myMutex01Handle, 10);
					    SelectRunFlag = 22;
					   }
					else if (SelectRunFlag == 22)
					   {  
							checkParamsValue = false; 
							readCurrentCalibrationState (ModBusSlaveCurrentDeviceAddr,usMRegHoldBuf);
							checkParamsValue = getCalibrationProcessState (ModBusSlaveCurrentDeviceAddr);	
							setErrorStatus(!checkParamsValue);
					    SelectRunFlag = 6;
					   }	 
						 
			      /* постоянный цикл опроса активных приборов */
				
			        /* ********************************* Handling INPUT registers *************************** */
				  else if (SelectRunFlag == 6)
				  	    {
								 /*  отправка  запроса на считывания значение текущей концентрации */
								  /* !!!!! на период настройки параметроы с дисплея  отключается запрос концентрации !!!!! */ 
									
                   SelectRunFlag = 7;
									
									if(!CmdIsReady){ 
					           eMBMasterReqReadInputRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_PRIMARY_VALUE_HIGH  - 1, 3, 200 );
									
									 }
									else {
									   SelectRunFlag = 8;
									}
					       
					      } 		
				      /* ********************************* set next slave addr *************************** */	
				   else if (SelectRunFlag == 7)
				        {
									 SelectRunFlag = 8;
									
									 if(!CmdIsReady){ 
						    	   /* значение концентрации текущее */
					           readCurrentSensorValue(ModBusSlaveCurrentDeviceAddr,usMRegInBuf);
									
					            /* выбираем только адреса активных приборов */
				            setNextActiveDeviceAddr_(&ModBusSlaveCurrentDeviceAddr,SensorInfo.count);	       // set next active sdevice addr
									} 
                  		    	
				       }									 
		else if (SelectRunFlag == 8)	
		{		
      uint8_t shouldChangeFlag = 1;	 // для обычного цикла, когда команды не прилетают

			/* *************** ОБРАБОТКА КОМАНД ОТ ДИСПЛЕЯ *************** */
    // Проверяем, есть ли команды от дисплея в очереди
	 if(CmdIsReady)
	 { 

    if(xQueueReceive(displayCommandQueue, &displayCmd, 0) == pdTRUE)
    {  	                                       
			    if(displayCmd.command ==DISPLAY_SCALE_DIMENSION){	 
						
						 registersTX[0] = displayCmd.binary32 & 0xFFFF;
              //registersTX[1] = displayCmd.binary32 & 0xFFFF;
						
					   osMutexRelease(myMutex01Handle);
             osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
             osMutexWait(myMutex01Handle, 10);
				    		 
						 eMBMasterReqWriteMultipleHoldingRegister(  displayCmd.deviceAddr, 
						                                             SENSOR_SCALE_DIMENSTION - 1, 
						                                             1, 
						                                             (USHORT *)&registersTX[0], 
						                                             300); 
											
						osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_PACKET);
            osMutexWait(myMutex01Handle, 10);
						
						shouldChangeFlag = 0;
				    SelectRunFlag = 8;
					 }
			
				/* ******************  DISPLAY_SCALE_MAX *************************** */	
			 else		if(displayCmd.command ==DISPLAY_SCALE_MAX){		
				 
				   registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
           registersTX[1] = displayCmd.binary32 & 0xFFFF;
 
           osMutexRelease(myMutex01Handle);
           osDelay(TIME_DELAY_PACKET);
           osMutexWait(myMutex01Handle, 10);
					 	
           eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr, 
				                                            SENSOR_SCALE_MAX_HIGH - 1, 
				                                            2, 
				                                            (USHORT *)&registersTX[0], 
				                                            300); 

           osMutexRelease(myMutex01Handle);
           osDelay(TIME_DELAY_PACKET);
           osMutexWait(myMutex01Handle, 10);

				   shouldChangeFlag = 0;
				   SelectRunFlag = 8; 
		    }
				/* ******************  DISPLAY_CALIBRATION_PRIMARY_ZERO ********************** */	  
				else	if(displayCmd.command == DISPLAY_CALIBRATION_PRIMARY_ZERO){
					   
					registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
          registersTX[1] = displayCmd.binary32 & 0xFFFF;
					
					osMutexRelease(myMutex01Handle);
          osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
          osMutexWait(myMutex01Handle, 10);
					
          eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr, 
					                                             CALIBRATION_PRIMARY_ZERO_VALUE_HIGH - 1, 
					                                             2, 
					                                             (USHORT *)&registersTX[0], 
					                                             300);	 

          osMutexRelease(myMutex01Handle);
          osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
          osMutexWait(myMutex01Handle, 10);

			    shouldChangeFlag = 0;
			    
					
				
			      CmdWriteIsReady = 1;
					
					 /* пройти один цикл опроса состояния калибровки  */
				    SelectRunFlag = 21;
						ModBusSlaveCurrentDeviceAddr = displayCmd.deviceAddr;	
	     }
        /* ******************  DISPLAY_CALIBRATION_POINT_1 *************************** */	  
	  else   if(displayCmd.command ==DISPLAY_CALIBRATION_POINT_1){	
			      registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
            registersTX[1] = displayCmd.binary32 & 0xFFFF;
			
			      osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
            osMutexWait(myMutex01Handle, 10);
			
				    
            eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr, 
			                                               CALIBRATION_PRIMARY_SPAN_VALUE_HIGH - 1, 
			                                               2, 
			                                               (USHORT *)&registersTX[0], 
			                                               300);
			
			
            osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
            osMutexWait(myMutex01Handle, 10);

			      shouldChangeFlag = 0;
			      CmdWriteIsReady = 1;   
						
						/* пройти один цикл опроса состояния калибровки  */
				    SelectRunFlag = 21;
						ModBusSlaveCurrentDeviceAddr = displayCmd.deviceAddr;
		    }  
         /* ******************  DISPLAY_THRESHOLD_WARNING************************ */	 
			 else	 if(displayCmd.command == DISPLAY_THRESHOLD_WARNING){
				 
             registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
             registersTX[1] = displayCmd.binary32 & 0xFFFF;
				 
				 
				     osMutexRelease(myMutex01Handle);
             osDelay(TIME_DELAY_PACKET);
             osMutexWait(myMutex01Handle, 10);
				 

             eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr, 
				                                              SENSOR_THRESHOLD_WARNIGN_HIGN - 1, 
				                                              2, 
				                                              (USHORT *)&registersTX[0], 
				                                              300);
				 
				 
				     osMutexRelease(myMutex01Handle);
             osDelay(TIME_DELAY_PACKET);
             osMutexWait(myMutex01Handle, 10);
				   
             shouldChangeFlag = 0;
				     SelectRunFlag = 8;
	      }
          /* ******************  DISPLAY_THRESHOLD_ALARM ************************ */	  
       else if(displayCmd.command == DISPLAY_THRESHOLD_ALARM){
				 
				    osMutexRelease(myMutex01Handle);
             osDelay(TIME_DELAY_PACKET);
            osMutexWait(myMutex01Handle, 10);
				 
            registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
            registersTX[1] = displayCmd.binary32 & 0xFFFF;	
				 
				 
            eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr,
				                                             SENSOR_THRESHOLD_ALARM_HIGH - 1, 
				                                             2, 
				                                             (USHORT *)&registersTX[0], 
			                                               300);	

            osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_PACKET);
            osMutexWait(myMutex01Handle, 10);

				 
				    shouldChangeFlag = 0;
				    SelectRunFlag = 8;
        }
				/* ******************  DISPLAY_THRESHOLD_ADDITIONAL ************************ */	 
			 else	if(displayCmd.command == DISPLAY_THRESHOLD_ADDITIONAL){
				    registersTX[0] = (displayCmd.binary32 >> 16) & 0xFFFF;
            registersTX[1] = displayCmd.binary32 & 0xFFFF;	
				 
				    osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_PACKET);
            osMutexWait(myMutex01Handle, 10);
				 
				 
            eMBMasterReqWriteMultipleHoldingRegister(displayCmd.deviceAddr,
				                                             SENSOR_THRESHOLD_ADDITIONAL_HIGH - 1, 
				                                             2, 
				                                             (USHORT *)&registersTX[0], 
				                                             300); 
				 
				    osMutexRelease(myMutex01Handle);
            osDelay(TIME_DELAY_BEFORE_AFTER_CMD);
            osMutexWait(myMutex01Handle, 10);
				 
				 
				    shouldChangeFlag = 0;
				    CmdWriteIsReady = 1;
				   /* Для вычичитки записанных данных  пройти один цикл опроса */
				    SelectRunFlag = 0;
						ControlCycleFlag = 1;
						HoldingPollsDone = 0;
						ModBusSlaveCurrentDeviceAddr = displayCmd.deviceAddr;
				 	 /* ******************************************************** */
				    
        }
      }	
		}
	   if(shouldChangeFlag)
		   {	    
		    SelectRunFlag = 6;
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
  /* Infinite loop */
  for(;;)
  {			
	 	if(SensorInfo.count)   
			{ 
				// обновление конценатрации, если датчики есть
		    UpdateNextionDisplayWithChannelData(SensorInfo.count);
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
		
		//osDelay(100); // 3 минуты  после нажатия кнопки пауза 3 сек (+/- 0,5)
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
	 osStatus status; 
  /* Infinite loop */
  for(;;)
  { 	
	 osDelay(5);    
  }
  /* USER CODE END SendToDispTaskFunction */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
