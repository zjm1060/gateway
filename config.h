/*
 * config.h
 *
 *  Created on: 2017Äê2ÔÂ10ÈÕ
 *      Author: zjm09
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "json/json.h"

extern struct json *Config;

extern int Session;

void config_init(void);
void config_save(void);

//const char *config_get_string(const char *path,...);
#define config_get_string(def,...)	json_exists(Config,__VA_ARGS__)?json_string(Config,__VA_ARGS__):def
#define config_get_integer(def,...)	json_exists(Config,__VA_ARGS__)?(int)json_number(Config,__VA_ARGS__):def
#define config_get_number(...)	json_number(Config,__VA_ARGS__)

#define config_set_string(...)	json_setstring(Config,__VA_ARGS__)
#define config_set_integer(...)	json_setnumber(Config,__VA_ARGS__)
#define config_set_number(...)	json_setnumber(Config,__VA_ARGS__)

#define SaveConfig()	config_save()

struct _config_tab {
	int type;
	int size;
	const char *name;
	void *val;
	union{
		char *string;
		int number;
		float f;
	}def;
};

#define TAB(t,n,v)	{.type=t,.name=n,.val=&v}
#define TAB_I(n,v)	{.type='i',.name=n,.val=&v}
#define TAB_N(n,v)	{.type='n',.name=n,.val=&v}
#define TAB_S(n,v,s)	{.type='s',.name=n,.val=&v,.size=s}

#define INT(v)	(*((int *)v))
#define FLOAT(v)	(*((float *)v))

#endif /* CONFIG_H_ */
