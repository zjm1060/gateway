/*
 * network.c
 *
 *  Created on: 2017Äê12ÔÂ14ÈÕ
 *      Author: zjm09
 */

#include "includes.h"
#include "mqtt.h"
#include "ds.h"
#include "log.h"

void *Node_Task(void *args);

static int lostPower = 0;

void powerFailure(void)
{
	log_info("power lost");
	lostPower = 1;
}


static void login(struct mqttObj *mo)
{
	char topic[64];
	char message[512];

	snprintf(topic,sizeof(topic),"Client/Collectors/%s",opts.did);
	snprintf(message,sizeof(message),"{\"id\":\"%s\",\"state\":\"OK\"}",opts.did);

	log_info("send login");
//	if(mqtt_send(mo,topic,message,strlen(message)) == MQTTCLIENT_SUCCESS){
//		opts.mqtt.lastSend = CurrentTime;
//	}else{
//		mqtt_stop(mo);
//	}

	mqttSend(mo,topic,message,strlen(message));
}

static void poweroff(struct mqttObj *mo)
{
	char topic[64];
	char message[512];

	snprintf(topic,sizeof(topic),"Client/Collectors/%s",opts.did);
	snprintf(message,sizeof(message),"{\"id\":\"%s\",\"state\":\"poweroff\"}",opts.did);

	log_info("send login");
//	if(mqtt_send(mo,topic,message,strlen(message)) == MQTTCLIENT_SUCCESS){
//		opts.mqtt.lastSend = CurrentTime;
//	}else{
//		mqtt_stop(mo);
//	}

	mqttSend(mo,topic,message,strlen(message));
}

static void timeSync(MQTTClient_message* md)
{
	MQTTClient_message* message = md->payload;
	struct json *J;
	int error;

	J = json_open(JSON_F_NONE, &error);
	error = json_loadbuffer(J,(char*)message->payload,(int)message->payloadlen);
	if(!error){
		time_t t = json_number(J,"timestamp");

		if(t && (abs(t-CurrentTime) > 60)){
			stime(&t);
		}
	}

	json_close(J);
}

void *network(void *args)
{
	struct mqttObj mo = {
		.ifname = "ppp0",
	};
	int rc;
//	char topic_will[64];
//	char will[64];

	mo.clientid = "test_abc";
	mo.host = "ssl://192.168.1.53:8883";
	mo.username = opts.username;
	mo.passwd = opts.passwd;
	mo.port = opts.port;
	mo.ssl.enable = opts.ssl.enable;

//	snprintf(topic_will,sizeof(topic_will),"Client/Collectors/%s",opts.did);
//	mo.will = topic_will;
//	mo.willData = "{}";

	log_info("host: %s",mo.host);
	log_info("clientid:%s",mo.clientid);
	log_info("username:%s",mo.username);
	log_info("password:%s",mo.passwd);


	do{
		sleep(5);
		rc = mqtt_connect(&mo);
	}while(rc < 0);

	sleep(1);

//	login(&mo);

	mqtt_subscribe(&mo,"Client/Server/timeSync",2,timeSync);

//	CreateThread("Node",Node_Task,&mo);

	while(1){
		if(!MQTTClient_isConnected(mo.c)){
			log_info("reconnect it...");
			do{
				sleep(5);
				rc = mqtt_connect(&mo);
			}while(rc < 0);

			mqtt_subscribe(&mo,"Client/Server/timeSync",2,timeSync);
		}

		if(abs(opts.mqtt.lastSend - CurrentTime) > 600){
			login(&mo);
		}

		if(lostPower){
//			do_alarm(mo,n,alarm_lost);
			poweroff(&mo);
//			system("poweroff");
//			gpio_set(GPIO_SYS_CTL,0);

			exit(0);
		}

		mqttQueueSend(&mo);

		usleep(500*1000);
	}

	return NULL;
}
