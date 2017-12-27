/*
 * mqtt.h
 *
 *  Created on: 2017Äê8ÔÂ14ÈÕ
 *      Author: zjm09
 */

#ifndef MQTT_H_
#define MQTT_H_

#include "list.h"
#include "MQTT/MQTTClient.h"

#define MAX_MESSAGE_HANDLERS	(16)

typedef void (*messageHandler)(MQTTClient_message *);

typedef struct __sendQueue{
	struct __sendQueue *next;
	char topic[64];
	char message[512];
	size_t msg_len;
	time_t timestamp;
}mqttMessgae;

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

	struct{
		int enable;
	}ssl;

    struct MessageHandlers
    {
        const char* topicFilter;
        void (*fp) (MQTTClient_message*);
    } messageHandlers[MAX_MESSAGE_HANDLERS];

    pthread_mutex_t mutex;
    mqttMessgae *msg;
    mqttMessgae **msgAdd;
};


int mqtt_connect(struct mqttObj *mo);
void mqtt_stop(struct mqttObj *mo);
int mqtt_send(struct mqttObj *mo,char *topic,char *msg,size_t msg_len);
int mqtt_state(struct mqttObj *mo);
void mqtt_subscribe(struct mqttObj *mo,char *topic,int qos,void (*cb)(MQTTClient_message *));
int mqttQueueSend(struct mqttObj *mo);
int mqttSend(struct mqttObj *mo,char *topic,char *msg,size_t msg_len);

#endif /* MQTT_H_ */
