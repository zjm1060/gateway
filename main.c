/*
 * main.c
 *
 *  Created on: 2017Äê12ÔÂ14ÈÕ
 *      Author: zjm09
 */

#include <MQTTClient.h>
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
	.port = 8883
};

void *menu_proc(void *args);
void *network(void *args);


int main(int argc,char *args[])
{

	char qr_msg[512];
//	strncpy(opts.did,basename(args[0]),sizeof(opts.did));

//	if(argc == 2){
//		strncpy(opts.did,args[1],sizeof(opts.did));
//	}

	opts.mqtt.retained = 0;
	opts.mqtt.Qos = 1;

//	gpio_init();

//	gpio_set(GPIO_SYS_CTL,1);

//	Init_LCD();

	Init_Timer();

	config_init();

//	gpio_set(GPIO_NET_LED, 1);

//	strcpy(opts.did,"asmkldnaskn");

	zlog_init("/etc/zlog.conf");

	log_start("mqtt");

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
