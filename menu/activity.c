/*
 * activity.c
 *
 *  Created on: 2017Äê8ÔÂ2ÈÕ
 *      Author: zjm09
 */

#include "includes.h"
#include "activity.h"
#include "stack.h"

struct actObj gActObj;

static activity gActivity;

void setupActivity(activity f,void *p)
{
	if(f == NULL)
		return ;

//	if(!stack_empty(&gActObj.stack)){
		stack_push(&gActObj.stack,gActObj.activity);
//	}

	gActObj.activity = f;

	f(&gActObj,MSG_INIT,(int)p,0);
}

void popActivityEx(int msg)
{
	if(stack_empty(&gActObj.stack) == 0){
		activity act = stack_pop(&gActObj.stack);
		gActObj.activity = act;
		act(&gActObj,msg,0,0);
	}

//	return NULL;
}

void initActivity(void)
{
	stack_init(&gActObj.stack);
}

void doActivity(int msg,int wparam,int lparam)
{
	gActObj.activity(&gActObj,msg,wparam,lparam);
}

void activityAttach(void *p)
{
	gActObj.data = p;
}
