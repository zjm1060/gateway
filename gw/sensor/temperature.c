/*
 * temperature.c
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
	uint16_t myTemp;  // 自身温度
	uint16_t seTemp1; // 探头温度1
	uint16_t seTemp2;
	uint16_t seTemp3;
	uint8_t VBAT;  // 0.1V，后备电池电压
	int8_t Power; // 发射功率
	int8_t Signal; // 接收功率
	uint8_t CS;
	uint8_t End;
}__PACKED tempSensor_t;

void temp_proc(struct mqttObj *mo,Node *n,uint32_t addr,uint8_t *data)
{
	tempSensor_t *p = (void *)data;
	uint32_t t;

	log_info("--->temp_sensor :%08X",addr);

	opts.analysis.node.Signal = p->Signal;

	if(n){
		if(n->NodeState == Node_s_Lost){
			log_info("node state: Node_s_Normal");
			n->NodeState = Node_s_Normal;
			do_alarm(mo,n,alarm_lost_recover);
		}

	}
	else{
		n = addNode(addr);
		log_info("new node add-in");
		t = p->VBAT;
		n->analyze.battery = (t*1.0)/10;
		n->Config.deviceType = type_temp_sensor;
		do_alarm(mo,n,alarm_node_new);
	}

	t = p->seTemp1;
	n->Data.D50H.temp = (t*1.0)/100.0;
	t = p->VBAT;
	n->analyze.battery = (t*1.0)/10;

	do_alarm(mo,n,alarm_normal);

	n->NodeState = Node_s_Normal;
	n->lastTime = time(0);
}
