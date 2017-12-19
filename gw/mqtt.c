/*
 * mqtt.c
 *
 *  Created on: 2017Äê8ÔÂ14ÈÕ
 *      Author: zjm09
 */

#include "framework/framework.h"
#include "mqtt.h"
#include "../ds.h"
#include "log.h"

static const char *mqtt_connect_error[] = {
	"Success",
	"Unsupported protocol version",
	"Unqualified client identity",
	"The server is not available",
	"Invalid username or password",
	"Unauthorized",
	"Reserved Error"
};

int mqtt_connect(struct mqttObj *mo)
{
	NetworkInit(&mo->n);

	mo->n.ifname = mo->ifname;

	int rc = NetworkConnect(&mo->n, mo->host, mo->port);
	if(rc < 0){
		printf("can't connect to %s:%d\n", mo->host, mo->port);
		return rc;
	}
	MQTTClientInit(&mo->c, &mo->n, 3000, mo->sendBuf, 1024, mo->recvBuf, 1024);

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

	data.keepAliveInterval = 300;
	data.MQTTVersion = 3;
	data.clientID.cstring = mo->clientid;
	data.username.cstring = mo->username;
	data.password.cstring = mo->passwd;

	data.cleansession = 1;

	if(mo->will){
		data.willFlag = 1;
		data.will.topicName.cstring = mo->will;
		data.will.message.cstring = mo->willData.data;
	}

	log_info("Connecting to MQTT broker %s:%d", mo->host, mo->port);
	rc = MQTTConnect(&mo->c, &data);
	log_info("Connected (%d) %s",
			rc,
			rc>=0?(rc>5?mqtt_connect_error[6]:mqtt_connect_error[rc]):"unkown error");

	return rc;
}

int mqtt_send(struct mqttObj *mo,char *topic,char *msg,size_t msg_len)
{
	MQTTMessage m;

	m.payload = msg;
	m.payloadlen = msg_len;
	m.retained = opts.mqtt.retained;
	m.qos = opts.mqtt.Qos;

	log_info("publish message to [%s]",topic);

	return MQTTPublish(&mo->c,topic,&m);
}

void mqtt_stop(struct mqttObj *mo)
{
	log_info("mqtt stop...");
	MQTTDisconnect(&mo->c);
	NetworkDisconnect(&mo->n);
}

void mqtt_subscribe(struct mqttObj *mo,char *topic,int qos,void (*cb)(MessageData *))
{
	log_info("subscribe: %s",topic);
	MQTTSubscribe(&mo->c,topic,qos,cb);
}

int mqtt_state(struct mqttObj *mo)
{
	return MQTTIsConnected(&mo->c);
}
