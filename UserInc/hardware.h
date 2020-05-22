/*
 * hardware.h
 *
 *  Created on: May 18, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */

#ifndef USERINC_HARDWARE_H_
#define USERINC_HARDWARE_H_



#include "stm32f1xx_hal.h"

/**
 * @brief general define, this file base on the EmbedFile Badao v2 board
 */

/* LED pin define***************************************************************/
#define LED1_CLK_EN							__HAL_RCC_GPIOA_CLK_ENABLE()
#define LED1_PIN							GPIO_PIN_11
#define LED1_GPIO_PORT						GPIOA

#define LED_RUN_CLK_EN						__HAL_RCC_GPIOC_CLK_ENABLE()
#define LED_RUN_PIN							GPIO_PIN_8
#define LED_RUN_GPIO_PORT					GPIOC

/*******debug USART define******************************************************/
#define USART3_TX_CLK_EN					__HAL_RCC_GPIOA_CLK_ENABLE()
#define USART3_TX_PIN						GPIO_PIN_8
#define USART3_TX_GPIO_PORT					GPIOD
#define USART3_RX_CLK_EN					__HAL_RCC_GPIOA_CLK_ENABLE()
#define USART3_RX_PIN						GPIO_PIN_9
#define USART3_RX_GPIO_PORT					GPIOD
#define DEBUG_UART							USART3

/**********mqtt define**********************************************************/
#define MQTT_USART1_TX_CLK_EN				__HAL_RCC_GPIOA_CLK_ENABLE();
#define MQTT_USART1_TX_PIN					GPIO_PIN_9
#define MQTT_USART1_TX_GPIO_PORT			GPIOA

#define MQTT_USART1_RX_CLK_EN				__HAL_RCC_GPIOA_CLK_ENABLE();
#define MQTT_USART1_RX_PIN					GPIO_PIN_10
#define MQTT_USART1_RX_GPIO_PORT			GPIOA

/*********************RELAY PIN*************************************************/
#define KJ_IN1_PIN							GPIO_PIN_1
#define KJ_IN1_GPIO_PORT					GPIOE

#define KJ_IN2_PIN							GPIO_PIN_0
#define KJ_IN2_GPIO_PORT					GPIOE

#define KJ_IN3_PIN							GPIO_PIN_9
#define KJ_IN3_GPIO_PORT					GPIOB


#define KJ_IN4_PIN							GPIO_PIN_4
#define KJ_IN4_GPIO_PORT					GPIOD

#define KJ_IN5_PIN							GPIO_PIN_3
#define KJ_IN5_GPIO_PORT					GPIOD

#define KJ_IN6_PIN							GPIO_PIN_2
#define KJ_IN6_GPIO_PORT					GPIOD

#define KJ_IN7_PIN							GPIO_PIN_1
#define KJ_IN7_GPIO_PORT					GPIOD

#define KJ_IN8_PIN							GPIO_PIN_0
#define KJ_IN8_GPIO_PORT					GPIOD

#define KJ_IN9_PIN							GPIO_PIN_12
#define KJ_IN9_GPIO_PORT					GPIOC

#define KJ_IN10_PIN							GPIO_PIN_10
#define KJ_IN10_GPIO_PORT					GPIOC


/*********************SWITCH PIN*************************************************/

#define P_B_PIN								GPIO_PIN_7
#define P_B_GPIO_PORT						GPIOD

#define P_A_PIN								GPIO_PIN_3
#define P_A_GPIO_PORT						GPIOB

#define P_C_PIN								GPIO_PIN_4
#define P_C_GPIO_PORT						GPIOB

#define P_D_PIN								GPIO_PIN_5
#define P_D_GPIO_PORT						GPIOB




#endif /* USERINC_HARDWARE_H_ */
