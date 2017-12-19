/*
 * lib_Thread.c
 *
 *  Created on: 2011-4-18
 *      Author: zjm
 */
#include "framework/framework.h"


static LIST_HEAD(ThreadList);

/*****************************************************************
* 函数名称       : CreateThread
* 功能描述       : *
* 输入参数       : char*, void*(*)(void*), void*
*
* 返回值            : pThreadData
* 创建日期       ：2012-1-3
*****************************************************************/
pThreadData CreateThread(char *Name,void *(*start_rtn)(void *),void *data)
{
	pThreadData pThread;
	int stacksize = 512*1024;
	pthread_attr_t attr;

	pThread = malloc(sizeof(tThreadData));

	if(pThread == NULL)
		return NULL;

	memset(pThread,0,sizeof(tThreadData));

	pThread->ThreadName = Name;
	pThread->PrivateData = data;
	pThread->MessageQueue = NULL;//malloc(sizeof(QUEUE)*queueSize);
	pThread->QueueAdd = &pThread->MessageQueue;

	pthread_mutex_init(&pThread->MessageQueueMutex,NULL);
	sem_init(&pThread->WaitEvent,0,0);

	pthread_attr_init(&attr);

	pthread_attr_setstacksize(&attr, stacksize);


	if(pthread_create(&pThread->pthread,&attr,start_rtn,(void *)pThread)!=0){
		perror("pthread_create");
		return NULL;
	}
	printf("create thread:%s\n",Name);

	//*_gAddToList = pThread;
	//_gAddToList = &pThread->next;

	list_add(&pThread->ThreadList,&ThreadList);

	return pThread;
}

void DestroyThread(pThreadData lpthis)
{
	list_del(&lpthis->ThreadList);
	QueueSendMsg(NULL,lpthis->ThreadName,MSG_DESTROY,0,0);
}

void DestroyThreadEx(char *SendTo)
{
	pThreadData Send = NULL;
	struct list_head *i,*q;

	list_for_each_safe(i,q,&ThreadList){
		Send = (pThreadData)i;
		if(strcmp(Send->ThreadName,SendTo) == 0){
			//QueueSendMsg(NULL,SendTo,MSG_DESTROY,0,0);
			pthread_cancel(Send->pthread);
			list_del(&Send->ThreadList);
			free(Send);
			return;
		}
	}
}

/*****************************************************************
* 函数名称       : QueueSendMsg
* 功能描述       : *
* 输入参数       : pThreadData, const char*, uint32_t, uint32_t, uint32_t, int(*)(int, void*, void*), void*
*
* 返回值            : int
* 创建日期       ：2012-1-3
*****************************************************************/
int QueueSendMsg(pThreadData lpthis,const char *SendTo,uint32_t Msg,uint32_t WParam,uint32_t LParam)
{
	pQUEUE msg;
	pThreadData Send = NULL;
	struct list_head *i;

	list_for_each(i,&ThreadList){
		Send = (pThreadData)i;
		if(strcmp(Send->ThreadName,SendTo) == 0){
			pthread_mutex_lock(&Send->MessageQueueMutex);

			msg = malloc(sizeof(QUEUE));

			if(msg == NULL){
				pthread_mutex_unlock(&Send->MessageQueueMutex);
				return ERR_FIAL;
			}

			msg->Sender = lpthis;
			msg->TimeStamp = CurrentTime;
			msg->Msg = Msg;
			msg->WParam = WParam;
			msg->LParam = LParam;
			msg->next = NULL;

			if(Send->MessageQueue){
				*Send->QueueAdd = msg;
				Send->QueueAdd = &msg->next;
			}
			else{
				Send->MessageQueue = msg;
				Send->QueueAdd = &msg->next;
			}

			Send->MessageNum ++;

//			out:
			pthread_mutex_unlock(&Send->MessageQueueMutex);

			sem_post(&Send->WaitEvent);

			return ERR_OK;
		}
	}

	return ERR_FIAL;

}


/*****************************************************************
* 函数名称       : QueueGetMsg
* 功能描述       : *
* 输入参数       : pThreadData, pQUEUE
*
* 返回值            : int
* 创建日期       ：2012-1-3
*****************************************************************/
int QueueGetMsg(pThreadData lpthis,pQUEUE Msg)
{
	pQUEUE msg;

	sem_wait(&lpthis->WaitEvent);

	pthread_mutex_lock(&lpthis->MessageQueueMutex);

	if(lpthis->MessageQueue){
		msg = lpthis->MessageQueue;
		lpthis->MessageQueue = lpthis->MessageQueue->next;
	}
	else{
		pthread_mutex_unlock(&lpthis->MessageQueueMutex);
		return ERR_FIAL;
	}

	memcpy(Msg,msg,sizeof(QUEUE));

	lpthis->MessageNum --;

	free(msg);

	pthread_mutex_unlock(&lpthis->MessageQueueMutex);

	return ERR_OK;
}


