/*
 * onOff.c
 *
 *  Created on: 2018年1月11日
 *      Author: zjm09
 */
#include "includes.h"
#include "ds.h"
#include "../mqtt.h"
#include "../node.h"
#include "../alarm.h"
#include "log.h"

typedef struct{
	uint8_t SER;
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
}__PACKED onOff_t;

void onOff_proc(struct mqttObj *mo,Node *n,uint32_t addr,uint8_t *data)
{
	onOff_t *p = (void *)data;
	log_info("--->onOff_sensor :%08X",addr);
	opts.analysis.node.Signal = p->Signal;
	if(n){
		n->analyze.battery = p->VBAT*1.0/10;
		if(n->NodeState == Node_s_Lost){
			log_info("node state: Node_s_Normal");
			n->NodeState = Node_s_Normal;
			do_alarm(mo,n,alarm_lost_recover);
		}
		log_info("ST:%02X,LineState:%02X,bitMask:%02X",p->ST,n->Data.D49H.LineState.state,n->Data.D49H.bitMask);
		if((p->ST&n->Data.D49H.bitMask) != n->Data.D49H.LineState.state){
			log_info("node state changed");
			n->Data.D49H.LineState.state = p->ST&n->Data.D49H.bitMask;
			if(p->ST&n->Data.D49H.bitMask){
				do_alarm(mo,n,alarm_abnormal);
			}else{
				do_alarm(mo,n,alarm_normal);
			}
		}
	}else{
		n = addNode(addr);
		log_info("new node add-in");
		n->analyze.battery = p->VBAT*1.0/10;
		n->Config.deviceType = type_power_failure_1;
		n->Data.D49H.bitMask = 0x0EU;
		do_alarm(mo,n,alarm_node_new);
		if(p->ST&n->Data.D49H.bitMask){
			n->Data.D49H.LineState.state = p->ST&n->Data.D49H.bitMask;
			log_info("node state is abnormal");
			do_alarm(mo,n,alarm_abnormal);
		}
	}

	n->NodeState = Node_s_Normal;
	n->lastTime = time(0);
}
