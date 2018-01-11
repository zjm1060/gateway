/*
 * ds.h
 *
 *  Created on: 2017Äê12ÔÂ15ÈÕ
 *      Author: zjm09
 */

#ifndef DS_H_
#define DS_H_

#include "MQTT/MQTTClient.h"
#include "gw/qr-coder/QR_Encode.h"

typedef struct{
	char did[32];
	char username[64];
	char passwd[64];
	char host[128];
	int port;
	struct{
		int enable;
		char CAFile[256];
		char clientKeyFile[256];
		char clientPassFile[256];
		char clientKeyPassword[256];
	}ssl;
	struct{
		struct{
			uint32_t currentAddress;
			int Signal;
		}node;
	}analysis;
	struct{
		int retained;
		int Qos;
		time_t lastSend;
	}mqtt;
	struct{
		char data[MAX_BITDATA];
		int width;
		char url[1024];
	}qr;
}deviceOpts;

extern  deviceOpts opts;

#define __PACKED __attribute__((__packed__))

#endif /* DS_H_ */
