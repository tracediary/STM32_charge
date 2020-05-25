#include "networkUtil.h"



extern QueueHandle_t sendMqttQueue;



bool send2Mqtt(cJSON* cJSON_Data)
{
	char* p ;
	
	//size_t freeSize = 0;
	
	MqttSendBuf_S buf;
	
	//freeSize = xPortGetFreeHeapSize();
	//printf("%s before apply the free size is %d\n", pcTaskGetName(xTaskGetCurrentTaskHandle()),freeSize);
	
	p = cJSON_Print(cJSON_Data);
	buf.lenth = strlen(p);
	if (buf.lenth > (MQTT_TX_BUF_SIZE - 5))
	{
		printf("%s json data too long\n", pcTaskGetName(xTaskGetCurrentTaskHandle()));
		vPortFree(p);		
		return false;
	}
	
	buf.sendBuf = pvPortMalloc((buf.lenth + 5));	
	if(buf.sendBuf == NULL)
	{
		Debug("apply ram for %s send buf failed\n",pcTaskGetName(xTaskGetCurrentTaskHandle()));
		vPortFree(p);
		return false;
	}
	
	//freeSize = xPortGetFreeHeapSize();	
	//printf("%s after apply the free size is %d\n", pcTaskGetName(xTaskGetCurrentTaskHandle()),freeSize);

	memset(buf.sendBuf, 0, (buf.lenth + 5));
	memcpy(buf.sendBuf, p, buf.lenth);
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