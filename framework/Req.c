/*
 * lib_Req.c
 *
 *  Created on: 2011-7-5
 *      Author: zjm
 */

#include "framework/framework.h"

/*****************************************************************
* 函数名称       : InitReq
* 功能描述       : *
* 输入参数       : pReq
*
* 返回值            : void
* 创建日期       ：2012-1-3
*****************************************************************/
void InitReq(pReq req)
{
	sem_init(&req->xSem,0,0);
}

/*****************************************************************
* 函数名称       : WaitReqFinsh
* 功能描述       : *
* 输入参数       : pReq
*
* 返回值            : int
* 创建日期       ：2012-1-3
*****************************************************************/
int WaitReqFinsh(pReq req)
{
	sem_wait(&req->xSem);
	return ERR_OK;
}

/*****************************************************************
* 函数名称       : ReqFinsh
* 功能描述       : *
* 输入参数       : pReq
*
* 返回值            : void
* 创建日期       ：2012-1-3
*****************************************************************/
void ReqFinsh(pReq req)
{
	sem_post(&req->xSem);
}
