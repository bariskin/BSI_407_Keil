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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t SelectaFlag = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern uint8_t ModBusSlaveDefaultDeviceAddr;
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
osMutexId myMutex01Handle;
osStaticMutexDef_t myMutex01ControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void SlaveModbusTaskFunction(void const * argument);
void MasterModbusTaskFunction(void const * argument);
void HoldingHandlerFunction(void const * argument);
void InputHandlerFunction(void const * argument);

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
  osThreadStaticDef(MasterModbusTas, MasterModbusTaskFunction, osPriorityNormal, 0, 256, MasterModbusTasBuffer, &MasterModbusTasControlBlock);
  MasterModbusTasHandle = osThreadCreate(osThread(MasterModbusTas), NULL);

  /* definition and creation of HoldingHandler */
  osThreadStaticDef(HoldingHandler, HoldingHandlerFunction, osPriorityNormal, 0, 256, HoldingHandlerBuffer, &HoldingHandlerControlBlock);
  HoldingHandlerHandle = osThreadCreate(osThread(HoldingHandler), NULL);

  /* definition and creation of InputHandler */
  //osThreadStaticDef(InputHandler, InputHandlerFunction, osPriorityNormal, 0, 256, InputHandlerBuffer, &InputHandlerControlBlock);
  //InputHandlerHandle = osThreadCreate(osThread(InputHandler), NULL);

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
    osDelay(3);
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
	
		  
			  // Освобождаем мьюте
       osMutexRelease(myMutex01Handle);
		 }
		 
		 else
		 {
		 
		 
		 }
    osDelay(40);
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
  /* Infinite loop */
  for(;;)
  {	// Пытаемся захватить мьютекс (ждём 10 мс)
     osStatus status = osMutexWait(myMutex01Handle, 50);
		 if (status == osOK) {
			 if(SelectRunFlag == 0)
			 {
			  eMBMasterReqReadHoldingRegister( ModBusSlaveDefaultDeviceAddr, 0, 100, 200 );
				SelectRunFlag = 1;
			 }
			else if(SelectRunFlag == 1)
			 { 
			  eMBMasterReqReadInputRegister( ModBusSlaveDefaultDeviceAddr, 0, 50, 200 );
				SelectRunFlag = 0;
			 }
			//Освобождаем мьютекс
       osMutexRelease(myMutex01Handle);
		 }
		 
		 else
		 {
		 
		 
		 } 
			 
    osDelay(250);
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
void InputHandlerFunction(void const * argument)
{
  /* USER CODE BEGIN InputHandlerFunction */
  /* Infinite loop */
  for(;;)
  {// Пытаемся захватить мьютекс (ждём 10 мс)
//     osStatus status = osMutexWait(myMutex01Handle, 50);
//		 if (status == osOK) {
//		  eMBMasterPoll();	
//			 eMBMasterReqReadInputRegister( ModBusSlaveDefaultDeviceAddr, 0, 20, 100 );
//			 eMBMasterPoll();
//				  // Освобождаем мьютекс
//       osMutexRelease(myMutex01Handle);
//		 }
//		 
//		 else
//		 {
//		 
//		 
//		 } 
			 
    osDelay(500);
  }
  /* USER CODE END InputHandlerFunction */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
