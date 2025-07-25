/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_6
#define LED3_GPIO_Port GPIOA
#define LED4_Pin GPIO_PIN_7
#define LED4_GPIO_Port GPIOA
#define LED5_Pin GPIO_PIN_4
#define LED5_GPIO_Port GPIOC
#define LED6_Pin GPIO_PIN_5
#define LED6_GPIO_Port GPIOC
#define LED7_Pin GPIO_PIN_0
#define LED7_GPIO_Port GPIOB
#define LED8_Pin GPIO_PIN_1
#define LED8_GPIO_Port GPIOB
#define LED9_Pin GPIO_PIN_2
#define LED9_GPIO_Port GPIOB
#define LED10_Pin GPIO_PIN_11
#define LED10_GPIO_Port GPIOF
#define KN1_Pin GPIO_PIN_14
#define KN1_GPIO_Port GPIOF
#define KN2_Pin GPIO_PIN_15
#define KN2_GPIO_Port GPIOF
#define RX_disp_Pin GPIO_PIN_8
#define RX_disp_GPIO_Port GPIOD
#define TX_disp_Pin GPIO_PIN_9
#define TX_disp_GPIO_Port GPIOD
#define TX_debug_Pin GPIO_PIN_6
#define TX_debug_GPIO_Port GPIOC
#define RX_debug_Pin GPIO_PIN_7
#define RX_debug_GPIO_Port GPIOC
#define UART4_RD_Pin GPIO_PIN_11
#define UART4_RD_GPIO_Port GPIOA
#define USART2_RD_Pin GPIO_PIN_12
#define USART2_RD_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
typedef struct {
  uint8_t message[20];
}QUEUE_msg; 
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
