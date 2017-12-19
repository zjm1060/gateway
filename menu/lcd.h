/*
 * lcd.h
 *
 *  Created on: 2017Äê8ÔÂ2ÈÕ
 *      Author: zjm09
 */

#ifndef MENU_LCD_H_
#define MENU_LCD_H_

#include "menu/font/font.h"

void Init_LCD(void);
void LCD_Clear(void);
int LCD_DrawString(int x,int y,tFont *font,const char *string);
void LCD_DrawFormat(int x,int y,tFont *font,const char *fmt,...);
int LCD_Drawbitmap(int X,int Y,int W,int H,uint8_t *pChImage);
void LCD_Mark(int x,int y,int w,int h);
void LCD_Refresh(void);

#define lcd_string(x,y,str)	LCD_DrawString(x,y,&Font_6X12,str)
#define lcd_printf(x,y,str...)	LCD_DrawFormat(x,y,&Font_6X12,##str)

#endif /* MENU_LCD_H_ */
