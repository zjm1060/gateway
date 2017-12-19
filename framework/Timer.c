/*
 * lib_Timer.c
 *
 *  Created on: 2011-4-18
 *      Author: zjm
 */
#include <sys/time.h>
#include <linux/rtc.h>
#include <linux/watchdog.h>
#include <sys/timerfd.h>
#include "framework/framework.h"

#ifdef DO_FAST_TEST
#define TIMER_INV	1000
#else
#define TIMER_INV	100
#endif
//extern pSramTemp TempData;

static int timeFd;
#ifdef USE_WATCHDOG
static int WatchDogFd;
#endif
//static pTimerList _gTimerList = NULL;
static LIST_HEAD(_gTimerList);
uint32_t _gRunTime;
time_t _gLocalTime;
struct tm _gTime;

/*****************************************************************
* ��������       : SetupTimer
* ��������       : *
* ��������       : int, int, int(*)(void*, time_t), void*
*
* ����ֵ            : pTimerList
* ��������       ��2012-1-3
*****************************************************************/
ulong __SetupTimer(int timeout,int mode,int(*cb)(void *,time_t),void *cb_data)
{
	pTimerList time;

	time = malloc(sizeof(TimerList));
	if(time == NULL)
		return 0;

	time->timeout = timeout;
	time->timeval = 0;
	time->cb = cb;
	time->cb_data = cb_data;
	time->mode = mode;

	list_add(&time->TimerList,&_gTimerList);

	return (ulong)time;
}


/*****************************************************************
* ��������       : DestroyTimer
* ��������       : *
* ��������       : pTimerList
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void DestroyTimer(ulong ID)
{
	struct list_head *p = (struct list_head *)ID;

	list_del(p);

	free(p);
}



/*****************************************************************
* ��������       : Time2Date
* ��������       : *
* ��������       : time_t*, struct tm*
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void Time2Date(time_t *time,struct tm *date)
{
	struct tm *local_time;

	local_time = localtime(time);
	memcpy(date,local_time,sizeof(struct tm));
}


void WatchDog(void)
{
#ifdef USE_WATCHDOG
		//write(WatchDogFd, &flag, 1); //Reset Watchdog
		ioctl(WatchDogFd, WDIOC_KEEPALIVE, 0);
#endif
}

void StartWatchDog(void)
{
#ifdef USE_WATCHDOG
	int timeout = 5;

	WatchDogFd = open("/dev/watchdog",O_RDWR);
	if(WatchDogFd == -1){
		perror("Open watchdog");
		exit(-1);
	}


	timeout = 5;
	ioctl(WatchDogFd, WDIOC_SETTIMEOUT, &timeout);
	ioctl(WatchDogFd, WDIOC_GETTIMEOUT, &timeout);
#endif
}

/*****************************************************************
* ��������       : TimerProc
* ��������       : *
* ��������       : void
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void TimerProc(void)
{
	pTimerList list;
	static clock_t t = 1000;
	struct list_head *i;

//	if(t >= 40000000)
//		t = 0;
	t += TIMER_INV;
	if(t%1000 == 0){
//		printf("%d\n",CurrentTime);
		_gRunTime ++;
		_gLocalTime += 1;
		Time2Date(&_gLocalTime,&_gTime);
		if(_gRunTime&1){
			SET_RUN_LED();
		}
		else{
			CLR_RUN_LED();
		}
		PostMsg(MSG_TIMER,0,0);
		WatchDog();
#if 0
		if(GET_IO(IO_PWR_PROBE) == 0){
			PostMsg(MSG_POWERDOWN,0,0);
			CLR_RUN_LED();
			sleep(3);
			PostMsg(MSG_DATABASE_OFF,0,0);
#ifdef USE_WATCHDOG
			close(WatchDogFd);
#endif
		}
#endif
	}

	list_for_each(i,&_gTimerList){
		list = (pTimerList)i;
		list->timeval += TIMER_INV;
		if(list->timeval >= list->timeout){
			IsValidFunction(list->cb,(list->cb_data,CurrentTime));
			list->timeval = 0;
//			if(list->mode == MODE_SINGLE){
				// remove this timer and free memroy
				//DestroyTimer(list);
//			}
		}
	}
}


/*****************************************************************
* ��������       : Init_Timer
* ��������       : *
* ��������       : void
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void Init_Timer(void)
{
	struct itimerspec time;

	GetRTCTime(&_gLocalTime);

	//_gLocalTime -= _gLocalTime%60;

	Time2Date(&_gLocalTime,&_gTime);

	time.it_interval.tv_sec = 0;
	time.it_interval.tv_nsec = TIMER_INV*1000*1000;
//	time.it_interval.tv_nsec = 50*1000*1000;
	time.it_value.tv_sec = 0;
	time.it_value.tv_nsec = TIMER_INV*1000*1000;
//	time.it_value.tv_nsec = 50*1000*1000;

	_gRunTime = 0;

	timeFd = timerfd_create(CLOCK_MONOTONIC,0);
	timerfd_settime(timeFd,0,&time,NULL);

//	EnableSyncTime();
}

void CloseWDT(void)
{
#ifdef USE_WATCHDOG
	ioctl(WatchDogFd, WDIOC_KEEPALIVE, 0);
	close(WatchDogFd);
#endif
}


/*****************************************************************
* ��������       : WaitTimer
* ��������       : *
* ��������       : void
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void WaitTimer(void)
{
	uint64_t exp;
	ssize_t s;

	s = read(timeFd,&exp,sizeof(uint64_t));
	if (s != sizeof(uint64_t)){
		perror("read");
	}
//	static time_t old;
//	time_t t;// = CurrentTime;
//	while(1){
//		t = CurrentTime;
//		if(t != old){
//			old = t;
//			return ;
//		}
//		usleep(100*1000);
//	}

//	printf("--->read: %llu; %s\n",exp,DateTransform(CurrentTime));
}


/*****************************************************************
* ��������       : GetRTCTime
* ��������       : *
* ��������       : time_t*
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void GetRTCTime(time_t *_time)
{
	*_time = time(0);
}


/*****************************************************************
* ��������       : SyncRTCTime
* ��������       : *
* ��������       : void
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void SyncRTCTime(void)
{
	int rtc;
	struct tm _time;
	struct timeval tv;
	const struct timezone tz = { timezone/60 - 60*daylight, 0 };

	rtc = open("/dev/rtc0",O_RDONLY);
	if(rtc == -1){
		perror("open /dev/rtc0");
		return;
	}
	memset(&_time, 0, sizeof(struct tm));
	ioctl(rtc, RTC_RD_TIME, &_time);
	_time.tm_isdst = -1; /* not known */
	close(rtc);
	tv.tv_sec = mktime(&_time);
	tv.tv_usec = 0;
	if(settimeofday(&tv, &tz)){
		perror("settimeofday() failed");
	}
	time(&_gLocalTime);
}


