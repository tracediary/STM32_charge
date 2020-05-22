/*
 * cammand.c
 *
 *  Created on: May 18, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */
#include "cammand.h"
#include "cJSON_Process.h"
#include "maintain.h"





MqttSendBuf_S activeSendBuf[2];
MqttSendBuf_S querySendBuf[2];

void vTimerCallback(xTimerHandle pxTimer);


extern QueueHandle_t activeQueue;
extern QueueHandle_t queryQueue;
extern QueueHandle_t sendMqttQueue;
extern QueueHandle_t closeInfoQueue;
extern QueueHandle_t openInfoQueue;

extern EventGroupHandle_t hlwEventHandler;
extern EventGroupHandle_t sysEventHandler;


const RelayKey_S relayInfo[CHANNEL_MAX] = { { KJ_IN1_PIN, KJ_IN1_GPIO_PORT }, { KJ_IN2_PIN, KJ_IN2_GPIO_PORT }, 
{KJ_IN3_PIN, KJ_IN3_GPIO_PORT}, { KJ_IN4_PIN, KJ_IN4_GPIO_PORT }, { KJ_IN5_PIN, KJ_IN5_GPIO_PORT }, {KJ_IN6_PIN, KJ_IN6_GPIO_PORT}, { KJ_IN7_PIN, KJ_IN7_GPIO_PORT }, { KJ_IN8_PIN, KJ_IN8_GPIO_PORT }, 
{KJ_IN9_PIN, KJ_IN9_GPIO_PORT}, { KJ_IN10_PIN, KJ_IN10_GPIO_PORT } };


const uint32_t eventChannel[CHANNEL_MAX] = { EVENTBIT_CHANNEL_1, EVENTBIT_CHANNEL_2, EVENTBIT_CHANNEL_3, EVENTBIT_CHANNEL_4,
                                        EVENTBIT_CHANNEL_5, EVENTBIT_CHANNEL_6, EVENTBIT_CHANNEL_7, EVENTBIT_CHANNEL_8, EVENTBIT_CHANNEL_9,
                                        EVENTBIT_CHANNEL_10
                                      };



bool excuteAction(uint32_t channelId, ACTION_E action);
uint8_t updatActiveJson(cJSON* cJSON_Data, Msg_S* msg);
uint8_t updatQueryJson(cJSON* cJSON_Data, QueryChannelInfo_S* chInfo);
void updateMaintain(Msg_S msgBuf);


void activeTask(void *pvParameters)
{
    BaseType_t xReturn = pdFALSE;
    Msg_S msgBuf;
    cJSON* cJSON_Data = cJSON_Data_InitActive();
    uint8_t index = 0;
    char* p ;
	
	activeSendBuf[0].sendBuf = pvPortMalloc(MQTT_TX_BUF_SIZE);
	activeSendBuf[1].sendBuf = pvPortMalloc(MQTT_TX_BUF_SIZE);
	
	if(activeSendBuf[0].sendBuf == NULL  || activeSendBuf[1].sendBuf == NULL)
	{
		Debug("apply ram for active send buf failed, restart!");
		//TODO 重启
		while(1)
			;
	}
	
    Debug("active task start\n");
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_ACTIVE_TASK);

    while (1)
    {
        xReturn = xQueueReceive(activeQueue, &msgBuf, portMAX_DELAY);
        if (!xReturn)
        {
            continue;
        }
        msgBuf.executeStatus = excuteAction(msgBuf.channelID, msgBuf.action) ? SUC : ERR;
#if 0
        Debug("active task get msg, the id is %d, action is %d, time is %d, SN is %d, status is %d, type is %d\n",
              msgBuf.channelID, msgBuf.action, msgBuf.maxTime, msgBuf.msgSN, msgBuf.executeStatus, msgBuf.msgType);
#endif

        if (!updatActiveJson(cJSON_Data, &msgBuf))
        {
            printf("update json failed, do nothing\n");
            continue;
        }

        p = cJSON_Print(cJSON_Data);
        activeSendBuf[index].lenth = strlen(p);
        if (activeSendBuf[index].lenth > MQTT_TX_BUF_SIZE)
        {
            printf("the active ack data too long\n");
            continue;
        }

        memset(activeSendBuf[index].sendBuf , 0, MQTT_TX_BUF_SIZE);
        memcpy(activeSendBuf[index].sendBuf, p, activeSendBuf[index].lenth);
		
		//Debug("the json is: %sthe lenth is %d\n",activeSendBuf[index].sendBuf,activeSendBuf[index].lenth);
		
        vPortFree(p);
        p = NULL;

        xQueueSend(sendMqttQueue, &activeSendBuf[index], DELAY_BASE_SEC_TIME);
		updateMaintain(msgBuf);
        index = !index;
		
    }
}


void updateMaintain(Msg_S msgBuf)
{
	OpenChannelInfo_S openInfo;
	
	if(msgBuf.channelID > CHANNEL_MAX || msgBuf.channelID < 1 )
	{
		printf("the channle id is error\n");
		return ;
	}

    switch (msgBuf.action)
    {
    case CLOSE:
		xQueueSend(closeInfoQueue, &(msgBuf.channelID), DELAY_BASE_SEC_TIME);
        return;
    case OPEN:
		openInfo.channelID = msgBuf.channelID;
		openInfo.maxTime = msgBuf.maxTime;
	
        xQueueSend(openInfoQueue, &openInfo, DELAY_BASE_SEC_TIME);
        return ;
    default:
        printf("the cmd is invalid, do nothing\n");
        return ;
    }
}


