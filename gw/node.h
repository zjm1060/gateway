/*
 * Node.h
 *
 *  Created on: 2017Äê12ÔÂ14ÈÕ
 *      Author: zjm09
 */

#ifndef GW_NODE_H_
#define GW_NODE_H_

#include <stdint.h>
#include <time.h>
#include "list.h"

typedef enum{
	Node_s_Normal = 0,
	Node_s_Lost = 1,
}NodeState;

typedef enum{
	type_power_failure_1 = 0x49,
}deviceType;

typedef union{
	struct{
		uint16_t P0:1;
		uint16_t P1:1;
		uint16_t P2:1;
		uint16_t P3:1;
		uint16_t P4:1;
		uint16_t P5:1;
		uint16_t P6:1;
		uint16_t P7:1;
	};
	uint16_t state;
}LineState;

typedef struct{
	struct list_head List;
	struct{
		uint32_t address;
		deviceType deviceType;
	}Config;
	union{
		struct{
			uint32_t bitMask;
			LineState LineState;
		}D49H;
	}Data;
	NodeState NodeState;
	time_t lastTime;
}Node;

struct list_head *NodeList(void);

Node *addNode(uint32_t address);
Node *isNodeExist(uint32_t address);
Node *Node_Remove(uint32_t address);

#endif /* GW_NODE_H_ */
