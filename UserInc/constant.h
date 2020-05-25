/*
 * constant.h
 *
 *  Created on: May 18, 2020
 *      Author: Jason C
 *     version: V1.0.1
 ************************************************************************
 *	 attention: GPLv3, free software
 */

#ifndef USERINC_CONSTANT_H_
#define USERINC_CONSTANT_H_



/******common define*********/
#define DELAY_BASE_10MIL_TIME			10
#define DELAY_BASE_SEC_TIME				(DELAY_BASE_10MIL_TIME * 100)
#define DELAY_BASE_MIN_TIME				(DELAY_BASE_SEC_TIME * 60)
#define NORMAL_BUF_SIZE					50
#define NORMAL_REPEAT_TIME				3

#define MIN_COLLECT_TIME				100


/*********MQTT define start*************************************************/
#define DEFAULT_MQTT_USERNAME			"username"
#define DEFAULT_MQTT_PWD				"pwd"

/**********topic define**********************************/
#define TOPIC_CODE_LEN					3
#define TOPIC_NUM						6

#define MULTI_TOPIC						1

#define SYS_START						"100"
#define SYS_IDLE						"101"

#define CLOSE_INFO						"301"
#define PERIOD							"302"


#define QUERY_CHANNEL_ACK				"501"
#define SET_CHANNEL_ACK					"601"


#define SYS_START_SERIAL				0
#define SYS_IDLE_SERIAL					1

#define CLOSE_INFO_SERIAL				2
#define PERIOD_SERIAL					3


#define QUERY_CHANNEL_ACK_SERIAL		4
#define SET_CHANNEL_ACK_SERIAL			5






/**********topic define end**********************************/
/*********JSON string define start******************************************/
//common define
#define SYS_VERSION						"version"
#define DEFAULT_VER						"v1.0.1"


#define MSG_TYPE						"msgType"
#define CHANNEL_ID						"channelID"
#define MSG_SN							"msgSN"
#define MAX_TIME						"maxTime"
#define CHANNEL_STATUS					"channelStatus"
#define VOLT_220V						220
#define MIN_CURRENT						0.1

//active msg define
#define ACTION							"action"
#define EXECUTE_STATUS					"executeStatus"

//query msg define
#define VOLT							"volt"
#define CURRENT							"current"
#define POWER							"power"
#define ENERGY							"energy"
#define CUR_TIME						"curTime"

//close channel define
#define DETAIL							"detail"
#define CHANNEL							"channel"
#define IDLE							"idle"
/*********JSON string define end******************************************/

/*********network string define start*************************************/
#define MQTT_RX_BUF_SIZE				(NORMAL_BUF_SIZE * 6)
#define MQTT_TX_BUF_SIZE				MQTT_RX_BUF_SIZE

#define MQTT_UART						USART1
#define MQTT_UART_WAITTIME				(DELAY_BASE_10MIL_TIME * 50)

#define EVENTBIT_MQTT_UART_REC			(1<<0)
#define EVENTBIT_MQTT_UART_TC			(1<<1)
/*********network string define end*************************************/

/**MQTT define end*****************************************************/


/********HLW8032 define start******************************************/

#define CHANNEL_MAX						10
#define SWITCH_MAX						4

#define EVENTBIT_CHANNEL_1				(1<<0)
#define EVENTBIT_CHANNEL_2				(1<<1)
#define EVENTBIT_CHANNEL_3				(1<<2)
#define EVENTBIT_CHANNEL_4				(1<<3)
#define EVENTBIT_CHANNEL_5				(1<<4)
#define EVENTBIT_CHANNEL_6				(1<<5)
#define EVENTBIT_CHANNEL_7				(1<<6)
#define EVENTBIT_CHANNEL_8				(1<<7)
#define EVENTBIT_CHANNEL_9				(1<<8)
#define EVENTBIT_CHANNEL_10				(1<<9)

#define EVENTBIT_CHANNEL				(EVENTBIT_CHANNEL_1 \
										|EVENTBIT_CHANNEL_2 \
										|EVENTBIT_CHANNEL_3 \
										|EVENTBIT_CHANNEL_4 \
										|EVENTBIT_CHANNEL_5 \
										|EVENTBIT_CHANNEL_6 \
										|EVENTBIT_CHANNEL_7 \
										|EVENTBIT_CHANNEL_8 \
										|EVENTBIT_CHANNEL_9 \
										|EVENTBIT_CHANNEL_10)

#define EVENTBIT_HLW_UART_REC			(1<<10)

#define HLW_UART						USART4
#define HLW_WAITTIME					(DELAY_BASE_10MIL_TIME * 50)

#define MAINTAIN_PERIDIC_REPORT			(DELAY_BASE_MIN_TIME * 15)



/********HLW8032 define end******************************************/
/**********DEBUG UART3 define start************************************/
#define DEBUG_RX_BUF_SIZE				(NORMAL_BUF_SIZE * 5)
#define DEBUG_TX_BUF_SIZE				DEBUG_RX_BUF_SIZE

#define EVENTBIT_DEBUG_UART_REC			(1<<0)
#define EVENTBIT_DEBUG_UART_TC			(1<<1)
/**********DEBUG UART3 define end**************************************/

/**********system define start*****************************************/

#define EVENTBIT_SYS_SYNC_TIME			(1<<0)
#define EVENTBIT_SYS_MQTT_REC_TASK		(1<<1)
#define EVENTBIT_SYS_MQTT_SEND_TASK		(1<<2)
#define EVENTBIT_SYS_ACTIVE_TASK		(1<<3)
#define EVENTBIT_SYS_QUERY_TASK			(1<<4)
#define EVENTBIT_SYS_MAINTAIN_TASK		(1<<5)
#define EVENTBIT_SYS_MT_OPEN_TASK		(1<<6)
#define EVENTBIT_SYS_UP_CL_INFO_TASK	(1<<7)
#define EVENTBIT_SYS_MT_HLW_TASK		(1<<8)
#define EVENTBIT_SYS_PERI_REP_TASK		(1<<9)

#define EVENTBIT_SYS_TASK				(EVENTBIT_SYS_MQTT_REC_TASK \
										|EVENTBIT_SYS_MQTT_SEND_TASK \
										|EVENTBIT_SYS_ACTIVE_TASK \
										|EVENTBIT_SYS_QUERY_TASK \
										|EVENTBIT_SYS_MAINTAIN_TASK \
										|EVENTBIT_SYS_MT_OPEN_TASK \
										|EVENTBIT_SYS_UP_CL_INFO_TASK \
										|EVENTBIT_SYS_MT_HLW_TASK \
										|EVENTBIT_SYS_PERI_REP_TASK)

#define BASIC_QUEUE_LEN					5

#define ACTIVE_QUEUE					BASIC_QUEUE_LEN
#define QUERY_QUEUE						BASIC_QUEUE_LEN
#define CLOSE_INFO_QUEUE				BASIC_QUEUE_LEN
#define OPEN_INFO_QUEUE					BASIC_QUEUE_LEN

#define MSG_MAX							BASIC_QUEUE_LEN
#define SEND_MQTT_QUEUE					BASIC_QUEUE_LEN

#endif /* USERINC_CONSTANT_H_ */
