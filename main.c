/*
 * main.c
 *
 *  Created on: 2017��12��14��
 *      Author: zjm09
 */

#include <gw/paho.mqtt/inc/MQTTClient.h>
#include "framework/framework.h"
#include "ds.h"
#include "gw/qr-coder/QR_Encode.h"
#include "log.h"

zlog_category_t *zc;

deviceOpts opts = {
	.did = {"1234567890"},
	.username = {"test"},
	.passwd = {"test"},
	.host = {"lhloao.com"},
	.port = 1883
};

void *menu_proc(void *args);
void *network(void *args);

const char *qr_text = "https://github.com/swex/QR-Image-embedded";
int main(int argc,char *args[])
{

	char qr_msg[512];
//	strncpy(opts.did,basename(args[0]),sizeof(opts.did));

//	if(argc == 2){
//		strncpy(opts.did,args[1],sizeof(opts.did));
//	}

	opts.mqtt.retained = 0;
	opts.mqtt.Qos = 1;

	gpio_init();

	Init_LCD();

	Init_Timer();

	config_init();

	strcpy(opts.did,"asmkldnaskn");

	zlog_init("/etc/zlog.conf");

	log_start("qqw");

//	snprintf(qr_msg,sizeof(qr_msg),
//			opts.qr.url,
//			opts.did);

	varPrint(qr_msg,sizeof(qr_msg),opts.qr.url,0);
	log_info("\n\n------>\nqr url : %s",qr_msg);

	opts.qr.width = EncodeData(QR_LEVEL_M,0,qr_msg,strlen(qr_msg),opts.qr.data);

//	CreateThread("menu",menu_proc,NULL);
	CreateThread("mqtt",network,NULL);

	while (1) {
		WaitTimer();
		TimerProc();
	}

	return 0;
}
