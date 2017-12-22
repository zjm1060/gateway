/*
 * nodeTask.c
 *
 *  Created on: 2017年12月15日
 *      Author: zjm09
 */
#include "includes.h"
#include "../../ds.h"
#include "../node.h"
#include "../mqtt.h"
#include "../serial.h"
#include "../alarm.h"
#include "log.h"

#pragma pack(1)
typedef struct{
	uint8_t start;
	uint8_t T;
	uint32_t addr;
	uint8_t C;
	uint8_t L;
	uint8_t SER;
	uint8_t data[0];
}tHeader;

typedef struct{
	uint16_t ST;
	uint8_t A_VOLT[4]; //
	uint8_t B_VOLT[4];
	uint8_t C_VOLT[4];
	uint8_t A_CURR[4]; //
	uint8_t B_CURR[4];
	uint8_t C_CURR[4];
	uint8_t VBAT;  // 0.1V，后备电池电压
	int8_t Power; // 发射功率
	int8_t Signal; // 接收功率
	uint8_t CS;
	uint8_t End;
}tPackage;

typedef struct{
	uint8_t CS;
	uint8_t end;
}tTial;
#pragma pack()

static int lostPower = 0;

void powerFailure(void)
{
	log_info("power lost");

//	system("poweroff");
//
//	gpio_set(GPIO_SYS_CTL,0);
}

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
				switch(h->T){
					case type_power_failure_1:{
							log_info("node :%08X",h->addr);
							tPackage *p = (void *)h->data;
							size = serial_read(sfd,p,sizeof(tPackage),1000);
							log_dump(h,size+sizeof(tHeader));
							if(size == sizeof(tPackage)){
								// TODO checksum
								opts.analysis.node.currentAddress = h->addr;
								opts.analysis.node.Signal = p->Signal;
								Node *n = isNodeExist(h->addr);
								if(n){
									if(n->Data.NodeState == Node_s_Lost){
										log_info("node state: Node_s_Normal");
										n->Data.NodeState = Node_s_Normal;
										do_alarm(mo,n,alarm_lost_recover);
									}
									log_info("ST:%02X,LineState:%02X",p->ST,n->Data.LineState.state);
									if((p->ST&n->Config.bitMask) != n->Data.LineState.state){
										log_info("node state changed");
										n->Data.LineState.state = p->ST&n->Config.bitMask;
										if(p->ST&n->Config.bitMask){
											do_alarm(mo,n,alarm_abnormal);
										}else{
											do_alarm(mo,n,alarm_normal);
										}
									}
								}else{
									n = addNode(h->addr);
									log_info("new node add-in");
									n->Config.deviceType = type_power_failure_1;
									n->Config.bitMask = 0x0EU;
									do_alarm(mo,n,alarm_node_new);
									if(p->ST&n->Config.bitMask){
										n->Data.LineState.state = p->ST&n->Config.bitMask;
										log_info("node state is abnormal");
										do_alarm(mo,n,alarm_abnormal);
									}
								}

								n->Data.NodeState = Node_s_Normal;
								n->Data.lastTime = time(0);
							}
						}break;
					default:
						break;
				}
			}
		}

		list_for_each(i,NodeList()){
			Node *n = (void *)i;
			if(n->Data.NodeState == Node_s_Normal && abs(n->Data.lastTime - time(0)) >= 3600){
				log_info("[%08X] node state: Node_s_Lost",n->Config.address);
				n->Data.NodeState = Node_s_Lost;
				do_alarm(mo,n,alarm_lost);
			}
		}

		if(lostPower){
//			do_alarm(mo,n,alarm_lost);
			mqtt_stop(mo);
			system("poweroff");
			gpio_set(GPIO_SYS_CTL,0);
		}
	}
}

