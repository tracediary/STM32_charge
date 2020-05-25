/*
 * maintain.c
 *
 *  Created on: May 19, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */
#include "maintain.h"
#include <cJSON_Process.h>
#include "cammand.h"
#include "networkUtil.h"

UsartRx_S hlwData;
uint8_t * hlwUartCurdataAddr;

ChannelStatus_S chStatus[CHANNEL_MAX];
SemaphoreHandle_t countSemphr;
SemaphoreHandle_t RecMutexChannelSemphrs[CHANNEL_MAX];
TimerHandle_t PeriRepTimer;

extern const uint32_t eventChannel[CHANNEL_MAX];
extern EventGroupHandle_t hlwEventHandler;

extern QueueHandle_t closeInfoQueue;
extern QueueHandle_t openInfoQueue;
extern EventGroupHandle_t sysEventHandler;
extern TaskHandle_t PeriRep_Handle;

extern UART_HandleTypeDef *hlw8032Uart;

const SwitchKey switchInfo[SWITCH_MAX] = { { P_A_PIN, P_A_GPIO_PORT }, { P_B_PIN, P_B_GPIO_PORT }, {P_C_PIN, P_C_GPIO_PORT}, { P_D_PIN, P_D_GPIO_PORT } };


uint8_t updateMaintainJson(cJSON* cJSON_Data, CloseChannelInfo_S* chInfo);

bool getHlwData(uint8_t* data, ChannelStatus_S *channelInfo);
void switchChannel(uint32_t channelID);
void vTimerCallback(xTimerHandle pxTimer);


void clearChannelInfo(uint32_t channelID)
{
	if(channelID > CHANNEL_MAX || channelID < 1)
	{
		return;
	}
	if (xSemaphoreTakeRecursive(RecMutexChannelSemphrs[channelID - 1], DELAY_BASE_10MIL_TIME) == pdFALSE)
	{
		Error("get channel get the mutex semphr failed\n");
		return;
	}
	chStatus[channelID - 1].channelStatus = CLOSE;
	chStatus[channelID - 1].current = 0;
	chStatus[channelID - 1].curTime = 0;
	chStatus[channelID - 1].energy = 0;
	chStatus[channelID - 1].maxTime = 0;
	chStatus[channelID - 1].power = 0;
	chStatus[channelID - 1].volt = VOLT_220V;
	
	xSemaphoreGiveRecursive(RecMutexChannelSemphrs[channelID - 1]);
}

ChannelStatus_S getChannelStatus(uint32_t channelID)	
{
	ChannelStatus_S channelInfo = {0};
	if(channelID > CHANNEL_MAX || channelID < 1)
	{
		return channelInfo;
	}
	if (xSemaphoreTakeRecursive(RecMutexChannelSemphrs[channelID - 1], DELAY_BASE_10MIL_TIME) == pdFALSE)
	{
		Error("get channel get the mutex semphr failed\n");
		return channelInfo;
	}
	
	channelInfo = chStatus[channelID - 1];
	xSemaphoreGiveRecursive(RecMutexChannelSemphrs[channelID - 1]);
	
	return channelInfo;
}

bool changeChannel(ChannelStatus_S *channel)
{
	if (xSemaphoreTakeRecursive(RecMutexChannelSemphrs[channel->channelID - 1], (DELAY_BASE_10MIL_TIME * 10)) == pdFALSE)
	{
		Error("change channel get the mutex semphr failed\n");
		return false;
	}
	chStatus[channel->channelID - 1].channelStatus = channel->channelStatus;
	chStatus[channel->channelID - 1].current = channel->current;
	chStatus[channel->channelID - 1].curTime = channel->curTime;
	chStatus[channel->channelID - 1].energy = channel->energy;
	chStatus[channel->channelID - 1].maxTime = channel->maxTime;
	chStatus[channel->channelID - 1].power = channel->power;
	chStatus[channel->channelID - 1].volt = channel->volt;
			
	xSemaphoreGiveRecursive(RecMutexChannelSemphrs[channel->channelID - 1]);
	return true;
}


void initChannel()
{
	int i = 0;
	for(;i < CHANNEL_MAX;i++)
	{
		chStatus[i].channelID = i + 1;
		chStatus[i].channelStatus = CLOSE;
		chStatus[i].current = 0;
		chStatus[i].curTime = 0;
		chStatus[i].energy = 0;
		chStatus[i].maxTime = 0;
		chStatus[i].power = 0;
		chStatus[i].volt = 0;
	}
}


