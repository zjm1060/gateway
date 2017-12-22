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

static char isTopicMatched(char* topicFilter, const char* topicName)
{
    char* curf = topicFilter;
    char* curn = topicName;
    char* curn_end = curn + strlen(topicName);//->lenstring.len;

    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+')
        {   // skip until we meet the next separator, or end of string
            char* nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;    // skip until end of string
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}

static int MQTTPacket_equals(const char * a, char* bptr)
{
	int alen = 0,
		blen = 0;
	char *aptr;

	aptr = a;
	alen = strlen(a);
	blen = strlen(bptr);

	return (alen == blen) && (strncmp(aptr, bptr, alen) == 0);
}

static int deliverMessage(struct mqttObj * c, const char * topicName, MQTTClient_message* message)
{
    int i;
    int rc = MQTTCLIENT_FAILURE;

    // we have to find the right message handler - indexed by topic
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != 0 && (MQTTPacket_equals(topicName, (char*)c->messageHandlers[i].topicFilter) ||
                isTopicMatched((char*)c->messageHandlers[i].topicFilter, topicName)))
        {
            if (c->messageHandlers[i].fp != NULL)
            {
                c->messageHandlers[i].fp(message);
                rc = MQTTCLIENT_SUCCESS;
            }
        }
    }

    return rc;
}


int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message)
{
	struct mqttObj *mo = context;
	return deliverMessage(mo,topicName,message);
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
}

int mqtt_send(struct mqttObj *mo,char *topic,char *msg,size_t msg_len)
{
	int rc = MQTTCLIENT_FAILURE;
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
}


int MQTTSetMessageHandler(struct mqttObj* c, const char* topicFilter, messageHandler messageHandler)
{
    int rc = MQTTCLIENT_FAILURE;
    int i = -1;

    /* first check for an existing matching slot */
    for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
    {
        if (c->messageHandlers[i].topicFilter != NULL && strcmp(c->messageHandlers[i].topicFilter, topicFilter) == 0)
        {
            if (messageHandler == NULL) /* remove existing */
            {
                c->messageHandlers[i].topicFilter = NULL;
                c->messageHandlers[i].fp = NULL;
            }
            rc = MQTTCLIENT_SUCCESS; /* return i when adding new subscription */
            break;
        }
    }
    /* if no existing, look for empty slot (unless we are removing) */
    if (messageHandler != NULL) {
        if (rc == MQTTCLIENT_FAILURE)
        {
            for (i = 0; i < MAX_MESSAGE_HANDLERS; ++i)
            {
                if (c->messageHandlers[i].topicFilter == NULL)
                {
                    rc = MQTTCLIENT_SUCCESS;
                    break;
                }
            }
        }
        if (i < MAX_MESSAGE_HANDLERS)
        {
            c->messageHandlers[i].topicFilter = topicFilter;
            c->messageHandlers[i].fp = messageHandler;
        }
    }
    return rc;
}


void mqtt_subscribe(struct mqttObj *mo,char *topic,int qos,void (*cb)(MQTTClient_message *))
{
	log_info("subscribe: %s",topic);

	if(MQTTClient_subscribe(mo->c,topic,qos) == MQTTCLIENT_SUCCESS){
		MQTTSetMessageHandler(mo,topic,cb);
	}
}

int mqtt_state(struct mqttObj *mo)
{
//	return MQTTIsConnected(&mo->c);
	return 0;
}
