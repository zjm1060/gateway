/*
 * lib_Timer.h
 *
 *  Created on: 2011-4-18
 *      Author: zjm
 */

#ifndef LIB_TIMER_H_
#define LIB_TIMER_H_

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#define CurrentTime time(0)

typedef struct tagTimerList{
	//struct tagTimerList *next;
	//struct tagTimerList *prev;
	struct list_head TimerList;
	int (*cb)(void *,time_t);
	void *cb_data;
	uint32_t timeout;
	uint32_t timeval;
	uint32_t mode;
}TimerList,*pTimerList;

extern time_t _gLocalTime;
extern struct tm _gTime;
extern uint32_t _gRunTime;

ulong __SetupTimer(int timeout,int mode,int(*cb)(void *,time_t),void *cb_data);
void DestroyTimer(ulong ID);
void TimerProc(void);
void mtime(time_t time,struct tm *tm);
int MonthDay(int year,int mon);
void Time2Date(time_t *time,struct tm *date);
void Init_Timer(void);
void WaitTimer(void);
int SameMonth(time_t t1,time_t t2);
void SyncSystemTime(struct tm *time);
void SyncSystemTimeEx(time_t t);
void SyncRTCTime(void);
void EnableSyncTime(void);
void DisableSyncTime(void);
void Time2DateLogic(time_t *time,struct tm *logic);
time_t Time2Logic(time_t *time);
void GetRTCTime(time_t *_time);
time_t MonShift(time_t time,int shift);
int MinuteForNowMonth(void);
char *DateTransform(time_t time);

#define SetupTimer(to,mode,cb,cb_d)	\
	do{\
		__SetupTimer(to,mode,cb,cb_d);\
		printf("Setup Timer "#cb"\n");\
	}while(0)

#endif /* LIB_TIMER_H_ */
