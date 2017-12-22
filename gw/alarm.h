/*
 * alarm.h
 *
 *  Created on: 2017Äê12ÔÂ15ÈÕ
 *      Author: zjm09
 */

#ifndef GW_ALARM_H_
#define GW_ALARM_H_

typedef enum{
	alarm_normal = 0,
	alarm_abnormal = 1,
	alarm_lost = 2,
	alarm_lost_recover = 3,
	alarm_node_new = 4,
	alarm_all
}alarm_t;

void do_alarm(struct mqttObj *mo,Node *node,alarm_t t);

#endif /* GW_ALARM_H_ */