uint8_t updateMaintainJson(cJSON* cJSON_Data, CloseChannelInfo_S* chInfo)
{
    uint8_t ret = 1;
	
	if(chInfo == NULL)
	{
		return 0;
	}
	
	double channelID = chInfo->channelInfo->channelID;
	double detail = chInfo->detail;
	double chStatus = chInfo->channelInfo->channelStatus;
	double maxTime = chInfo->channelInfo->maxTime;
    double curTime = chInfo->channelInfo->curTime;
	
	ret =ret& cJSON_Update(cJSON_Data, DETAIL, &detail);
	
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

uint8_t updatePeriRepJson(cJSON* cJSON_Data, ChannelStatus_S* chInfo)
{
    uint8_t ret = 1;
	
	if(chInfo == NULL)
	{
		return 0;
	}
	
	double channelID = chInfo->channelID;
	double chStatus = chInfo->channelStatus;
	double maxTime = chInfo->maxTime;
    double curTime = chInfo->curTime;
	

	ret =ret& cJSON_Update(cJSON_Data, CHANNEL_ID, &channelID);
	ret =ret& cJSON_Update(cJSON_Data, CHANNEL_STATUS, &chStatus);
	ret =ret& cJSON_Update(cJSON_Data, MAX_TIME, &maxTime);
	ret =ret& cJSON_Update(cJSON_Data, CUR_TIME, &curTime);
	
	ret =ret& cJSON_Update(cJSON_Data, VOLT, &(chInfo->volt));
    ret =ret& cJSON_Update(cJSON_Data, CURRENT, &(chInfo->current));
	ret =ret& cJSON_Update(cJSON_Data, POWER, &(chInfo->power));
	ret =ret& cJSON_Update(cJSON_Data, ENERGY, &(chInfo->energy));
    
    return ret;
}




void switchChannel(uint32_t channelID)
{
	uint8_t i = 0;
	if(channelID > CHANNEL_MAX || channelID < 1)
	{
		Error("switch channel id invalid\n");
		return;
	}
	
	for(;i < SWITCH_MAX; i++)
	{
		HAL_GPIO_WritePin(switchInfo[i].keyPort, switchInfo[i].keyPin, ( ((channelID - 1) &(1<<i)) ? GPIO_PIN_SET:GPIO_PIN_RESET ) );
	}
}


bool getHlwData(uint8_t* data, ChannelStatus_S *channelInfo)
{
	return true;
}




void creatTimer()
{
	PeriRepTimer  = xTimerCreate("PeriRepTimer",(DELAY_BASE_MIN_TIME),pdFALSE,(void *)1,vTimerCallback);
}

void vTimerCallback(xTimerHandle pxTimer)
{
	xTaskNotifyGive(PeriRep_Handle);
}


void changeTimerPeriod( uint32_t period)
{
	xTimerChangePeriod(PeriRepTimer,(period * 1000),0);
	xTimerReset(PeriRepTimer,0);
}






void sysStartEvent()
{
	int i  = 0;
	cJSON* cJSON_Data = cJSON_Data_InitStartEvent();
	
	send2Mqtt(cJSON_Data);
	
	if(cJSON_Data!= NULL)
	{
		cJSON_Delete(cJSON_Data);
	}

}



void maintainTask(void *pvParameters)
{
    BaseType_t xReturn = pdFALSE;
	EventBits_t eventBit;
	cJSON* cJSON_Data = cJSON_Data_InitCloseChInfo();
	
	CloseChannelInfo_S closeInfo;
	uint8_t index = 0;

	countSemphr = xSemaphoreCreateCounting(0xffff,0);
	
	for(index = 0;index < CHANNEL_MAX;index++)
	{
		RecMutexChannelSemphrs[index] = xSemaphoreCreateRecursiveMutex();
	}
	
	initChannel();
    printf("maintain task start\n");
	
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_MAINTAIN_TASK);
	
    while (1)
    {
		xSemaphoreTake(countSemphr, portMAX_DELAY);
		
		for(index = 0; index < CHANNEL_MAX ; index++)
		{
			eventBit = xEventGroupGetBits(hlwEventHandler);

			if((eventBit & eventChannel[index]) == 0)
			{
				continue;
			}
			
			if (xSemaphoreTakeRecursive(RecMutexChannelSemphrs[index], (DELAY_BASE_10MIL_TIME * 10)) == pdFALSE)
			{
				Error("get the mutex semphr failed\n");
				continue;
			}
			chStatus[index].curTime = chStatus[index].curTime  + 1;
			
			
			//判断时间和是否充满
			if(chStatus[index].curTime < MIN_COLLECT_TIME && chStatus[index].maxTime >= MIN_COLLECT_TIME)
			{
				goto RELEASE_SEMPHR;
			}
			if(chStatus[index].curTime > chStatus[index].maxTime)
			{
				closeInfo.detail = TIME_EXPIRE_CLOSE;
			}
			#if 0
			else if(chStatus[index].current < MIN_CURRENT )
			{
				closeInfo.detail = FULL_CLOSE;
			}
			#endif
			else
			{
				goto RELEASE_SEMPHR;
			}
			
			
			//发送计时/充满关闭事件
			
			closeChannel(chStatus[index].channelID);
			
			chStatus[index].channelStatus = CLOSE;
			closeInfo.channelInfo = &chStatus[index];
			
			if(!updateMaintainJson(cJSON_Data, &closeInfo))
			{
				Error("update maintain json failed, do nothing\n");
				goto RELEASE_SEMPHR;
			}
			send2Mqtt(cJSON_Data);
			
			clearChannelInfo((index + 1));

RELEASE_SEMPHR:
			xSemaphoreGiveRecursive(RecMutexChannelSemphrs[index]);
			
			
			
		}
    }
}



