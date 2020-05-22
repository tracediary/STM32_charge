/*
 * network.h
 *
 *  Created on: May 18, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */

#ifndef USERINC_NETWORK_H_
#define USERINC_NETWORK_H_

#include "system.h"

void mqttRecTask(void *pvParameters);
void mqttSendTask(void *pvParameters);

#endif /* USERINC_NETWORK_H_ */
