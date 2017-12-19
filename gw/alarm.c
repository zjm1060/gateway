/*
 * alarm.c
 *
 *  Created on: 2017��12��15��
 *      Author: zjm09
 */
#include "includes.h"
#include "node.h"
#include "mqtt.h"
#include "alarm.h"
#include "ds.h"

const char *alarm_text[alarm_all] = {
	[0] = "{"
			"\"did\":\"%DEVICE_ID\","
			"\"id\":\"%NODE_ADDR\","
			"\"type\":%NODE_TYPE,"
			"\"info\":0,"
			"\"state\":[%NODE_VOLSTATE],"
			"\"timestamp\":%TIMESTAMP,"
			"\"date\":\"%DATE\","
			"\"time\":\"%TIME\""
		"}",
	[1] = "{"
			"\"did\":\"%DEVICE_ID\","
			"\"id\":\"%NODE_ADDR\","
			"\"type\":%NODE_TYPE,"
			"\"info\":1,"
			"\"state\":[%NODE_VOLSTATE],"
			"\"timestamp\":%TIMESTAMP,"
			"\"date\":\"%DATE\","
			"\"time\":\"%TIME\""
		"}",
	[2] = "{"
			"\"did\":\"%DEVICE_ID\","
			"\"id\":\"%NODE_ADDR\","
			"\"type\":%NODE_TYPE,"
			"\"info\":2,"
			"\"state\":[%NODE_VOLSTATE],"
			"\"timestamp\":%TIMESTAMP,"
			"\"date\":\"%DATE\","
			"\"time\":\"%TIME\""
		"}",
	[3] = "{"
			"\"did\":\"%DEVICE_ID\","
			"\"id\":\"%NODE_ADDR\","
			"\"type\":%NODE_TYPE,"
			"\"info\":3,"
			"\"state\":[%NODE_VOLSTATE],"
			"\"timestamp\":%TIMESTAMP,"
			"\"date\":\"%DATE\","
			"\"time\":\"%TIME\""
		"}",
	[4] = "{"
			"\"did\":\"%DEVICE_ID\","
			"\"id\":\"%NODE_ADDR\","
			"\"type\":%NODE_TYPE,"
			"\"info\":4,"
			"\"timestamp\":%TIMESTAMP,"
			"\"date\":\"%DATE\","
			"\"time\":\"%TIME\""
		"}",
};

void vprint(char *buffer,const char *format,void *p);

void do_alarm(struct mqttObj *mo,Node *node,alarm_t t)
{
	char topic[64];
	char message[512];

	memset(topic,0,sizeof(topic));
	memset(message,0,sizeof(message));

	snprintf(topic,sizeof(topic),"Client/Node/%08X",node->Config.address);

	vprint(message,alarm_text[t],node);

	if(mqtt_send(mo,topic,message,strlen(message)) != FAILURE){
		opts.mqtt.lastSend = CurrentTime;
	}else{
		mqtt_stop(mo);
	}
}

