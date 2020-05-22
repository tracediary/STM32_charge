#include <cJSON_Process.h>

/*******************************************************************
 *              define var
 *******************************************************************/

cJSON* cJSON_Data_InitActive(void)
{
	cJSON* cJSON_Root = NULL;
	char* p = NULL;

	cJSON_Root = cJSON_CreateObject(); /*create obj*/
	if (NULL == cJSON_Root)
	{
		return NULL;
	}

	cJSON_AddNumberToObject(cJSON_Root, MSG_TYPE, ACTIVE);
	cJSON_AddNumberToObject(cJSON_Root, CHANNEL_ID, CHANNEL_MAX);
	cJSON_AddNumberToObject(cJSON_Root, MAX_TIME, 0);
	cJSON_AddNumberToObject(cJSON_Root, ACTION, CLOSE);
	cJSON_AddNumberToObject(cJSON_Root, MSG_SN, 0);
	cJSON_AddNumberToObject(cJSON_Root, EXECUTE_STATUS, ERR);

	return cJSON_Root;

}




cJSON* cJSON_Data_InitStartEvent(void)
{
	cJSON* cJSON_Root = NULL;
	char* p = NULL;

	cJSON_Root = cJSON_CreateObject();
	if (NULL == cJSON_Root)
	{
		return NULL;
	}
	
	cJSON_AddStringToObject(cJSON_Root, SYS_VERSION, DEFAULT_VER);	

	//cJSON_AddStringToObject(cJSON_Root, SYS_VERSION, DEFAULT_VER);

	return cJSON_Root;

}




cJSON* cJSON_Data_InitQuery(void)
{
	cJSON* cJSON_Root = NULL;
	char* p = NULL;

	cJSON_Root = cJSON_Data_InitChStatusRep();
	if (NULL == cJSON_Root)
	{
		return NULL;
	}

	cJSON_AddNumberToObject(cJSON_Root, MSG_SN, 0xFFFFFFFF);

	return cJSON_Root;
}


cJSON* cJSON_Data_InitCloseChInfo(void)
{
	cJSON* cJSON_Root = NULL;
	char* p = NULL;

	cJSON_Root = cJSON_Data_InitChStatusRep();
	if (NULL == cJSON_Root)
	{
		return NULL;
	}

	cJSON_AddNumberToObject(cJSON_Root, DETAIL, ERR_CLOSE);

	return cJSON_Root;
}

cJSON* cJSON_Data_InitChStatusRep(void)
{
	cJSON* cJSON_Root = NULL;
	char* p = NULL;

	cJSON_Root = cJSON_CreateObject();
	if (NULL == cJSON_Root)
	{
		return NULL;
	}
	
	cJSON_AddNumberToObject(cJSON_Root, CHANNEL_ID, CHANNEL_MAX + 1);	
	cJSON_AddNumberToObject(cJSON_Root, MAX_TIME, 0);
	cJSON_AddNumberToObject(cJSON_Root, CUR_TIME, 0);
	cJSON_AddNumberToObject(cJSON_Root, CHANNEL_STATUS, CLOSE);
	
	cJSON_AddNumberToObject(cJSON_Root, VOLT, VOLT_220V);
	cJSON_AddNumberToObject(cJSON_Root, CURRENT, 0);
	cJSON_AddNumberToObject(cJSON_Root, POWER, 0);
	cJSON_AddNumberToObject(cJSON_Root, ENERGY, 0);

	return cJSON_Root;
}


cJSON* cJSON_Data_InitChannelIdle(void)
{
	cJSON* cJSON_Root = NULL;
	char* p = NULL;

	cJSON_Root = cJSON_CreateObject();
	if (NULL == cJSON_Root)
	{
		return NULL;
	}
	
	cJSON_AddStringToObject(cJSON_Root, CHANNEL, IDLE);	

	return cJSON_Root;
}



uint8_t cJSON_Update(const cJSON * const object, const char * const string, void *d)
{
	cJSON* node = NULL;
	node = cJSON_GetObjectItem(object, string);
	if (node == NULL)
		return NULL;

	if (cJSON_IsBool(node))
	{
		int *b = (int*) d;
		cJSON_GetObjectItem(object, string)->type = *b ? cJSON_True : cJSON_False;
		return 1;
	}
	else if (cJSON_IsString(node))
	{
		cJSON_GetObjectItem(object, string)->valuestring = (char*) d;
		return 1;
	}
	else if (cJSON_IsNumber(node))
	{
		double *num = (double*) d;
		double x = *num;
//    cJSON_GetObjectItem(object,string)->valueint = (double)*num;
		cJSON_GetObjectItem(object, string)->valuedouble = (double) *num;

		return 1;
	}
	else
	{
		return 1;
	}
}


bool Json2Struct(cJSON *root, Msg_S* msg)
{
	cJSON *msgType = NULL;

	cJSON *channelID = NULL;
	cJSON *maxTime = NULL;
	cJSON *action = NULL;
	cJSON *sn = NULL;
	
	if (root == NULL)
	{
		return false;
	}
	
	msgType = cJSON_GetObjectItem(root, MSG_TYPE);
	if (msgType == NULL || !cJSON_IsNumber(msgType) )
	{
		return false;
	}
	channelID = cJSON_GetObjectItem(root, CHANNEL_ID);
	if (channelID == NULL|| !cJSON_IsNumber(channelID))
	{
		return false;
	}
	sn = cJSON_GetObjectItem(root, MSG_SN);
	if (sn == NULL || !cJSON_IsNumber(sn))
	{
		return false;
	}
	msg->msgSN = sn->valueint;
	msg->channelID = channelID->valueint;
	msg->msgType = msgType->valueint;
	if(msg->msgType == QUERY)
	{
		//查询任务，只需要这三个参数
		return true;
	}
	
	action = cJSON_GetObjectItem(root, ACTION);
	if (action == NULL || !cJSON_IsNumber(action))
	{
		return false;
	}
	msg->action = action->valueint;

	if(msg->action != ACTIVE )
	{
		//非开命令不需要时间
		return true;
	}
	
	maxTime = cJSON_GetObjectItem(root, MAX_TIME);	
	if (maxTime == NULL || !cJSON_IsNumber(maxTime))
	{
		return false;
	}

	
	msg->maxTime = maxTime->valueint;
	
	return true;
}



