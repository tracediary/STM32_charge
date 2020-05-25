#include "networkUtil.h"



extern QueueHandle_t sendMqttQueue;

const char* topic[TOPIC_NUM] = {SYS_START, SYS_IDLE, CLOSE_INFO, PERIOD, QUERY_CHANNEL_ACK, SET_CHANNEL_ACK};



bool send2Mqtt(cJSON* cJSON_Data,uint8_t serial)
{
	char* p ;
	
	//size_t freeSize = 0;
	
	MqttSendBuf_S buf;
	
	//freeSize = xPortGetFreeHeapSize();
	//printf("%s before apply the free size is %d\n", pcTaskGetName(xTaskGetCurrentTaskHandle()),freeSize);
	
	if(serial > (TOPIC_NUM - 1))
	{
		printf("%s the topic number is invalid\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
		return false;
	}	
	
	p = cJSON_Print(cJSON_Data);
	buf.lenth = strlen(p);
	if (buf.lenth > (MQTT_TX_BUF_SIZE - TOPIC_CODE_LEN - 1))
	{
		printf("%s json data too long\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
		vPortFree(p);		
		return false;
	}
	
	buf.sendBuf = pvPortMalloc((buf.lenth + TOPIC_CODE_LEN + 1));	
	if(buf.sendBuf == NULL)
	{
		Debug("apply ram for %s send buf failed\n",pcTaskGetName(xTaskGetCurrentTaskHandle()));
		vPortFree(p);
		return false;
	}
	
	//freeSize = xPortGetFreeHeapSize();	
	//printf("%s after apply the free size is %d\n", pcTaskGetName(xTaskGetCurrentTaskHandle()),freeSize);
#if (MULTI_TOPIC == 1)
	sprintf(buf.sendBuf,"%s",topic[serial]);
	
	memset((buf.sendBuf + TOPIC_CODE_LEN), 0, (buf.lenth + 1));
	memcpy((buf.sendBuf + TOPIC_CODE_LEN), p, buf.lenth);
	
	buf.lenth = buf.lenth + TOPIC_CODE_LEN;
	
#else
	
	memset(buf.sendBuf, 0, (buf.lenth + 1));
	memcpy(buf.sendBuf, p, buf.lenth);
	
#endif	
	vPortFree(p);
	p = NULL;
	
	Debug("%s send json to net\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
	
	if (xQueueSend(sendMqttQueue, &buf, (DELAY_BASE_SEC_TIME * 5)) != pdPASS )
	{
		vPortFree(buf.sendBuf);
		buf.sendBuf = NULL;
		return false;
	}
	
	//freeSize = xPortGetFreeHeapSize();	
	//printf("%s after send the free size is %d\n", pcTaskGetName(xTaskGetCurrentTaskHandle()),freeSize);
	
	
	return true;
}