/*
 * mqtt.h
 *
 *  Created on: 2017Äê8ÔÂ14ÈÕ
 *      Author: zjm09
 */

#ifndef MQTT_H_
#define MQTT_H_

#include <gw/paho.mqtt/inc/MQTTClient.h>

#define MAX_MESSAGE_HANDLERS	(16)

typedef void (*messageHandler)(MQTTClient_message *);

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

    struct MessageHandlers
    {
        const char* topicFilter;
        void (*fp) (MQTTClient_message*);
    } messageHandlers[MAX_MESSAGE_HANDLERS];
};

int mqtt_connect(struct mqttObj *mo);
void mqtt_stop(struct mqttObj *mo);
int mqtt_send(struct mqttObj *mo,char *topic,char *msg,size_t msg_len);
int mqtt_state(struct mqttObj *mo);
void mqtt_subscribe(struct mqttObj *mo,char *topic,int qos,void (*cb)(MQTTClient_message *));

#endif /* MQTT_H_ */