/*****************************************************************
* 函数名称       : QueueGetMsgEx
* 功能描述       : *
* 输入参数       : pThreadData, pQUEUE
*
* 返回值            : int
* 创建日期       ：2012-1-3
*****************************************************************/
int QueueGetMsgEx(pThreadData lpthis,pQUEUE Msg)
{
	pQUEUE msg;

	//sem_wait(&lpthis->WaitEvent);

	pthread_mutex_lock(&lpthis->MessageQueueMutex);

	if(lpthis->MessageQueue){
		msg = lpthis->MessageQueue;
		lpthis->MessageQueue = lpthis->MessageQueue->next;
	}
	else{
		pthread_mutex_unlock(&lpthis->MessageQueueMutex);
		return ERR_FIAL;
	}

	memcpy(Msg,msg,sizeof(QUEUE));

	lpthis->MessageNum --;

	free(msg);

	pthread_mutex_unlock(&lpthis->MessageQueueMutex);

	return ERR_OK;
}

int PostMsg(uint32_t Msg,uint32_t WParam,uint32_t LParam)
{
	struct list_head *i;
	int group = Msg>>5;
	int offset = Msg&0x1f;
	pThreadData ops;
	pQUEUE msg;

	list_for_each(i,&ThreadList){
		ops = (void *)i;

		if(ops->signalTab[group] & (1<<offset)){
			pthread_mutex_lock(&ops->MessageQueueMutex);
			msg = malloc(sizeof(QUEUE));

			if(msg == NULL){
				pthread_mutex_unlock(&ops->MessageQueueMutex);
				continue;
			}

			msg->Sender = NULL;
			msg->TimeStamp = CurrentTime;
			msg->Msg = Msg;
			msg->WParam = WParam;
			msg->LParam = LParam;
			msg->next = NULL;

			if(ops->MessageQueue){
				*ops->QueueAdd = msg;
				ops->QueueAdd = &msg->next;
			}
			else{
				ops->MessageQueue = msg;
				ops->QueueAdd = &msg->next;
			}

			ops->MessageNum ++;

//			printf("%s,%d\n",ops->ThreadName,CurrentTime);
//			out:
			pthread_mutex_unlock(&ops->MessageQueueMutex);

			sem_post(&ops->WaitEvent);
		}
	}

	return ERR_OK;
}


int RegisterSignal(pThreadData lpthis,uint32_t msg)
{
	int group = msg>>5;
	int offset = msg&0x1f;

	lpthis->signalTab[group] |= (1<<offset);

	return 0;
}

/*****************************************************************
* 函数名称       : Post_Sem
* 功能描述       : *
* 输入参数       : const char*
*
* 返回值            : int
* 创建日期       ：2012-1-3
*****************************************************************/
int Post_Sem(const char *name)
{
	pThreadData Send = NULL;//_gThreadList;
	struct list_head *i;

	list_for_each(i,&ThreadList){
		Send = (pThreadData)i;
		if(strcmp(Send->ThreadName,name) == 0){
			sem_post(&Send->WaitEvent);
			return ERR_OK;
		}
	}

	return ERR_FIAL;
}


/*****************************************************************
* 函数名称       : dump
* 功能描述       : *
* 输入参数       : void*, int
*
* 返回值            : void
* 创建日期       ：2012-1-3
*****************************************************************/
void dump(void *addr, int len)
{
    unsigned int i = 0;
    unsigned char *buf = (unsigned char*)addr;

    //return;
    printf("  address | ");
    for(i=0;i<=0xf;i++)    printf("%2x ", i);
    printf("\n-------------------------------------------------------------\n");
    i = 0;
    while(i< len)
    {
        unsigned char j = 0;
        printf("0x%08X| ",(uint32_t)&buf[i]);
        do{
                if(i==len){
                int k=0;
                while(0xf - j){
                    j++;
                    k++;
                    printf("   ");
                }
                i+=k+1;
                printf("   ");
                continue;
            }
                printf("%02X ", buf[i++]);
        }while(j++!=0xf);

        i -= j;
        j=0;
        printf("  ");
        do{
            if(i==len){
                j=0xf;
                continue;
            }
            if((buf[i]<32)||(buf[i]>126))    printf(".");
            else
            	printf("%c",buf[i]);
            i++;
        }while(j++!=0xf);
        printf("\n");
    }
    printf("\n");
}
