/*
 * node.c
 *
 *  Created on: 2017Äê12ÔÂ14ÈÕ
 *      Author: zjm09
 */
#include <stdlib.h>
#include <gw/node.h>
#include "list.h"

LIST_HEAD(_NODES);

struct list_head *NodeList(void)
{
	return &_NODES;
}

void Node_Append(Node *n)
{
	list_add(&n->List,&_NODES);
}

Node *addNode(uint32_t address)
{
	Node *n = calloc(1,sizeof(Node));

	INIT_LIST_HEAD(&n->List);

	n->Config.address = address;

	Node_Append(n);

	return n;
}

Node *isNodeExist(uint32_t address)
{
	struct list_head *i;
	list_for_each(i,&_NODES){
		Node *n = (void *)i;
		if(n->Config.address == address)
			return n;
	}

	return NULL;
}

Node *Node_Remove(uint32_t address)
{
	struct list_head *i;
	list_for_each(i,&_NODES){
		Node *n = (void *)i;
		if(n->Config.address == address){
			list_del(i);
			return (Node *)i;
		}
	}

	return NULL;
}
