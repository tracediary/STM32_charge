/*
 * network.c
 *
 *  Created on: May 18, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */
#include "hardware.h"
#include "system.h"
#include "network.h"
#include "usart.h"
#include "rtc.h"
#include "cJSON_Process.h"


UsartRx_S netData;
uint8_t * wifiUartCurdataAddr;
uint8_t *netTxData;


extern UART_HandleTypeDef *netUart;
extern EventGroupHandle_t netEventHandler;
extern EventGroupHandle_t sysEventHandler;
extern QueueHandle_t sendMqttQueue;
extern QueueHandle_t activeQueue;
extern QueueHandle_t queryQueue;


void clearMsgBuf(Msg_S *msg);
	

void mqttRecTask(void *pvParameters)
{
	cJSON *root;
	EventBits_t eventValue = 0;
	Msg_S msg = {0};

	netData.index = 0;
	
	netData.usartRxBuffer[0] = pvPortMalloc(MQTT_RX_BUF_SIZE);
	netData.usartRxBuffer[1] = pvPortMalloc(MQTT_RX_BUF_SIZE);
	
	if(netData.usartRxBuffer[0] == NULL  || netData.usartRxBuffer[1] == NULL)
	{
		Debug("apply ram for recive task buf failed, restart!");
		//TODO 重启
		while(1)
			;
	}
	
	HAL_UART_Receive_DMA(netUart, netData.usartRxBuffer[!netData.index], MQTT_RX_BUF_SIZE);
	wifiUartCurdataAddr = netData.usartRxBuffer[netData.index];

	Debug("mqtt receive task start\n");
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_MQTT_REC_TASK);

	while (1)
	{
		
		eventValue = xEventGroupWaitBits(netEventHandler, EVENTBIT_MQTT_UART_REC, pdTRUE, pdFALSE, portMAX_DELAY);
		if ((eventValue & EVENTBIT_MQTT_UART_REC) == 0)
		{
			continue;
		}
		Debug("get net data\n");
		clearMsgBuf(&msg);

		root = cJSON_Parse((char*) wifiUartCurdataAddr);

		if (!Json2Struct(root, &msg))
		{
			Debug("trans data to JSON failed\n");
			goto CLEAN_CACHE;
		}

		switch (msg.msgType)
		{
		case QUERY:
			xQueueSend(queryQueue, &msg, DELAY_BASE_SEC_TIME);
			break;
		case ACTIVE:
			xQueueSend(activeQueue, &msg, DELAY_BASE_SEC_TIME);
			break;
		default:
			printf("the cmd type is error\n");
			break;
		}

CLEAN_CACHE:
		if(root!= NULL)
		{
			cJSON_Delete(root);
		}
		root = NULL;

	}

}

void mqttSendTask(void *pvParameters)
{

	MqttSendBuf_S databuf ;

	BaseType_t xReturn = pdTRUE;
	Debug("mqtt send task start\n");
	
	netTxData = pvPortMalloc(MQTT_TX_BUF_SIZE);
	
	if(netTxData == NULL )
	{
		Debug("apply ram for query send buf failed, restart!");
		//TODO 重启
		while(1)
			;
	}

	xEventGroupSetBits(netEventHandler, EVENTBIT_MQTT_UART_TC );
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_MQTT_SEND_TASK);

	while (1)
	{
		xReturn = xQueueReceive(sendMqttQueue, &databuf, portMAX_DELAY);
		if (!xReturn)
		{
			continue;
		}

		netUartSend(databuf.sendBuf, databuf.lenth);
		Debug("mqtt send over, the lenth is %d, the string is %s\n",databuf.lenth,databuf.sendBuf );
	}
}




void clearMsgBuf(Msg_S *msg)
{
	msg->action = ERROR_ACTION;
	msg->channelID = CHANNEL_MAX + 1;
	msg->executeStatus = ERR;
	msg->maxTime = 0;
	msg->msgSN = 0xFFFFFFFF;
	msg->msgType = ERR_MSG;
}
	