/*****************************************************************
* ��������       : SyncSystemTime
* ��������       : *
* ��������       : struct tm*
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void SyncSystemTime(struct tm *time)
{
	int rtc;

	rtc = open("/dev/rtc0",O_WRONLY);
	if(rtc == -1){
		perror("open /dev/rtc0");
		return;
	}

	time->tm_isdst = 0;
	ioctl(rtc, RTC_SET_TIME, time);
	close(rtc);

	SyncRTCTime();
}

void SyncSystemTimeEx(time_t t)
{
//	int rtc;
//	struct tm tm;

//	t += 8*3600;

//	Time2Date(&t,&tm);
//
//	rtc = open("/dev/rtc0",O_WRONLY);
//	if(rtc == -1){
//		perror("open /dev/rtc0");
//		return;
//	}
//
//	tm.tm_isdst = 0;
//	ioctl(rtc, RTC_SET_TIME, &tm);
//	close(rtc);
//
//	SyncRTCTime();

	char cmd[64];

	snprintf(cmd,sizeof cmd,"date @%lu && hwclock -w",t);
	system(cmd);
}

/*****************************************************************
* ��������       : mtime
* ��������       : *
* ��������       : time_t, struct tm*
*
* ����ֵ            : void
* ��������       ��2012-1-3
*****************************************************************/
void mtime(time_t time,struct tm *tm)
{
	struct tm *__tm;

	__tm = gmtime(&time);

	memcpy(tm,__tm,sizeof(struct tm));
}


/*****************************************************************
* ��������       : SameMonth
* ��������       : *
* ��������       : time_t, time_t
*
* ����ֵ            : int
* ��������       ��2012-1-3
*****************************************************************/
int SameMonth(time_t t1,time_t t2)
{
	struct tm t_1,t_2;
	struct tm *local_time;

	local_time = localtime(&t1);
	memcpy(&t_1,local_time,sizeof(t_1));
	local_time = localtime(&t2);
	memcpy(&t_2,local_time,sizeof(t_2));
	if(t_1.tm_mon == t_2.tm_mon && t_1.tm_year == t_2.tm_year)
		return ERR_OK;
	return ERR_FIAL;
}


/*****************************************************************
* ��������       : MonShift
* ��������       : *
* ��������       : time_t, int
*
* ����ֵ            : time_t
* ��������       ��2012-1-3
*****************************************************************/
time_t MonShift(time_t time,int shift)
{
	struct tm *local_time = localtime(&time);
	struct tm tm_time;

	memcpy(&tm_time,local_time,sizeof(struct tm));
	tm_time.tm_sec = 0;
	tm_time.tm_min = 0;
	tm_time.tm_hour = 0;
	tm_time.tm_mday = 1;
	tm_time.tm_mon += shift;
	if(tm_time.tm_mon > 11){
		tm_time.tm_year ++;
		tm_time.tm_mon = 0;
	}
	else if(tm_time.tm_mon < 0){
		tm_time.tm_year --;
		tm_time.tm_mon = 11;
	}

	return mktime(&tm_time);
}

int MinuteForNowMonth(void)
{
	return _gTime.tm_mday*1440 + _gTime.tm_min;
}

char *DateTransform(time_t time)
{
	static char ouText[32];
	struct tm _time;

	if(time == 0){
		return "0000-00-00 00:00:00";
	}

//	mtime(time,&_time);

	memcpy(&_time,localtime(&time),sizeof(_time));

	snprintf(ouText,sizeof(ouText),"%04d-%02d-%02d %02d:%02d:%02d",
			_time.tm_year+1900,
			_time.tm_mon+1,
			_time.tm_mday,
			_time.tm_hour,
			_time.tm_min,
			_time.tm_sec);

	return ouText;
}