void uploadCloseInfoTask(void *pvParameters)
{
	BaseType_t xReturn = pdTRUE;
	uint32_t channelId;
	uint8_t index = 0;

	cJSON* cJSON_Data = cJSON_Data_InitCloseChInfo();
	
	CloseChannelInfo_S closeInfo;
	ChannelStatus_S channelInfo;
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_UP_CL_INFO_TASK);

	printf("upload close info task start\n");
	
	while (1)
	{
		xReturn = xQueueReceive(closeInfoQueue, &channelId, portMAX_DELAY);
		if (!xReturn)
		{
			continue;
		}
		
		if(channelId > CHANNEL_MAX || channelId < 1)
		{
			Debug("the channelId is invalid\n");
			continue;
		}
		
		
		if (xSemaphoreTakeRecursive(RecMutexChannelSemphrs[channelId - 1], (DELAY_BASE_10MIL_TIME * 10)) == pdFALSE)
		{
			Error("upload task get the mutex semphr failed, clear cache\n");
			goto CLEAR_CACHE;
		}
		channelInfo = getChannelStatus(channelId);
		channelInfo.channelStatus = CLOSE;
		
		closeInfo.detail = SYS_CLOSE;
		closeInfo.channelInfo = &channelInfo;
		
		if(updateMaintainJson(cJSON_Data, &closeInfo))
		{
			send2Mqtt(cJSON_Data);
		}
		
		xSemaphoreGiveRecursive(RecMutexChannelSemphrs[channelId - 1]);
		
	CLEAR_CACHE:
		
		clearChannelInfo(channelId);
		

	}
}




void maintainOpenTask(void *pvParameters)
{
	BaseType_t xReturn = pdTRUE;
	OpenChannelInfo_S openInfo;
	ChannelStatus_S channelInfo;
	
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_MT_OPEN_TASK);

	printf("maintain open task start\n");
	
	while (1)
	{
		xReturn = xQueueReceive(openInfoQueue, &openInfo, portMAX_DELAY);
		if (!xReturn)
		{
			Debug("open task not get info\n");
			continue;
		}
		
		if(openInfo.channelID > CHANNEL_MAX || openInfo.channelID < 1)
		{
			Debug("the channelId is invalid\n");
			continue;
		}
		
		
		if (xSemaphoreTakeRecursive(RecMutexChannelSemphrs[openInfo.channelID - 1], (DELAY_BASE_10MIL_TIME * 10)) == pdFALSE)
		{
			Error("upload task get the mutex semphr failed\n");
			continue;
		}
		
		Debug("open task get info\n");
		
		channelInfo = getChannelStatus(openInfo.channelID);
		channelInfo.channelStatus = OPEN;
		channelInfo.maxTime = openInfo.maxTime;
		changeChannel(&channelInfo);

		xSemaphoreGiveRecursive(RecMutexChannelSemphrs[openInfo.channelID - 1]);

	}
}


