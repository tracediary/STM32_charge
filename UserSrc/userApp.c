/*
 * userApp.c
 *
 *  Created on: May 18, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */



#include "hardware.h"
#include "system.h"
#include "usart.h"
#include "network.h"
#include "cammand.h"
#include "userApp.h"
#include "maintain.h"

TaskHandle_t MQTT_Rec_Handle = NULL, MQTT_Send_Handle = NULL;

TaskHandle_t Active_Handle = NULL, Query_Handle = NULL;

TaskHandle_t Maintain_Handle = NULL, MT_Open_Handle = NULL;
TaskHandle_t MT_Hlw_Handle = NULL, Upload_CL_Info_Handle = NULL;

TaskHandle_t PeriRep_Handle = NULL;

EventGroupHandle_t sysEventHandler = NULL;
EventGroupHandle_t netEventHandler = NULL;
EventGroupHandle_t hlwEventHandler = NULL;

QueueHandle_t activeQueue;
QueueHandle_t sendMqttQueue;
QueueHandle_t queryQueue;
QueueHandle_t closeInfoQueue;
QueueHandle_t openInfoQueue;



void wifi_task(void *pvParameters);
void sntp_task(void *pvParameters);
void wifi_uart_msg_distribute_task(void *pvParameters);
/**user var define****************************************/




#if 0
void recTask(void *pvParameters)
{
	BaseType_t xReturn = pdFALSE;
	Msg_S msgBuf = { ERR, QUERY, OPEN, 789, 456, 321 };
	int i = 0;

	Debug("rec task start\n");
	while (1)
	{
		xReturn = xQueueReceive(activeQueue, &msgBuf, portMAX_DELAY);
		if (!xReturn)
		{
			continue;
		}
		i++;
		Debug("rec task get msg, the count is %d \n", i);

//		if (msgBuf == NULL)
//		{
//			Debug("this is the null\n");
//		}
//		else
		{
			Debug("the sn is %d, id is %d, maxtime is %d, action is %d, type is %d\n", msgBuf.msgSN,
					msgBuf.channelID, msgBuf.maxTime, msgBuf.action, msgBuf.msgType);
		}

		if (i < 7)
		{
			vTaskDelay(3000);
		}
		else
		{
			i = 0;
		}


	}
}



void sendTask(void *pvParameters)
{
	//Msg_S msg = { SUC, ACTIVE, OPEN, 2, 147, 589 };
//	Msg_S *msgBuf = &msg;
	Msg_S msg;
	int i = 1;

	msg.action = OPEN;
	msg.channelID = 9;
	msg.excuteStatus = ERR;
	msg.maxTime = 17;
	msg.msgSN = 456558;
	msg.msgType = ACTIVE;

	Debug("test send task start\n");

	while (1)
	{
		xQueueSend(activeQueue, &msg, portMAX_DELAY);
		vTaskDelay(3000);
		Debug("send over: %d\n", i);
	}

}
#endif
void appTaskInit(void)
{
#if 1
	xTaskCreate(mqttRecTask, "mqttRecTask", (configMINIMAL_STACK_SIZE * 8), NULL, (tskIDLE_PRIORITY + 9),
			&MQTT_Rec_Handle);

	xTaskCreate(mqttSendTask, "mqttSendTask", (configMINIMAL_STACK_SIZE * 8), NULL, (tskIDLE_PRIORITY + 8),
			&MQTT_Send_Handle);
	xTaskCreate(activeTask, "activeTask", (configMINIMAL_STACK_SIZE * 6), NULL, (tskIDLE_PRIORITY + 7),
			&Active_Handle);

	xTaskCreate(queryTask, "queryTask", (configMINIMAL_STACK_SIZE * 6), NULL, (tskIDLE_PRIORITY + 6),
			&Query_Handle);
	
	xTaskCreate(maintainTask, "maintainTask", (configMINIMAL_STACK_SIZE * 6), NULL, (tskIDLE_PRIORITY + 1),
			&Maintain_Handle);
	
	xTaskCreate(maintainOpenTask, "maintainOpenTask", (configMINIMAL_STACK_SIZE * 6), NULL, (tskIDLE_PRIORITY + 5),
			&MT_Open_Handle);
	
	xTaskCreate(uploadCloseInfoTask, "uploadCloseInfoTask", (configMINIMAL_STACK_SIZE * 6), NULL, (tskIDLE_PRIORITY + 4),
			&Upload_CL_Info_Handle);

	xTaskCreate(maintainHlwTask, "maintainHlwTask", (configMINIMAL_STACK_SIZE * 6), NULL, (tskIDLE_PRIORITY + 3),
			&MT_Hlw_Handle);
#if 1		
	xTaskCreate(periRepTask, "periRepTask", (configMINIMAL_STACK_SIZE * 6), NULL, (tskIDLE_PRIORITY + 2),
			&PeriRep_Handle);
	creatTimer();
	#endif
	//PeriRep_Handle();
	
#endif

}





void appOsVarInit(void)
{
	sysEventHandler = xEventGroupCreate();
	hlwEventHandler = xEventGroupCreate();
	netEventHandler = xEventGroupCreate();

	activeQueue = xQueueCreate(ACTIVE_QUEUE, sizeof(Msg_S));
	queryQueue = xQueueCreate(QUERY_QUEUE, sizeof(Msg_S));
	sendMqttQueue = xQueueCreate(SEND_MQTT_QUEUE, sizeof(MqttSendBuf_S ));
	closeInfoQueue = xQueueCreate(CLOSE_INFO_QUEUE, sizeof(uint32_t));
	openInfoQueue = xQueueCreate(OPEN_INFO_QUEUE, sizeof(OpenChannelInfo_S ));

}

