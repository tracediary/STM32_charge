/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f1xx_hal.h"

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
#define RUNLED_Pin GPIO_PIN_8
#define RUNLED_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_11
#define LED1_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_12
#define BUZZER_GPIO_Port GPIOA
#define SETKEY_Pin GPIO_PIN_15
#define SETKEY_GPIO_Port GPIOA
#define SETKEY_EXTI_IRQn EXTI15_10_IRQn
#define KJ_IN10_Pin GPIO_PIN_10
#define KJ_IN10_GPIO_Port GPIOC
#define P_RX_Pin GPIO_PIN_11
#define P_RX_GPIO_Port GPIOC
#define KJ_IN9_Pin GPIO_PIN_12
#define KJ_IN9_GPIO_Port GPIOC
#define KJ_IN8_Pin GPIO_PIN_0
#define KJ_IN8_GPIO_Port GPIOD
#define KJ_IN7_Pin GPIO_PIN_1
#define KJ_IN7_GPIO_Port GPIOD
#define KJ_IN6_Pin GPIO_PIN_2
#define KJ_IN6_GPIO_Port GPIOD
#define KJ_IN5_Pin GPIO_PIN_3
#define KJ_IN5_GPIO_Port GPIOD
#define KJ_IN4_Pin GPIO_PIN_4
#define KJ_IN4_GPIO_Port GPIOD
#define P_B_Pin GPIO_PIN_7
#define P_B_GPIO_Port GPIOD
#define P_A_Pin GPIO_PIN_3
#define P_A_GPIO_Port GPIOB
#define P_C_Pin GPIO_PIN_4
#define P_C_GPIO_Port GPIOB
#define P_D_Pin GPIO_PIN_5
#define P_D_GPIO_Port GPIOB
#define KJ_IN3_Pin GPIO_PIN_9
#define KJ_IN3_GPIO_Port GPIOB
#define KJ_IN2_Pin GPIO_PIN_0
#define KJ_IN2_GPIO_Port GPIOE
#define KJ_IN1_Pin GPIO_PIN_1
#define KJ_IN1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
