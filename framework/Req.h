/*
 * lib_Req.h
 *
 *  Created on: 2011-7-5
 *      Author: zjm
 */

#ifndef LIB_REQ_H_
#define LIB_REQ_H_

#include <pthread.h>
#include <semaphore.h>

typedef struct{
	pthread_mutex_t xMut;
	pthread_cond_t xCond;
	sem_t xSem;
	int State;
	time_t Date;
	void *Data;
	char *Interface;
	char device[16];
}Req,*pReq;

void InitReq(pReq req);
int WaitReqFinsh(pReq req);
void ReqFinsh(pReq req);

#endif /* LIB_REQ_H_ */
