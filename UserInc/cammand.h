/*
 * cammand.h
 *
 *  Created on: May 18, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */

#ifndef USERINC_CAMMAND_H_
#define USERINC_CAMMAND_H_

#include "system.h"

void queryTask(void *pvParameters);
void activeTask(void *pvParameters);
bool closeChannel(uint32_t channelId);
void creatTimer();
#endif /* USERINC_CAMMAND_H_ */
