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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t SelectaFlag = 0;
extern volatile uint8_t startDisplayFlag;
extern volatile uint8_t packet_ready;   
extern UART_HandleTypeDef huart3;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern USHORT   usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];
extern SensorState_t   SensorStateArray[NUMBER_SLAVE_DEVICES];

extern volatile char arrDisplayRX[ARRAY_RX_SIZE];
extern volatile uint8_t displayResponse;
extern volatile uint8_t displayStartedFlag ;

uint8_t  numberOfDevices = 20;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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
uint32_t HoldingHandlerBuffer[ 256 ];
osStaticThreadDef_t HoldingHandlerControlBlock;
osThreadId InputHandlerHandle;
uint32_t InputHandlerBuffer[ 256 ];
osStaticThreadDef_t InputHandlerControlBlock;
osThreadId SlaveEventTaskHandle;
uint32_t SlaveEventTaskBuffer[ 256 ];
osStaticThreadDef_t SlaveEventTaskControlBlock;
osThreadId DisplayTaskHandle;
uint32_t DisplayTaskBuffer[ 256 ];
osStaticThreadDef_t DisplayTaskControlBlock;
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
  osThreadStaticDef(MasterModbusTas, MasterModbusTaskFunction, osPriorityNormal, 0, 256, MasterModbusTasBuffer, &MasterModbusTasControlBlock);
  MasterModbusTasHandle = osThreadCreate(osThread(MasterModbusTas), NULL);

  /* definition and creation of HoldingHandler */
  osThreadStaticDef(HoldingHandler, HoldingHandlerFunction, osPriorityNormal, 0, 256, HoldingHandlerBuffer, &HoldingHandlerControlBlock);
  HoldingHandlerHandle = osThreadCreate(osThread(HoldingHandler), NULL);

  /* definition and creation of InputHandler */
  //osThreadStaticDef(InputHandler, InputHandlerFunction, osPriorityBelowNormal, 0, 256, InputHandlerBuffer, &InputHandlerControlBlock);
  //InputHandlerHandle = osThreadCreate(osThread(InputHandler), NULL);

  /* definition and creation of SlaveEventTask */
  osThreadStaticDef(SlaveEventTask, SlaveEventFunction, osPriorityBelowNormal, 0, 256, SlaveEventTaskBuffer, &SlaveEventTaskControlBlock);
  SlaveEventTaskHandle = osThreadCreate(osThread(SlaveEventTask), NULL);

  /* definition and creation of DisplayTask */
  osThreadStaticDef(DisplayTask, DisplayTaskFunction, osPriorityBelowNormal, 0, 256, DisplayTaskBuffer, &DisplayTaskControlBlock);
  DisplayTaskHandle = osThreadCreate(osThread(DisplayTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
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
		taskYIELD();
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
		// Пытаемся захватить мьютекс (ждём 10 мс)
     osStatus status = osMutexWait(myMutex01Handle, 100);
		 if (status == osOK) {		 
	   
		     eMBMasterPoll();
			   eMBMasterPoll();
			  // Освобождаем мьютекс
       osMutexRelease(myMutex01Handle);
		 }
		 
    osDelay(10);
		taskYIELD();
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
	
	static uint8_t SelectRunFlag = 0;
	static uint8_t HoldingPollsDone = 0;  // Счётчик выполненных опросов Holding-регистров
  
	osDelay(timeStep/2);
	/* Infinite loop */
  for(;;)
  {	// Пытаемся захватить мьютекс (ждём 50 мс)
     osStatus status = osMutexWait(myMutex01Handle, 100);
		 if (status == osOK) {
			  
			   if(HoldingPollsDone  < 3) // опрашиваем первые ТРИ РАЗА все регистры 
				 {	 
			
			     /* *********************************  Handling HOLDING registers *************************** */
				    if(SelectRunFlag == 0)
				     {   
					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, (DEVICE_MODEL_CODE - 1),2, 200 );
					    SelectRunFlag = 1;
				     }
				    else if(SelectRunFlag == 1)
				     { 	 
					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SCALE_MAX_HIGH - 1 ,3, 200 );
					    SelectRunFlag = 2;
				     }				 
				    else if (SelectRunFlag == 2)
					   {
					    eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_THRESHOLD_WARNIGN_HIGN - 1, 4, 200 );
					    SelectRunFlag = 3;
					   }
				    else if (SelectRunFlag == 3)
					    {
					     eMBMasterReqReadHoldingRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_SUBSTANCE_CODE_1 - 1, 16, 200 );
					     SelectRunFlag = 4;	
					    } 	 
			      /* ********************************* Handling INPUT registers *************************** */
				     else if (SelectRunFlag == 4)
					    {
					     eMBMasterReqReadInputRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_PRIMARY_VALUE_HIGH  - 1, 3, 200 );
					     SelectRunFlag = 5;
					    } 	
				    /* ********************************* set next slave addr *************************** */	
				     else if (SelectRunFlag == 5)
				      {
					     readCurrentSensorState(ModBusSlaveCurrentDeviceAddr,usMRegInBuf);
				       setNextDeviceAddr(&ModBusSlaveCurrentDeviceAddr);	       // set next device addr
               SelectRunFlag = 0;
								
								
					     if(ModBusSlaveCurrentDeviceAddr == NUMBER_SLAVE_DEVICES)
					     {
							  HoldingPollsDone++;
								ModBusSlaveCurrentDeviceAddr = 1; 
							 }
		 
					     if(HoldingPollsDone == 3) // после трех проходов опредеояем наличие активных приборов
					      {
						      SelectRunFlag = 4;	
								  ModBusSlaveCurrentDeviceAddr = 1;
						     }
				       }					
			   }		  
			 else   /* постоянный цикл опроса активных приборов */
				 {
			        /* ********************************* Handling INPUT registers *************************** */
				       if (SelectRunFlag == 4)
				  	    {
								      	/* отправка  запроса на считываение регистров  */
					       eMBMasterReqReadInputRegister( ModBusSlaveCurrentDeviceAddr, SENSOR_PRIMARY_VALUE_HIGH  - 1, 3, 200 );
					       SelectRunFlag = 5;
					      } 		
				      /* ********************************* set next slave addr *************************** */	
				       else if (SelectRunFlag == 5)
				        {
						    	/* считиваем регистры после запроса */
					        readCurrentSensorState(ModBusSlaveCurrentDeviceAddr,usMRegInBuf);
									
					        /* выбираем только адреса активных приборов */
				          setNextActiveDeviceAddr(&ModBusSlaveCurrentDeviceAddr);	       // set next device addr
                  SelectRunFlag = 4;
				   }	
			   }
			//Освобождаем мьютекс
       osMutexRelease(myMutex01Handle);
		 }
		  
	 /* ************* osDelay()****************** */
		  if(HoldingPollsDone == 3) 
		   {
				 // для постоянного опроса, делим timestep на два, так как попадаем в кейс отправки команды каждый второй раз 
        osDelay((timeStep + 10)/2);   //для соответствия реальному и вводимомоу. 
		   } 
		  else
		   {
		   osDelay(TIME_DEFAULT_1 + 10);       // для первых трех опросов всех датчкиков
		   }
		/* *************************************** */
		taskYIELD();
  }
  /* USER CODE END HoldingHandlerFunction */
}

