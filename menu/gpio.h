/*
 * gpio.h
 *
 *  Created on: 2017Äê8ÔÂ2ÈÕ
 *      Author: zjm09
 */

#ifndef MENU_GPIO_H_
#define MENU_GPIO_H_

enum{
	GPIO_LCD_A0,
	GPIO_LCD_RES,
	GPIO_LCD_BLK,
//	GPIO_ALARM_LED1,
//	GPIO_ALARM_LED2,
//	GPIO_ALARM_LED3,
	GPIO_RUN_LED,
	GPIO_NET_LED,
	GPIO_POWER,
	GPIO_SYS_CTL,
	GPIO_ALARM_OUT,
};

enum{
	PA=0,PB,PC,PD,PE,PF,PG,PH,PI
};

#define PORT2PIN(port,pin)	((port*32) + pin)

void gpio_init(void);
void gpio_set(int fun,int v);
int gpio_get(int fun);

#define SET_RUN_LED()	gpio_set(GPIO_RUN_LED,1)
#define CLR_RUN_LED()	gpio_set(GPIO_RUN_LED,0)

#define GET_IO(i)	gpio_get(i)


#endif /* MENU_GPIO_H_ */
