/*
 * vprint.c
 *
 *  Created on: 2015Äê10ÔÂ19ÈÕ
 *      Author: zjm
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include "node.h"
#include "../ds.h"

typedef struct{
	const char *var;
	char *(*fun)(void *p);
}VarLIst;

struct MStr{
	const char *p;
	int len;
};

char *getDeviceID(void *p)
{
	return opts.did;
}

char *getNodeType(void *p)
{
	Node *ops = p;
	static char tmp[8];

	snprintf(tmp,sizeof(tmp),"%d",ops->Config.deviceType);

	return tmp;
}

char *getNodeAddr(void *p)
{
	Node *ops = p;
	static char tmp[32];

	snprintf(tmp,sizeof(tmp),"%08X",ops->Config.address);

	return tmp;
}

//char *getNodeName(void *p)
//{
//	Node *ops = p;
//	static char tmp[128];
//
//	snprintf(tmp,sizeof(tmp),"%s",ops->Config.name);
//
//	return tmp;
//}

char *getNodeVoltageState(void *p)
{
	Node *ops = p;
	static char tmp[128];

	if((ops->Data.LineState.state) == 0){
		return "0,0,0";
	}
	snprintf(tmp,sizeof(tmp),"%s,%s,%s",
			(ops->Data.LineState.P1)?"1":"0",
			(ops->Data.LineState.P2)?"1":"0",
			(ops->Data.LineState.P3)?"1":"0"
					);

	return tmp;
}

char *getDate(void *p)
{
	time_t t = time(0);
	struct tm *tm = localtime(&t);
	static char date[32];

	snprintf(date,sizeof(date),"%04d-%02d-%02d",
			tm->tm_year+1900,
			tm->tm_mon+1,
			tm->tm_mday);

	return date;
}

char *getTime(void *p)
{
	time_t t = time(0);
	struct tm *tm = localtime(&t);
	static char date[32];

	snprintf(date,sizeof(date),"%02d:%02d:%02d",
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec);

	return date;
}

char *getTimestamp(void *p)
{
	time_t t = time(0);
	static char date[32];

	snprintf(date,sizeof(date),"%lu",
			t);

	return date;
}

VarLIst __VarList[] = {
	{"NODE_ADDR",getNodeAddr},
//	{"NODE_NAME",getNodeName},
	{"NODE_VOLSTATE",getNodeVoltageState},
	{"DEVICE_ID",getDeviceID},
	{"NODE_TYPE",getNodeType},
	{"TIMESTAMP",getTimestamp},
	{"DATE",getDate},
	{"TIME",getTime},
	{NULL,NULL},
};

void vprint(char *buffer,const char *format,void *p)
{
	char c;
	int i;

	while(*format){
		c = *format;
		if(c != '%'){
			*buffer++ = c;
//			putc(c,stdout);
//			fflush(stdout);
			format ++;
		}
		else{
			i = 0;
			format ++;
			while(__VarList[i].var != NULL){
//				struct MStr s1 = {.p =  __VarList[i].var,.len = strlen(__VarList[i].var)};
//				char *p = strchr(format,'}');
//				struct MStr s = {.p = format,.len = (p-format)};
				int len = strlen(__VarList[i].var);
//				if(strncmp(__VarList[i].var,s.p,s.len/*__VarList[i].var,format,len*/) == 0){
				if(strncmp(__VarList[i].var,format,len) == 0){
					if(__VarList[i].fun){
						char *res = __VarList[i].fun(p);
						if(res){
							int _len = strlen(res);
							strcpy(buffer,res);
							buffer += _len;
						}
					}
					format += len;
					break;
				}

				i++;
			}
		}
	}
}

VarLIst __varList[] = {
		{"nodeAddr",getNodeAddr},
		{"deviceID",getDeviceID},
//		{"nodeName",getNodeName},
		{"voltageState",getNodeVoltageState},
		{"timestamp",getTimestamp},
		{"date",getDate},
		{"time",getTime},
};

// ${var}
int varPrint(char *buffer,int buffer_size,const char *format,void *p)
{
	char c;
	int output_count = 0;
	while(*format){
		c = *format;
		if(c != '$'){
			buffer[output_count] = c;
			output_count ++;
		}else{
			format ++;
			c = *format;
			if(c != '['){
//				loop:
				buffer[output_count] = c;
				output_count ++;
			}else{
//				char *varEnd = strchr(format,',');
//				if(varEnd)goto loop;
//				varEnd = strchr(format,' ');
//				if(varEnd)goto loop;
				char *varEnd = strchr(format,']');
				format ++;
				if(varEnd){
					struct MStr s1 = {.p =  format,.len = (varEnd-format)};

					for (int i = 0; i < sizeof(__varList)/sizeof(VarLIst); ++i) {
						struct MStr s2 = {.p = __varList[i].var,.len = strlen(__varList[i].var)};
						if(!strncmp(s2.p,s1.p,s2.len>s1.len?s2.len:s1.len)){
							if(__varList[i].fun){
								char *res = __varList[i].fun(p);
								if(res){
									int _len = strlen(res);
									strcpy(&buffer[output_count],res);
//									buffer += _len;
									output_count +=_len;
								}
							}
						}
					}

					format += s1.len;
				}else{
					buffer[output_count] = c;
					output_count ++;
				}
			}
		}

		format ++;
	}

	return output_count;
}
