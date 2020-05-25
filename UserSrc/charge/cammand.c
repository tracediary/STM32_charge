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
#include "networkUtil.h"


void vTimerCallback(xTimerHandle pxTimer);


extern QueueHandle_t activeQueue;
extern QueueHandle_t queryQueue;
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
uint8_t updateActiveJson(cJSON* cJSON_Data, Msg_S* msg);
uint8_t updateQueryJson(cJSON* cJSON_Data, QueryChannelInfo_S* chInfo);
void updateMaintain(Msg_S msgBuf);


void activeTask(void *pvParameters)
{
    BaseType_t xReturn = pdFALSE;
    Msg_S msgBuf;
    cJSON* cJSON_Data = cJSON_Data_InitActive();
	
    printf("active task start\n");
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_ACTIVE_TASK);

    while (1)
    {
        xReturn = xQueueReceive(activeQueue, &msgBuf, portMAX_DELAY);
        if (!xReturn)
        {
            continue;
        }
        msgBuf.executeStatus = excuteAction(msgBuf.channelID, msgBuf.action) ? SUC : ERR;

        if (!updateActiveJson(cJSON_Data, &msgBuf))
        {
            printf("update json failed, do nothing\n");
            goto UPDATA_MAINTAIN;
        }	
		send2Mqtt(cJSON_Data);

	UPDATA_MAINTAIN:	
		
		updateMaintain(msgBuf);		
		Debug("wait next active cmd\n");		
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

	cJSON* cJSON_Data = cJSON_Data_InitQuery();
	
    printf("query task start\n");
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_QUERY_TASK);
    while (1)
    {
        xReturn = xQueueReceive(queryQueue, &msgBuf, portMAX_DELAY);
        if (!xReturn)
        {
            continue;
        }
		
		queryChannelInfo.msgSN = msgBuf.msgSN;
		channelStatus = getChannelStatus(msgBuf.channelID);
		queryChannelInfo.channelInfo = &channelStatus;
		
		if( !updateQueryJson(cJSON_Data, &queryChannelInfo))
		{
			Error("update query json failed\n");
			continue;
		}
		send2Mqtt(cJSON_Data);
		printf("wait next query event\n");
       
    }
}





uint8_t updateActiveJson(cJSON* cJSON_Data, Msg_S* msg)
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


uint8_t updateQueryJson(cJSON* cJSON_Data, QueryChannelInfo_S* chInfo)
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











