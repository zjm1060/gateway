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
		uint8_t P0:1;
		uint8_t P1:1;
		uint8_t P2:1;
		uint8_t P3:1;
		uint8_t P4:1;
		uint8_t P5:1;
		uint8_t P6:1;
		uint8_t P7:1;
	};
	uint8_t state;
}LineState;

typedef struct{
	struct list_head List;
	struct{
		uint32_t address;
		deviceType deviceType;
		uint32_t bitMask;
	}Config;
	struct{
		NodeState NodeState;
		LineState LineState;
		time_t lastTime;
	}Data;
}Node;

struct list_head *NodeList(void);

Node *addNode(uint32_t address);
Node *isNodeExist(uint32_t address);
Node *Node_Remove(uint32_t address);

#endif /* GW_NODE_H_ */
