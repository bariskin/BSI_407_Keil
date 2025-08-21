/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DisplayDriver.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
extern volatile uint8_t startDisplayFlag;
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern  uint8_t rxIdxDisplayUart;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define CHECK_3_ZEROS(arr) (arr[0] == 0 && arr[1] == 0 && arr[2] == 0)
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim10;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim2;

/* USER CODE BEGIN EV */
                   // Максимальный размер буфера
#define PACKET_END_MARKER 0xFFFFF             // Маркер конца пакета (0xFF 0xFF 0xFF)

extern volatile uint8_t arrDisplayRX[ARRAY_RX_SIZE];   // Буфер приёма
volatile uint16_t rx_index = 0;               // Текущая позиция в буфере
volatile uint8_t packet_ready = 0;            // Флаг завершённого пакета
volatile uint32_t end_marker_counter = 0;     // Счётчик для детектирования 0xFF 0xFF 0xFF
volatile uint8_t displayResponse = 0;
volatile uint8_t displayStartedFlag = 0;

uint8_t tx_buffer[ARRAY_TX_SIZE + 3]; // Основной буфер + 3 байта маркера конца
volatile uint16_t tx_index = 0;
volatile uint16_t tx_size = 0;


 #define MAX_PACKETS 10         // Максимум сообщений в буфере
// uint8_t packet_buffer[MAX_PACKETS][ARRAY_RX_SIZE];
// uint8_t packet_lengths[MAX_PACKETS];
// uint8_t current_packet = 0;

 #define MAX_SIGNIFICANT_BYTES  10 // Максимум значимых байтов для сохранения
 
 typedef struct {
    uint8_t data[10];      // Полные данные сообщения
    uint8_t length;                   // Общая длина (без учёта FF FF FF)
    uint8_t significant_bytes[MAX_SIGNIFICANT_BYTES]; 
    uint8_t significant_count;        // Фактическое количество значимых байтов
} Packet;


 Packet received_packets[MAX_PACKETS];
 uint8_t packets_received = 0;
 uint8_t significant_bytes_count = 0;
 uint8_t significant_bytes[MAX_SIGNIFICANT_BYTES] = {0};
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim10);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */

void USART3_IRQHandler(void) {
    // Проверяем флаг прерывания по приёму данных (RXNE)
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE)) {
        uint8_t byte = (uint8_t)(huart3.Instance->DR & 0xFF);  // Читаем принятый байт

        // Если буфер не переполнен
        if (rx_index < ARRAY_RX_SIZE - 1) {
					
              arrDisplayRX[rx_index++] = byte;  // Сохраняем байт в буфер

            // Проверяем, является ли текущий байт частью маркера конца (0xFF)
            if (byte == 0xFF) {
                end_marker_counter++;  // Увеличиваем счётчик подряд идущих 0xFF
            } 
						else {
                end_marker_counter = 0; // Сброс, если байт не 0xFF
							
							  // Сохраняем первые 5 значимых байтов
                if (significant_bytes_count < MAX_SIGNIFICANT_BYTES) {
                   significant_bytes[significant_bytes_count++] = byte;
                  }	
            }
								
								// Обнаружение конца сообщения (3 0xFF подряд)
					if (end_marker_counter >= 3) {
							arrDisplayRX[rx_index - 3] = '\0';  // Удаляем маркер конца
							
							// Обновляем displayResponse
							if (significant_bytes_count > 0) {
									// Обработка специальных случаев
									switch(significant_bytes_count) {
											case 1:
													displayResponse = significant_bytes[0];
													break;
													
											case 2:
													displayResponse = (significant_bytes[0] == 0x00) 
																				 ? significant_bytes[1] 
																				 : significant_bytes[0];
													break;
													
											case 5:
													if (CHECK_3_ZEROS(significant_bytes)) {
															displayResponse = 0x33; // Количество устройств
													}
													break;
									}
		
									// Обработка командных пакетов (формат XX 01 YY)
									if (significant_bytes_count >= 3 && arrDisplayRX[1] == 0x01) {
											// Используем прямое соответствие между param_id и response
											if (arrDisplayRX[2] >= 0x01 && arrDisplayRX[2] <= 0x06) { // Baud Rate change
													displayResponse = arrDisplayRX[2];
											}
									}
									/// для Calibration Primary Zero 
									else if (significant_bytes_count >= 3 && arrDisplayRX[1] == (uint8_t)0x10 && arrDisplayRX[2] == (uint8_t)0x64)
									{
										displayResponse = 0x64; // Calibration Primary Zero 	
									}
														 
							}
							
							packet_ready = 1;  // Флаг готовности пакета
							rx_index = 0;
							end_marker_counter = 0;
							significant_bytes_count = 0;
					}
        } else {
            // Переполнение буфера — сбрасываем
            rx_index = 0;
            end_marker_counter = 0;
        }			
    }

		   // Обработка передачи
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TXE) && 
        (huart3.Instance->CR1 & USART_CR1_TXEIE)) {
        if (tx_index < tx_size) {
            huart3.Instance->DR = tx_buffer[tx_index++];
        } else {
            // Все данные переданы - выключаем прерывание передачи
            huart3.Instance->CR1 &= ~USART_CR1_TXEIE;
        }
    }	
    // Обработка ошибок UART (опционально)
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_ORE | UART_FLAG_FE | UART_FLAG_NE)) {
        __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_ORE | UART_FLAG_FE | UART_FLAG_NE);
        rx_index = 0;  // Сброс буфера при ошибке
        end_marker_counter = 0;
    }
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */

  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN UART4_IRQn 1 */

  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles USART6 global interrupt.
  */
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */

  /* USER CODE END USART6_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART6_IRQn 1 */

  /* USER CODE END USART6_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
