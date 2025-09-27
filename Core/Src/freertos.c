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
 
  /* Глобальная очередь для команд */
QueueHandle_t displayCommandQueue = NULL;

volatile uint8_t CmdIsReady = 0;
volatile uint8_t CmdWriteIsReady = 0;
volatile uint8_t PauseTaskCounter = 0;
 
  /* Глобальная очередь для логов */
QueueHandle_t SD_CardMsgQueue = NULL;
SensorLog_t sensorLog = {0}; 
SensorLog_t ReceivedSensorLog = {0};

volatile uint8_t RdyWrittingFlag = 0;


 
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
extern uint8_t ModBusSlaveDefaultDeviceAddr;
extern uint8_t ModBusSlaveCurrentDeviceAddr;
/* USER CODE END Variables */
osThreadId SlaveModbusTaskHandle;
uint32_t defaultTaskBuffer[ 256 ];
osStaticThreadDef_t defaultTaskControlBlock;
osThreadId MasterModbusTasHandle;
uint32_t MasterModbusTasBuffer[ 512 ];
osStaticThreadDef_t MasterModbusTasControlBlock;
osThreadId HoldingHandlerHandle;
uint32_t HoldingHandlerBuffer[ 512 ];
osStaticThreadDef_t HoldingHandlerControlBlock;
osThreadId InputHandlerHandle;
uint32_t InputHandlerBuffer[ 128 ];
osStaticThreadDef_t InputHandlerControlBlock;
osThreadId SlaveEventTaskHandle;
uint32_t SlaveEventTaskBuffer[ 128 ];
osStaticThreadDef_t SlaveEventTaskControlBlock;
osThreadId DisplayTaskHandle;
uint32_t DisplayTaskBuffer[ 128 ];
osStaticThreadDef_t DisplayTaskControlBlock;
osThreadId SendToDispTaskHandle;
uint32_t SendToDispTaskBuffer[ 1200];
osStaticThreadDef_t SendToDispTaskControlBlock;
osMutexId myMutex01Handle;
osStaticMutexDef_t myMutex01ControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

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
  //osThreadStaticDef(MasterModbusTas, MasterModbusTaskFunction, osPriorityAboveNormal, 0, 512, MasterModbusTasBuffer, &MasterModbusTasControlBlock);
  //MasterModbusTasHandle = osThreadCreate(osThread(MasterModbusTas), NULL);

  /* definition and creation of HoldingHandler */
  //osThreadStaticDef(HoldingHandler, HoldingHandlerFunction, osPriorityAboveNormal, 0, 512, HoldingHandlerBuffer, &HoldingHandlerControlBlock);
 // HoldingHandlerHandle = osThreadCreate(osThread(HoldingHandler), NULL);

  /* definition and creation of InputHandler */
  //osThreadStaticDef(InputHandler, InputHandlerFunction, osPriorityBelowNormal, 0, 256, InputHandlerBuffer, &InputHandlerControlBlock);
  //InputHandlerHandle = osThreadCreate(osThread(InputHandler), NULL);

  /* definition and creation of SlaveEventTask */
  //osThreadStaticDef(SlaveEventTask, SlaveEventFunction, osPriorityBelowNormal, 0, 256, SlaveEventTaskBuffer, &SlaveEventTaskControlBlock);
  //SlaveEventTaskHandle = osThreadCreate(osThread(SlaveEventTask), NULL);

  /* definition and creation of DisplayTask */
  //osThreadStaticDef(DisplayTask, DisplayTaskFunction, osPriorityBelowNormal, 0, 512, DisplayTaskBuffer, &DisplayTaskControlBlock);
 // DisplayTaskHandle = osThreadCreate(osThread(DisplayTask), NULL);

  /* definition and creation of SendToDispTask */
  osThreadStaticDef(SendToDispTask, SendToDispTaskFunction, osPriorityBelowNormal, 0, 1200 , SendToDispTaskBuffer, &SendToDispTaskControlBlock);
  SendToDispTaskHandle = osThreadCreate(osThread(SendToDispTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	
	displayCommandQueue = xQueueCreate(20, sizeof(DisplayCommand_t));
	SD_CardMsgQueue = xQueueCreate(20, sizeof(SensorLog_t));
	
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


void HoldingHandlerFunction(void const * argument)
{
  /* USER CODE BEGIN HoldingHandlerFunction */
	static uint8_t HoldingPollsDone = 0;  // Счётчик выполненных опросов Holding-регистров
  osDelay(10000);
	int i = 1;
	/* Infinite loop */
  for(;;)
  {	
	    	   sensorLog.sensorID = i;
		       sensorLog.Value = 25;
		        	
	         if(i  == 9)  i = 1;
		       i++;
		
	      if(xQueueSend(SD_CardMsgQueue, &sensorLog, 500) == pdPASS){	
					      					
         } else{}			 
					 
      vTaskDelay(pdMS_TO_TICKS(1000));			 		 
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
				
				if(setErrorStatusFlag) // отправить на дисплей успешность записи паhаметров т калибровки
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
    osDelay(200);
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
		int i = 1;
	osDelay(10000);  // 500 ms	
	
  /* Infinite loop */
  for(;;)
  { 	
 	      if(!RdyWrittingFlag)
				{
					 RdyWrittingFlag = 1;
				   ReceivedSensorLog.sensorID = i;
		       ReceivedSensorLog.Value = 25;
		        	
	         if(i  == 40)  i = 1;
		       i++;
				
			      SensorDataCallback(ReceivedSensorLog.sensorID, ReceivedSensorLog.Value);
				}
		 osDelay(500);  // 500 ms		
  }
  /* USER CODE END SendToDispTaskFunction */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
