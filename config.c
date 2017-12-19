/*
 * config.c
 *
 *  Created on: 2017Äê2ÔÂ10ÈÕ
 *      Author: zjm09
 */
#include <stdlib.h>
#include "includes.h"
#include "json/json.h"
#include "ds.h"

struct json *Config;

//struct _config_tab config_tab[] = {
//		TAB_I("config.lcd.a0",msic.EnableMaintain),
//		TAB_I("config.lcd.rst",msic.phase),
//		TAB_I("config.msic.EnablePowerAlarmReport",msic.EnablePowerAlarmReport),
//		TAB_I("config.msic.EnableMinReport",msic.EnableMinReport),
//		TAB_I("config.msic.EnableDayReport",msic.EnableDayReport),
//		TAB_I("config.msic.EnableMonReport",msic.EnableMonReport),
//		TAB_I("config.msic.EnableSurpassAlarmReport",msic.EnableSurpassAlarmReport),
//
//		TAB_N("config.elec.voltage.channel[0].rated",VOLTAGE_RATED(0)),
//		TAB_N("config.elec.voltage.channel[0].max",VOLTAGE_MAX(0)),
//		TAB_N("config.elec.voltage.channel[0].min",VOLTAGE_MIN(0)),
//		TAB_N("config.elec.voltage.channel[0].pt",VOLTAGE_PT(0)),
//
//		TAB_N("config.elec.voltage.channel[1].rated",VOLTAGE_RATED(1)),
//		TAB_N("config.elec.voltage.channel[1].max",VOLTAGE_MAX(1)),
//		TAB_N("config.elec.voltage.channel[1].min",VOLTAGE_MIN(1)),
//		TAB_N("config.elec.voltage.channel[1].pt",VOLTAGE_PT(1)),
//
//		TAB_N("config.elec.voltage.channel[2].rated",VOLTAGE_RATED(2)),
//		TAB_N("config.elec.voltage.channel[2].max",VOLTAGE_MAX(2)),
//		TAB_N("config.elec.voltage.channel[2].min",VOLTAGE_MIN(2)),
//		TAB_N("config.elec.voltage.channel[2].pt",VOLTAGE_PT(2)),
//
//		TAB_I("config.msic.mday",MONTH_DAY),
//
//		TAB_S("config.device.did",msic.Original_ID,17),
//		TAB_S("config.device.id",msic.Serial,17),
//		TAB_S("config.password",msic.Password,16),
//
//		TAB_S("config.network.gprs[0].apn",network(0).apn,128),
//		TAB_S("config.network.gprs[0].user",network(0).user,128),
//		TAB_S("config.network.gprs[0].passwd",network(0).passwd,128),
//		TAB_S("config.network.gprs[1].apn",network(1).apn,128),
//		TAB_S("config.network.gprs[1].user",network(1).user,128),
//		TAB_S("config.network.gprs[1].passwd",network(1).passwd,128),
//
//		TAB_S("config.cac[0].host",gWorkSpace.config.Master[0].ServerIP,32),
//		TAB_S("config.cac[0].ifname",gWorkSpace.config.Master[0].ifname,32),
//		TAB_I("config.cac[0].port",gWorkSpace.config.Master[0].ServerPort),
//		TAB_I("config.cac[0].useEncrypt",gWorkSpace.config.Master[0].useEncrypt),
//		TAB_I("config.cac[0].realData",gWorkSpace.config.Master[0].realData),
//		TAB_I("config.cac[0].heart",gWorkSpace.config.Master[0].heartbeat),
//		TAB_S("config.cac[0].proto",gWorkSpace.config.Master[0].protoName,32),
//
//		TAB_S("config.cac[1].host",gWorkSpace.config.Master[1].ServerIP,32),
//		TAB_S("config.cac[1].ifname",gWorkSpace.config.Master[1].ifname,32),
//		TAB_I("config.cac[1].port",gWorkSpace.config.Master[1].ServerPort),
//		TAB_I("config.cac[1].useEncrypt",gWorkSpace.config.Master[1].useEncrypt),
//		TAB_I("config.cac[1].realData",gWorkSpace.config.Master[1].realData),
//		TAB_I("config.cac[1].heart",gWorkSpace.config.Master[1].heartbeat),
//		TAB_S("config.cac[1].proto",gWorkSpace.config.Master[1].protoName,32),
//};

void config_init(void)
{
	int error;
	const char *s;

	Config = json_open(JSON_F_NONE, &error);
	error = json_loadpath(Config,"/etc/system.json");
	if(error){
		fprintf(stdout,"can't open config file!\n");
		exit(0);
	}

	strcpy(opts.did,config_get_string("1234567890","config.device.did"));
	strcpy(opts.qr.url,config_get_string("https://www.baidu.com/s?wd=%s","config.qr.url"));

}
//
//void config_save(void)
//{
//	FILE *fp = NULL;
//
//	for (int i = 0; i < ARRAY_SIZE(config_tab); ++i) {
////		printf("set config:\t%s\n",config_tab[i].name);
//		switch(config_tab[i].type){
//			case 'i':
//				 config_set_integer(INT(config_tab[i].val),config_tab[i].name);
//				break;
//			case 'n':
//				 config_set_number(FLOAT(config_tab[i].val),config_tab[i].name);
//				break;
//			case 's':
//				config_set_string(config_tab[i].val,config_tab[i].name);
//				break;
//		}
//	}
//
//	fp = fopen("/etc/system.json", "w");
//	json_printfile(Config, fp, JSON_F_PRETTY);
//	fclose(fp);
//}

//const char *config_get_string(const char *path,...)
//{
//	va_list ap;
//	int nArgValue =path;
//	va_start(ap, path);
//	const char *s = json_string(Config,path,nArgValue);
//	va_end(ap);
//
//	return s;
//}
//
//int config_get_integer(const char *path,...)
//{
//	int type = json_type(Config,path);
//	const char *s;
//	switch(type){
//		case JSON_T_NUMBER:
//			return json_number(Config,path);
//		case JSON_T_STRING:
//			s = json_string(Config,path);
//			return aoti(s);
//	}
//	return 0;
//}
//
//double config_get_real(const char *path,...)
//{
//	int type = json_type(Config,path);
//	const char *s;
//	switch(type){
//		case JSON_T_NUMBER:
//			return json_number(Config,path);
//		case JSON_T_STRING:
//			s = json_string(Config,path);
//			return aotf(s);
//	}
//	return 0;
//}
