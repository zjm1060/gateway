/*
 * mqtt.h
 *
 *  Created on: 2017Äê8ÔÂ14ÈÕ
 *      Author: zjm09
 */

#ifndef MQTT_H_
#define MQTT_H_

#include "MQTTClient.h"

struct mqttObj{
	MQTTClient c;
	Network n;
	const char *ifname;
	const char *will;
	MQTTLenString willData;
	unsigned char sendBuf[1024];
	unsigned char recvBuf[1024];
	char *clientid;
	char *username;
	char *passwd;
	char *host;
	int port;
};

int mqtt_connect(struct mqttObj *mo);
void mqtt_stop(struct mqttObj *mo);
int mqtt_send(struct mqttObj *mo,char *topic,char *msg,size_t msg_len);
int mqtt_state(struct mqttObj *mo);
void mqtt_subscribe(struct mqttObj *mo,char *topic,int qos,void (*cb)(MessageData *));

#endif /* MQTT_H_ */
