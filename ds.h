/*
 * ds.h
 *
 *  Created on: 2017��12��15��
 *      Author: zjm09
 */

#ifndef DS_H_
#define DS_H_

#include "MQTTClient.h"
#include "gw/qr-coder/QR_Encode.h"

typedef struct{
	char did[32];
	char username[64];
	char passwd[64];
	char host[128];
	int port;
	struct{
		struct{
			uint32_t currentAddress;
			int Signal;
		}node;
	}analysis;
	struct{
		int retained;
		enum QoS Qos;
		time_t lastSend;
	}mqtt;
	struct{
		char data[MAX_BITDATA];
		int width;
		char url[1024];
	}qr;
}deviceOpts;

extern  deviceOpts opts;

#endif /* DS_H_ */
