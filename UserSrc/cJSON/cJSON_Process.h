#ifndef _CJSON_PROCESS_H_
#define _CJSON_PROCESS_H_
#include <cJSON.h>
#include "stdint.h"
#include "system.h"





#define   UPDATE_SUCCESS       1
#define   UPDATE_FAIL          0

cJSON* cJSON_Data_Init(void);
uint8_t cJSON_Update(const cJSON * const object,const char * const string,void * d);
void Proscess(void* data);

cJSON* cJSON_Data_InitActive(void);
cJSON* cJSON_Data_InitQuery(void);
cJSON* cJSON_Data_InitCloseChInfo(void);
cJSON* cJSON_Data_InitChStatusRep(void);
cJSON* cJSON_Data_InitChannelIdle(void);
cJSON* cJSON_Data_InitStartEvent(void);

bool Json2Struct(cJSON *root, Msg_S* msg);




#endif