/* USER CODE BEGIN Header_InputHandlerFunction */
/**
* @brief Function implementing the InputHandler thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_InputHandlerFunction */
//void InputHandlerFunction(void const * argument)
//{
//  /* USER CODE BEGIN InputHandlerFunction */
//  /* Infinite loop */
//  for(;;)
//  {
//		ModBusSlaveEventHoldingRegHandler();
//    osDelay(4);
//		taskYIELD()
//  }
  /* USER CODE END InputHandlerFunction */
//}

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
		taskYIELD()
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
  /* Infinite loop */
  for(;;)
  {
			if(packet_ready)	
        {
				   packet_ready = 0;
           memset((void*)arrDisplayRX, 0, ARRAY_RX_SIZE);
           switch(displayResponse)
					 {	 
						 case 0x00:
							 
						 break;
						 case 0x88:   // первый ответ после старта дисплея  0x88 0xFF 0xFF 0xFF
							 displayStartedFlag++;
						 
							 SendNextionCommand("Init.qDev.txt=\"%d\"", numberOfDevices); // for testing  
						   osDelay(5);	
						 break;
						 
						 case 0x10:	 // второй ответ после старта дисплея   0x10 0xFF 0xFF 0xFF
							 
						   displayStartedFlag++;
						   InitNextionDisplayWithDeviceData(numberOfDevices);
						 break;	 	
					 }						 
        // Перезапускаем приём
           HAL_UART_Receive_IT(&huart3,(uint8_t *)&arrDisplayRX[0], 1);					
				}				
		
    osDelay(50);
		taskYIELD()
  }
  /* USER CODE END DisplayTaskFunction */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
