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

void mqtt_lost(void* context, char* cause)
{
	struct mqttObj *mo = context;

	log_info("close by remote");

	mqtt_stop(mo);

//	mqtt_connect(mo);
}

int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message)
{

}

int mqtt_connect(struct mqttObj *mo)
{
	MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
	MQTTClient_willOptions wopts = MQTTClient_willOptions_initializer;

	int rc = 0;

	log_info("Connecting to MQTT broker %s:%d", mo->host, mo->port);
	rc = MQTTClient_create(&mo->c, mo->host, mo->clientid,
				MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);

	if (rc != MQTTCLIENT_SUCCESS)
	{
		MQTTClient_destroy(&mo->c);
		return rc;
	}

	MQTTClient_setCallbacks(mo->c,mo,mqtt_lost,messageArrived,NULL);

	opts.keepAliveInterval = 300;
	opts.cleansession = 1;
	opts.MQTTVersion = MQTTVERSION_3_1_1;
	opts.username = mo->username;
	opts.password = mo->passwd;


	if(mo->will){
		opts.will = &wopts;
		opts.will->message = mo->willData;
		opts.will->qos = 1;
		opts.will->retained = 0;
		opts.will->topicName = mo->will;
	}

	rc = MQTTClient_connect(mo->c, &opts);
	if (rc != MQTTCLIENT_SUCCESS){
		MQTTClient_destroy(&mo->c);
		return rc;
	}

	return rc;
#if 0
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
#endif
	return rc;
}

int mqtt_send(struct mqttObj *mo,char *topic,char *msg,size_t msg_len)
{
#if 0
	MQTTMessage m;

	m.payload = msg;
	m.payloadlen = msg_len;
	m.retained = opts.mqtt.retained;
	m.qos = opts.mqtt.Qos;

	log_info("publish message to [%s]",topic);

	return MQTTPublish(&mo->c,topic,&m);
#endif
	int rc;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken dt;

	pubmsg.payload = msg;
	pubmsg.payloadlen = msg_len;
	pubmsg.qos = opts.mqtt.Qos;
	pubmsg.retained = opts.mqtt.retained;

	rc = MQTTClient_publishMessage(mo->c, topic, &pubmsg, &dt);
	if(rc != MQTTCLIENT_SUCCESS)
		return rc;

	if (pubmsg.qos > 0)
	{
		rc = MQTTClient_waitForCompletion(mo->c, dt, 5000L);
	}

	return rc;
}

void mqtt_stop(struct mqttObj *mo)
{
	log_info("mqtt stop...");
	MQTTClient_destroy(&mo->c);
//	NetworkDisconnect(&mo->n);
}

void mqtt_subscribe(struct mqttObj *mo,char *topic,int qos,void (*cb)(MQTTClient_message *))
{
	log_info("subscribe: %s",topic);

	MQTTClient_subscribe(mo->c,topic,qos);
}

int mqtt_state(struct mqttObj *mo)
{
//	return MQTTIsConnected(&mo->c);
	return 0;
}
