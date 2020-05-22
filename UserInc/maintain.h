/*
 * analysis.h
 *
 *  Created on: May 19, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */
#include "system.h"




ChannelStatus_S getChannelStatus(uint32_t channelID);

void uploadCloseInfoTask(void *pvParameters);
void maintainTask(void *pvParameters);
void maintainOpenTask(void *pvParameters);
void maintainHlwTask(void *pvParameters);
void periRepTask(void *pvParameters);
void creatTimer();