/*
 * mqtt.h
 *
 *  Created on: 2017��8��14��
 *      Author: zjm09
 */

#ifndef MQTT_H_
#define MQTT_H_

#include <gw/paho.mqtt/inc/MQTTClient.h>

struct mqttObj{
	MQTTClient c;
//	Network n;
	const char *ifname;
	const char *will;
	const char *willData;
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
void mqtt_subscribe(struct mqttObj *mo,char *topic,int qos,void (*cb)(MQTTClient_message *));

#endif /* MQTT_H_ */
