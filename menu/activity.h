/*
 * activity.h
 *
 *  Created on: 2017Äê8ÔÂ2ÈÕ
 *      Author: zjm09
 */

#ifndef MENU_ACTIVITY_H_
#define MENU_ACTIVITY_H_

#include <linux/input.h>
#include "stack.h"

struct actObj;

typedef int (*activity)(struct actObj *obj,int msg,int wparam,int lparam);

typedef void (*activityEx)(struct actObj *obj);

struct actObj{
	activity activity;
	void *data;
	struct Stack stack;
//	struct Stack data;
};


void initActivity(void);
void setupActivity(activity f,void *p);
void popActivityEx(int msg);
void doActivity(int msg,int wparam,int lparam);
void activityAttach(void *p);

#define popActivity()	popActivityEx(MSG_INIT)

#endif /* MENU_ACTIVITY_H_ */