void maintainHlwTask(void *pvParameters)
{
	uint8_t index  = 0;
	uint8_t repeat  = 1;
	
	EventBits_t eventBit = 0;
	
	ChannelStatus_S channelInfo;
	
	hlwData.usartRxBuffer[0] = pvPortMalloc(NORMAL_BUF_SIZE);
	hlwData.usartRxBuffer[1] = pvPortMalloc(NORMAL_BUF_SIZE);
	
	if(hlwData.usartRxBuffer[0] == NULL  || hlwData.usartRxBuffer[1] == NULL)
	{
		Debug("apply ram for maintain hlw8032 task buf failed, restart!");
		while(1)
			;
	}
	hlwData.index = 0;
	
	HAL_UART_Receive_DMA(hlw8032Uart, hlwData.usartRxBuffer[!hlwData.index], NORMAL_BUF_SIZE);
	hlwUartCurdataAddr = hlwData.usartRxBuffer[hlwData.index];

	
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_MT_HLW_TASK);
	
	printf("maintain hlw task start\n");

	while(1)
	{
		
		eventBit = xEventGroupWaitBits(hlwEventHandler, EVENTBIT_CHANNEL, pdFALSE, pdFALSE, portMAX_DELAY);
		if ((eventBit & EVENTBIT_CHANNEL) == 0)
		{
			Debug("no channel open, wait\n");
			continue;
		}
		
		Debug("start to get the hlw data\n");

		for(index = 0; index < CHANNEL_MAX;)
		{
			eventBit = xEventGroupGetBits(hlwEventHandler);
			if((eventBit &  eventChannel[index]) == 0)
			{
				//通道关闭，切换下一个
				goto SWITCH_CHANNEL;
			}
			
			xEventGroupClearBits(hlwEventHandler, EVENTBIT_HLW_UART_REC);
			eventBit = xEventGroupWaitBits(hlwEventHandler, EVENTBIT_HLW_UART_REC, pdTRUE, pdFALSE, (DELAY_BASE_SEC_TIME*5));
			if ((eventBit & EVENTBIT_HLW_UART_REC) == 0)
			{
				//等待串口数据超时，切换到下一个通道
				Error("maintain hlw wait the uart data overtime, switch\n");
				goto SWITCH_CHANNEL;
			}

		GET_MUTEX_SEMPHR:		
			if (xSemaphoreTakeRecursive(RecMutexChannelSemphrs[index], (DELAY_BASE_10MIL_TIME * 50)) == pdFALSE)
			{
				if(repeat++ > NORMAL_REPEAT_TIME)
				{
					Error("maintain hlw get the mutex semphr failed, too much time, switch channel\n");
					goto SWITCH_CHANNEL;
				}
				
				Error("maintain hlw get the mutex semphr failed, try again\n");
				
				continue;
			}

			
			eventBit = xEventGroupGetBits(hlwEventHandler);
			if ((eventBit & eventChannel[index]) == 0)
			{
				//通道已经关闭，切换到下一个通道
				xSemaphoreGiveRecursive(RecMutexChannelSemphrs[index]);
				goto SWITCH_CHANNEL;
			}
			
			channelInfo = getChannelStatus((index + 1));

			if(!getHlwData(hlwUartCurdataAddr, &channelInfo))
			{
				//参数校验失败，最多重试NORMAL_REPEAT_TIME次
				xSemaphoreGiveRecursive(RecMutexChannelSemphrs[index]);
				
				if(repeat++ > NORMAL_REPEAT_TIME)
				{
					Error("valid the data failed, overtime, switch channel\n");
					goto SWITCH_CHANNEL;
				}
				else
				{
					Error("valid the data failed, repeat\n");
					continue;
				}
			}
			
			changeChannel(&channelInfo);
			xSemaphoreGiveRecursive(RecMutexChannelSemphrs[index]);

		SWITCH_CHANNEL:
			switchChannel((index + 1));
			repeat = 1;
			index++;
		}

	}
	
}



void periRepTask(void *pvParameters)
{
	EventBits_t eventValue = 0;
	uint32_t channelId;
	uint8_t i = 0;
	ChannelStatus_S channelInfo;

	cJSON* cJSON_Data = cJSON_Data_InitChStatusRep();	
	cJSON* cJSON_idle = cJSON_Data_InitChannelIdle();	
	xEventGroupSetBits(sysEventHandler, EVENTBIT_SYS_PERI_REP_TASK);

	printf("peried report task start\n");

	vTaskDelay(DELAY_BASE_SEC_TIME * 15);
	sysStartEvent();
	while (1)
	{
		eventValue = xEventGroupWaitBits(hlwEventHandler, EVENTBIT_CHANNEL, pdFALSE, pdFALSE, MAINTAIN_PERIDIC_REPORT);
		if ((eventValue & EVENTBIT_CHANNEL) == 0)
		{
			//无一通道使用，上报所有空闲
			send2Mqtt(cJSON_idle);
			
			continue;
		}
		//等1分唤醒，启动定时器
		xTimerStart(PeriRepTimer,0);
		
		ulTaskNotifyTake(pdTRUE,(DELAY_BASE_MIN_TIME * 10));
		
		eventValue = xEventGroupGetBits(hlwEventHandler);
		for(i = 0;i < CHANNEL_MAX ; i++)
		{
			channelId = i + 1;
			if(!(eventValue & eventChannel[i]))
			{
				continue;
			}
			if (xSemaphoreTakeRecursive(RecMutexChannelSemphrs[i], (DELAY_BASE_10MIL_TIME * 10)) == pdFALSE)
			{
				Error("periRepTask get the mutex semphr failed\n");
				continue;
			}
			
			channelInfo = getChannelStatus(channelId);
			if(updatePeriRepJson(cJSON_Data, &channelInfo))
			{
				send2Mqtt(cJSON_Data);
			}

			xSemaphoreGiveRecursive(RecMutexChannelSemphrs[i]);
		}
	}
}






