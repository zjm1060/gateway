/*
 * Standby.c
 *
 *  Created on: 2017年8月2日
 *      Author: zjm09
 */

#include "includes.h"
#include "../menu.h"
#include "../../ds.h"

const unsigned char Antenna[12] = {
		0x00,0x7C,0x10,0x10,0x10,0x10,0x10,0x10,
		0x10,0x00,0x00,0x00
};

const unsigned char Signal_0[12] = {
		0x00,0x00,0x00,0x00,0x88,0x50,0x20,0x50,
		0x88,0x00,0x00,0x00
};

const unsigned char Signal_1[12] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
		0x80,0x00,0x00,0x00
};

const unsigned char Signal_2[12] = {
		0x00,0x00,0x00,0x00,0x00,0x20,0x20,0xA0,
		0xA0,0x00,0x00,0x00
};

const unsigned char Signal_3[12] = {
		0x00,0x00,0x00,0x08,0x08,0x28,0x28,0xA8,
		0xA8,0x00,0x00,0x00
};

char *getip(const char *name)
{
	struct ifreq temp;
	struct sockaddr_in *myaddr;
	static char ip_buf[64];
	int fd = 0;
	int ret = -1;
	strcpy(temp.ifr_name, name);
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return "0.0.0.0";
	}
	ret = ioctl(fd, SIOCGIFADDR, &temp);
	close(fd);
	if (ret < 0)
		return
		"0.0.0.0";
	myaddr = (struct sockaddr_in *) &(temp.ifr_addr);
	strcpy(ip_buf, inet_ntoa(myaddr->sin_addr));
	return ip_buf;
}

static int __index = 0;
void ShowVal(void)
{
	char ouText[32];
	int x,y;
	int width = opts.qr.width;

	LCD_Clear();

	if(__index == 1){

		lcd_printf(0,0,"GPRS:%s",getip("ppp0"));
		lcd_printf(0,12,"eth: %s",getip("eth0"));
		lcd_printf(0,24,"%08X %d dbm",opts.analysis.node.currentAddress,opts.analysis.node.Signal);

		lcd_string(6,52,DateTransform(time(0)));
	}
	else{
		LCD_Mark(0,0,width+4,width+4);
		for (x = 0; x < width; x++) {
			for (y = 0; y < width; y++) {
				int byte = (x * width + y) / 8;
				int bit = (x * width + y) % 8;
				int value = opts.qr.data[byte] & (0x80 >> bit);
				DrawDot(x+2, y+2, value ? 0 : 1);
			}
		}

		lcd_printf(width+5,width/2-12,"id:%s",opts.did);
		lcd_string(width+5,width/2,"请扫描二维码");
	}

	LCD_Refresh();
}

int mainMenu(struct actObj *obj,int msg,int wparam,int lparam);

int StandbyProc(struct actObj *obj,int msg,int wparam,int lparam)
{
	switch(msg){
		case MSG_INIT:{
			int fd = open("/tmp/sim.0",O_RDONLY);
			if(fd != -1){
				char buf[64];
				lseek(fd,0,SEEK_SET);

				read(fd,buf,sizeof(buf));

				close(fd);
			}
		}break;
		case MSG_TIMER:
			ShowVal();
			break;
		case MSG_KEY:{
				if(wparam == KEY_ENTER){
					__index = !__index;
				}
			}break;
		default:
			break;
	}
}

void menuInit(void)
{
	setupActivity(StandbyProc,NULL);
}
