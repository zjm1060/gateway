/*
 * menu.h
 *
 *  Created on: 2017Äê8ÔÂ3ÈÕ
 *      Author: zjm09
 */

#ifndef MENU_MENU_H_
#define MENU_MENU_H_

#include "lcd.h"
#include "activity.h"

char *VoltageTransform(float pt,float val);
int setupList(struct actObj *obj,char *title,char **itme,int size,int select);
int MenuGetDay(struct actObj *obj);
int selectPhase(struct actObj *obj,int max);
char *getPasswd(struct actObj *obj);
int onOff(struct actObj *obj);

#endif /* MENU_MENU_H_ */