void queryTask(void *pvParameters)
{
    BaseType_t xReturn = pdFALSE;
	QueryChannelInfo_S queryChannelInfo;
	
	ChannelStatus_S channelStatus;
    Msg_S msgBuf;
	
	uint8_t index = 0;
    char* p ;
	
	cJSON* cJSON_Data = cJSON_Data_InitQuery();
	
	querySendBuf[0].sendBuf = pvPortMalloc(MQTT_TX_BUF_SIZE);
	querySendBuf[1].sendBuf = pvPortMalloc(MQTT_TX_BUF_SIZE);
	
	if(querySendBuf[0].sendBuf == NULL  || querySendBuf[1].sendBuf == NULL)
	{
		Debug("apply ram for query send buf failed, restart!");
		//TODO 重启
		while(1)
			;
	}
    Debug("query task start\n");
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_QUERY_TASK);
    while (1)
    {
        xReturn = xQueueReceive(queryQueue, &msgBuf, portMAX_DELAY);
        if (!xReturn)
        {
			Error("++++++++++++++++");
            continue;
        }
		
		queryChannelInfo.msgSN = msgBuf.msgSN;
		channelStatus = getChannelStatus(msgBuf.channelID);
		queryChannelInfo.channelInfo = &channelStatus;
		
		if( !updatQueryJson(cJSON_Data, &queryChannelInfo))
		{
			Error("update query json failed\n");
			continue;
		}
		
		p = cJSON_Print(cJSON_Data);
        querySendBuf[index].lenth = strlen(p);
        if (querySendBuf[index].lenth > MQTT_TX_BUF_SIZE)
        {
            printf("the active ack data too long\n");
            continue;
        }

        memset(querySendBuf[index].sendBuf , 0, MQTT_TX_BUF_SIZE);
        memcpy(querySendBuf[index].sendBuf, p, querySendBuf[index].lenth);
		
		Debug("the json is the lenth is %d\n",querySendBuf[index].lenth);
		
        vPortFree(p);
        p = NULL;
        xQueueSend(sendMqttQueue, &querySendBuf[index], DELAY_BASE_SEC_TIME);
        index = !index;
		printf("wait next query event\n");
       
    }
}





uint8_t updatActiveJson(cJSON* cJSON_Data, Msg_S* msg)
{
    uint8_t ret = 1;
    double msgType = msg->msgType;
    double executeStatus = msg->executeStatus;
    double action = msg->action;
	double channelID = msg->channelID;
	double sn = msg->msgSN;
	
	double maxTime = msg->maxTime;

    ret =ret& cJSON_Update(cJSON_Data, MSG_SN, &sn);

    ret =ret& cJSON_Update(cJSON_Data, MSG_TYPE, &msgType);

    ret =ret& cJSON_Update(cJSON_Data, CHANNEL_ID, &channelID);

    ret =ret& cJSON_Update(cJSON_Data, MAX_TIME, &maxTime);

    ret =ret& cJSON_Update(cJSON_Data, ACTION, &action);
	
    ret =ret& cJSON_Update(cJSON_Data, EXECUTE_STATUS, &executeStatus);

    return ret;
}


uint8_t updatQueryJson(cJSON* cJSON_Data, QueryChannelInfo_S* chInfo)
{
    uint8_t ret = 1;
	
	if(chInfo == NULL)
	{
		return 0;
	}
	
	
	double channelID = chInfo->channelInfo->channelID;
	double sn = chInfo->msgSN;
	double chStatus = chInfo->channelInfo->channelStatus;
	double maxTime = chInfo->channelInfo->maxTime;
    double curTime = chInfo->channelInfo->curTime;
	
	ret =ret& cJSON_Update(cJSON_Data, MSG_SN, &sn);
	
	ret =ret& cJSON_Update(cJSON_Data, CHANNEL_ID, &channelID);
	ret =ret& cJSON_Update(cJSON_Data, CHANNEL_STATUS, &chStatus);
	ret =ret& cJSON_Update(cJSON_Data, MAX_TIME, &maxTime);
	ret =ret& cJSON_Update(cJSON_Data, CUR_TIME, &curTime);
	
	ret =ret& cJSON_Update(cJSON_Data, VOLT, &(chInfo->channelInfo->volt));
    ret =ret& cJSON_Update(cJSON_Data, CURRENT, &(chInfo->channelInfo->current));
	ret =ret& cJSON_Update(cJSON_Data, POWER, &(chInfo->channelInfo->power));
	ret =ret& cJSON_Update(cJSON_Data, ENERGY, &(chInfo->channelInfo->energy));
    
    return ret;
}





bool excuteAction(uint32_t channelId, ACTION_E action)
{
	if(channelId > CHANNEL_MAX || channelId < 1 )
	{
		printf("the channle id is error\n");
		return false;
	}

    switch (action)
    {
    case CLOSE:
        
        HAL_GPIO_WritePin(relayInfo[channelId - 1].keyPort, relayInfo[channelId - 1].keyPin, GPIO_PIN_RESET);
        Debug("go to clear bits\n");
        xEventGroupClearBits(hlwEventHandler, eventChannel[channelId - 1]);
        Debug("close channel id is %d\n", channelId);
        return true;
    case OPEN:
        HAL_GPIO_WritePin(relayInfo[channelId - 1].keyPort, relayInfo[channelId - 1].keyPin, GPIO_PIN_SET);
        xEventGroupSetBits(hlwEventHandler, eventChannel[channelId - 1]);
        return true;
    default:
        printf("the cmd is invalid, do nothing\n");
        return false;
    }

}





bool closeChannel(uint32_t channelId)
{
	return excuteAction(channelId, CLOSE);
}











