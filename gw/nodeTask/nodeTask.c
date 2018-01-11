/*
 * nodeTask.c
 *
 *  Created on: 2017Äê12ÔÂ15ÈÕ
 *      Author: zjm09
 */
#include "includes.h"
#include "../../ds.h"
#include "../node.h"
#include "../mqtt.h"
#include "../serial.h"
#include "../alarm.h"
#include "log.h"

typedef struct{
	uint8_t start;
	uint8_t T;
	uint32_t addr;
	uint8_t C;
	uint8_t L;
	uint8_t data[0];
}__PACKED tHeader;

typedef struct{
	uint8_t CS;
	uint8_t end;
}__PACKED tTial;

void onOff_proc(struct mqttObj *mo,Node *n,uint32_t addr,uint8_t *data);
void temp_proc(struct mqttObj *mo,Node *n,uint32_t addr,uint8_t *data);


void *Node_Task(void *args)
{
	pThreadData lpthis = args;

	struct mqttObj *mo = lpthis->PrivateData;

	char recvBuffer[512];
	struct serial_conf ser = {
		.dev = "/dev/ttyS2",
		.baud = B115200,
		.parity = 'n'
	};

	int sfd = serial_open(&ser);
	if(sfd == -1){
		exit(2);
	}

	while(1){
		struct list_head *i;
		tHeader *h = (void *)recvBuffer;
		int size = serial_read(sfd,h,sizeof(tHeader),1000);
		if(size == sizeof(tHeader)){
			if(h->start == 0x68){
				log_info("--->node :%08X",h->addr);
				void *__p = (void *)h->data;
				size = serial_read(sfd,__p,h->L+2,1000);
				if(size == h->L+2){
					// TODO checksum
				}

				log_dump(h,size+sizeof(tHeader));

				opts.analysis.node.currentAddress = h->addr;
				Node *n = isNodeExist(h->addr);
				switch(h->T){
					case type_power_failure_1:{
							onOff_proc(mo,n,h->addr,(void *)__p);
						}break;
					case type_temp_sensor:{
							temp_proc(mo,n,h->addr,(void *)__p);
						}break;
					default:
						break;
				}
			}
		}

		list_for_each(i,NodeList()){
			Node *n = (void *)i;
			if(n->NodeState == Node_s_Normal && abs(n->lastTime - time(0)) >= 3600){
				log_info("[%08X] node state: Node_s_Lost",n->Config.address);
				n->NodeState = Node_s_Lost;
				do_alarm(mo,n,alarm_lost);
			}
		}


	}
}

