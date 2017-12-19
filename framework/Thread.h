/*
 * lib_Thread.h
 *
 *  Created on: 2011-4-18
 *      Author: zjm
 */

#ifndef LIB_THREAD_H_
#define LIB_THREAD_H_

#include <stdint.h>
#include "list.h"

#define QUEUE_SIZE	(64)

#define IsValidFunction(fun, arg) (fun != NULL)? (fun arg) : (0)

struct ThreadData_t;

typedef struct tagQueue{
	struct ThreadData_t *Sender;
	uint32_t	Msg;
	uint32_t TimeStamp;
	uint32_t WParam;
	uint32_t LParam;
	struct tagQueue *next;
}QUEUE,*pQUEUE;

typedef struct ThreadData_t{
	struct list_head ThreadList;
	char *ThreadName;
	void *PrivateData;
	pthread_t pthread;
	uint32_t signalTab[8];
	struct tagQueue *MessageQueue;//[QUEUE_SIZE];
	struct tagQueue **QueueAdd;
	uint32_t MessageRead;
	uint32_t MessageWrite;
	uint32_t MessageSize;
	uint32_t MessageNum;
	pthread_mutex_t MessageQueueMutex;
	sem_t WaitEvent;
}tThreadData,*pThreadData;




pThreadData CreateThread(char *Name,void *(*start_rtn)(void *),void *data);
int QueueSendMsg(pThreadData lpthis,const char *SendTo,uint32_t Msg,uint32_t WParam,uint32_t LParam);
int QueueGetMsg(pThreadData lpthis,pQUEUE Msg);
int QueueGetMsgEx(pThreadData lpthis,pQUEUE Msg);
int Post_Sem(const char *name);
int PostMsg(uint32_t Msg,uint32_t WParam,uint32_t LParam);
void dump(void *addr, int len);
int RegisterSignal(pThreadData lpthis,uint32_t msg);
pQUEUE InitPool(void);
//pTaskList CreateTaskToThread(char *to);
//pTaskList CreateTaskToThreadEx(char *SendTo,pTaskList ops);
void DestroyThreadEx(char *SendTo);

#endif /* LIB_THREAD_H_ */
