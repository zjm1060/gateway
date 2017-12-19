/*
 * menu.c
 *
 *  Created on: 2017年8月2日
 *      Author: zjm09
 */
#include "includes.h"
#include "activity.h"
#include "stack.h"
#include "lcd.h"
#include "font/font.h"
#include <linux/input.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

void menuInit(void);

int keyboard(void)
{
	int fd = open("/dev/input/event0",O_RDWR);

	return fd;
}

int timer(void)
{
	struct itimerspec time;

	time.it_interval.tv_sec = 1;
	time.it_interval.tv_nsec = 0;
	time.it_value.tv_sec = 1;
	time.it_value.tv_nsec = 0;

	int timeFd = timerfd_create(CLOCK_REALTIME,TFD_NONBLOCK);
	timerfd_settime(timeFd,0,&time,NULL);

	return timeFd;
}

static void *menu_ex(void *args)
{
	pThreadData lpthis = args;
	QUEUE Msg;

	activityAttach(lpthis);

	while(1){
		QueueGetMsg(lpthis,&Msg);
		doActivity(Msg.Msg,Msg.WParam,Msg.LParam);
	}

	return NULL;
}

void *menu_proc(void *args)
{
	struct epoll_event events[2];
	struct epoll_event epv[2];
	int blk_timeout = 5;

	gpio_set(GPIO_LCD_BLK,1);

	int g_epollFd = epoll_create(1);

	int key = keyboard();
	int tim = timer();

	epv[0].events = EPOLLIN;
	epv[0].data.fd = key;
	epv[1].events = EPOLLIN | EPOLLET;
	epv[1].data.fd = tim;

	epoll_ctl(g_epollFd, EPOLL_CTL_ADD, key, &epv[0]);
	epoll_ctl(g_epollFd, EPOLL_CTL_ADD, tim, &epv[1]);

	initActivity();

	menuInit();

	CreateThread("menuEx",menu_ex,NULL);

	while(1){
		int fds = epoll_wait(g_epollFd, events, 1, -1);
		if(fds < 0){
//			perror("epoll");
		}

		for (int i = 0; i < fds; ++i) {
			if(events[i].data.fd == key){
				struct input_event k;

				read(key,&k,sizeof(k));
				if(k.type == EV_KEY && (k.value == 1 || k.value == 2)){
//					doActivity(MSG_KEY,k.code,0);
					QueueSendMsg(NULL,"menuEx",MSG_KEY,k.code,0);
					gpio_set(GPIO_LCD_BLK,1);
					blk_timeout = 30;
				}
			}
			else if(events[i].data.fd == tim){
				uint64_t exp;

				read(tim,&exp,sizeof(uint64_t));

//				doActivity(MSG_TIMER,0,0);
				QueueSendMsg(NULL,"menuEx",MSG_TIMER,0,0);
				if(CurrentTime%300 == 0){
					Init_LCD();
				}
//				if(blk_timeout && --blk_timeout == 0)
//					gpio_set(GPIO_LCD_BLK,0);
			}
		}
	}
}

void DrawItme(tFont *font,char *title,char **itme,int size,int select)
{
	int _y = 0,index;
	int line_pre_srccen,start = 0;
	int font_max_hight = font->GetSymbolMaxHight(font)+1;

	LCD_Clear();

	LCD_DrawString(0,_y,font,title);
	_y += font->GetSymbolMaxHight(font);

	line_pre_srccen = (64/font_max_hight) - 1;

	if(select < start)
		start = 0;
	else{
		while((start+line_pre_srccen) < select){
			start ++;
		}
	}

	index = start;

	while((_y+font_max_hight) <= 64){
		LCD_DrawString(10,_y,font,itme[index]);
		_y += font_max_hight;
		index ++;
		if(index >= size)
			break;
	}

	index = select - start;

	_y = (index+1)*font_max_hight;

	LCD_Mark(0,_y-1,128,font->GetSymbolMaxHight(font));

	LCD_Refresh();
}

#if 0
struct _itme{
	char *title;
	char **itme;
	int size;
};

static int list(struct actObj *obj,int msg,int wparam,int lparam)
{
	static int select;
	static int timeout;
	static char *title;
	static char **itme;
	static int size;
	switch(msg){
		case MSG_INIT:
			select = 0;
			timeout = 60;
			title = ((struct _itme *)wparam)->title;
			itme = ((struct _itme *)wparam)->itme;
			size = ((struct _itme *)wparam)->size;
			DrawItme(&Font_6X12,title,itme,size,select);
			break;
		case MSG_KEY:
			timeout = 60;
			switch(wparam){
				case KEY_UP:
					select --;
					if(select < 0)
						select = size-1;
					break;
				case KEY_DOWN:
					select ++;
					if(select >= size)
						select = 0;
					break;
				case KEY_ESC:
					stack_push(&obj->data,(void *)-1);
					popActivityEx(MSG_RESTORE);
					return 0;
				case KEY_ENTER:
					stack_push(&obj->data,(void *)select);
					popActivityEx(MSG_RESTORE);
					return 0;
			}
			DrawItme(&Font_6X12,title,itme,size,select);
			break;
		case MSG_TIMER:
			timeout --;
			if(timeout == 0)
				return ERR_FIAL;
			break;
	}

	return 0;
}
#endif

int setupList(struct actObj *obj,char *title,const char **itme,int size,int select)
{
	pThreadData lpthis = obj->data;
	QUEUE Msg;
	int timeout;

	QueueSendMsg(NULL,lpthis->ThreadName,MSG_INIT,0,0);

	while(1){
		QueueGetMsg(lpthis,&Msg);
		switch(Msg.Msg){
			case MSG_INIT:
				select = 0;
				timeout = 60;
				DrawItme(&Font_6X12,title,itme,size,select);
				break;
			case MSG_KEY:
				timeout = 60;
				switch(Msg.WParam){
					case KEY_UP:
						select --;
						if(select < 0)
							select = size-1;
						break;
					case KEY_DOWN:
						select ++;
						if(select >= size)
							select = 0;
						break;
					case KEY_ESC:
						return -1;
					case KEY_ENTER:
						return select;
				}
				DrawItme(&Font_6X12,title,itme,size,select);
				break;
			case MSG_TIMER:
				timeout --;
				if(timeout == 0)
					return ERR_FIAL;
				break;
		}
	}
	return 0;
}

static const char *onOffList[] = {
	"关闭",
	"开启",
};

int onOff(struct actObj *obj)
{
	return setupList(obj,"请选择",onOffList,2,0);
}
