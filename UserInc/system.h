/*
 * system.h
 *
 *  Created on: May 18, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */

#ifndef USERINC_SYSTEM_H_
#define USERINC_SYSTEM_H_



#include "stm32f1xx_hal.h"
#include "constant.h"
#include "debug.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
#include "hardware.h"

/***FreeRTOS var define**************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "queue.h"

/***************enum define start***********************/
typedef enum
{
	CLOSE = 1, OPEN = 2, ERROR_ACTION = 3
} ACTION_E;

typedef enum
{
	QUERY = 1, ACTIVE = 2, ERR_MSG = 3
} MSG_TYPE_E;


typedef enum
{
	SUC = 1, ERR = 2
} EXECUTE_STATUS_E;


typedef enum
{
	SYS_OPEN = 1, SYS_CLOSE = 2, TIME_EXPIRE_CLOSE = 3, FULL_CLOSE = 4, ERR_CLOSE = 5
} EXECUTE_INFO_E;

/***************enum define end***********************/



/***************struct define start***********************/

typedef struct _USART_RX
{
	uint8_t index;
	uint8_t* usartRxBuffer[2];

} UsartRx_S;




typedef struct _MQTT_SEND_BUF
{
	uint32_t lenth;
	uint8_t *sendBuf;

} MqttSendBuf_S;



typedef struct _CHANNEL_STATUS
{
	uint32_t channelID;
	ACTION_E channelStatus;
	uint32_t maxTime;
	uint32_t curTime;
	double volt;
	double current;
	double power;
	double energy;
} ChannelStatus_S;




typedef struct _QUERY_CHANNEL_INFO
{
	uint32_t msgSN;
	ChannelStatus_S *channelInfo;

} QueryChannelInfo_S;

typedef struct _CLOSE_CHANNEL_INFO
{
	EXECUTE_INFO_E detail;
	ChannelStatus_S *channelInfo;

} CloseChannelInfo_S;



typedef struct _MSG
{
	EXECUTE_STATUS_E executeStatus;
	MSG_TYPE_E msgType;
	ACTION_E action;
	uint32_t channelID;
	uint32_t maxTime;
	uint32_t msgSN;

} Msg_S, OpenChannelInfo_S;

typedef struct _CHANNEL_KEY
{
	uint16_t keyPin;
	GPIO_TypeDef * keyPort;

} RelayKey_S, SwitchKey;



typedef struct _SYSTEM_INFO
{
	uint16_t keyPin;
	GPIO_TypeDef * keyPort;

} SystemInfo_S;



/***************struct define end***********************/





#endif /* USERINC_SYSTEM_H_ */
