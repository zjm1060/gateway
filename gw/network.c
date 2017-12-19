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

static void login(struct mqttObj *mo)
{
	char topic[64];
	char message[512];

	snprintf(topic,sizeof(topic),"Client/Collectors/%s",opts.did);
	snprintf(message,sizeof(message),"{\"id\":\"%s\",\"state\":\"OK\"}",opts.did);

	log_info("send login");
	if(mqtt_send(mo,topic,message,strlen(message)) == MQTTCLIENT_SUCCESS){
		opts.mqtt.lastSend = CurrentTime;
	}else{
		mqtt_stop(mo);
	}
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
//		.clientid = {"1234567890"},
//		.username = {"test"},
//		.passwd = {"test"},
//		.host = {"lhloao.com"},
//		.port = 1883
	};
	int rc;

	mo.clientid = opts.did;
	mo.host = opts.host;
	mo.username = opts.username;
	mo.passwd = opts.passwd;
	mo.port = opts.port;

	log_info("host: %s:%d",mo.host,mo.port);
	log_info("clientid:%s",mo.clientid);
	log_info("username:%s",mo.username);
	log_info("password:%s",mo.passwd);


	do{
		sleep(5);
		rc = mqtt_connect(&mo);
	}while(rc < 0);

	sleep(1);

	login(&mo);

	mqtt_subscribe(&mo,"Client/Server/timeSync",2,timeSync);

//	CreateThread("Node",Node_Task,&mo);

	while(1){
//		if((mqtt_state(&mo) == 0)
//#if defined(MQTT_TASK)
//		MutexLock(&mo.c.mutex);
//#endif
//		rc = MQTTYield(&mo.c, 500);
//#if defined(MQTT_TASK)
//		MutexUnlock(&mo.c.mutex);
//#endif
//		if(rc != MQTTCLIENT_SUCCESS){
//			log_info("closed by remote!");
//			mqtt_stop(&mo);
//			rc = mqtt_connect(&mo);
//			if(rc >= 0){
////				login(&mo);
////				onLine(&mo);
////				mqtt_subscribe(&mo,"dc/cs/set/all",QOS2,messageSet);
////				mqtt_subscribe(&mo,topic,QOS2,messageSet);
//				mqtt_subscribe(&mo,"Client/Server/timeSync",QOS2,timeSync);
//			}else{
//				mqtt_stop(&mo);
//			}
//		}
		if(!MQTTClient_isConnected(mo.c)){
			log_info("reconnect it...");
			do{
				sleep(5);
				rc = mqtt_connect(&mo);
			}while(rc < 0);
		}

		usleep(100*1000);

		if(abs(opts.mqtt.lastSend - CurrentTime) > 60){
			login(&mo);
		}
	}

	return NULL;
}
