/*
 * lcd.c
 *
 *  Created on: 2017Äê8ÔÂ2ÈÕ
 *      Author: zjm09
 */
#include "includes.h"
#include "font/font.h"
#include "gpio.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define X_SIZE 128
#define Y_SIZE 64

#define LCD_DEVICE	"/dev/spidev2.0"

static int spi_fd = 0;
static uint8_t mDispBuffer[Y_SIZE/8][X_SIZE];

#define SET_A0(v) 	gpio_set(GPIO_LCD_A0,v)
#define SET_RES(v) 	gpio_set(GPIO_LCD_RES,v)

static void spi_transfer(const uint8_t *c,int len)
{
#if 0
	struct spi_ioc_transfer xfer;

	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = (__u64) c;
	xfer.len = 1;
	xfer.delay_usecs = 0;

	if(ioctl(spi_fd, SPI_IOC_MESSAGE(1), &xfer) < 0){
		perror("SPI_IOC_MESSAGE");
	}

#else
	write(spi_fd,c,len);
#endif
}

void LCD_CMD(const uint8_t *cmd,int len)
{
	SET_A0(0);
	spi_transfer(cmd,len);
}

void LCD_DAT(const uint8_t *dat,int len)
{
	SET_A0(1);
	spi_transfer(dat,len);
}

const uint8_t Lcd_init_cmd[] = {0xe2,0xa2,0xa0,0xc0,0xa6,0x24,0x81,0x27,0x2f,0xaf};

void Init_LCD(void)
{
	static int inited = 0;
	int mode = SPI_MODE_0;
	int bits = 8;
	int speed = 1000000;
	int lsb = 0;
	int ret;

	if(inited == 0){
		spi_fd = open(LCD_DEVICE,O_RDWR);
		if(spi_fd < 0){
			perror("lcd spi");
			exit(1);
		}

		ret = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
		ret = ioctl(spi_fd,SPI_IOC_WR_LSB_FIRST,&lsb);
		ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
		ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

		inited = 1;
	}

	SET_RES(0);
	usleep(100*1000);
	SET_RES(1);
	usleep(300*1000);

	LCD_CMD(Lcd_init_cmd,sizeof(Lcd_init_cmd));
}

void DrawDot(int x,int y,int black)
{
	int __x,__y;

	if(x >= X_SIZE || y >= Y_SIZE || x < 0 || y < 0)
		return;

	__x = x;
	__y = y>>3;

	if(black){
		mDispBuffer[__y][__x] |= (1<<(7-(y&0x07))) ;
	}
	else{
		mDispBuffer[__y][__x] &= ~(1<<(7-(y&0x07)));
	}
}

int LCD_DrawLine(int x1, int y1, int x2,int y2)
{
    int dx,dy,e;
    dx=x2-x1;
    dy=y2-y1;

    if(dx>=0)
    {
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 1/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                	DrawDot(x1,y1,1);
                    if(e>0){y1+=1;e-=dx;}
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 2/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                	DrawDot(x1,y1,1);
                    if(e>0){x1+=1;e-=dy;}
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)

            if(dx>=dy) // 8/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                	DrawDot(x1,y1,1);
                    if(e>0){y1-=1;e-=dx;}
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 7/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                	DrawDot(x1,y1,1);
                    if(e>0){x1+=1;e-=dy;}
                    y1-=1;
                    e+=dx;
                }
            }
        }
    }
    else //dx<0
    {
        dx=-dx;        //dx=abs(dx)
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 4/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                	DrawDot(x1,y1,1);
                    if(e>0){y1+=1;e-=dx;}
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 3/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                	DrawDot(x1,y1,1);
                    if(e>0){x1-=1;e-=dy;}
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)

            if(dx>=dy) // 5/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                	DrawDot(x1,y1,1);
                    if(e>0){y1-=1;e-=dx;}
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 6/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                	DrawDot(x1,y1,1);
                    if(e>0){x1-=1;e-=dy;}
                    y1-=1;
                    e+=dx;
                }
            }
        }
    }
	return ERR_OK;
}

void LCD_Clear(void)
{
	memset(mDispBuffer,0,sizeof(mDispBuffer));
}

#if 1
int LCD_DrawChar(int X,int Y,tFont *font,short ch)
{

	uint32_t xCnt,yCnt,x,y;//,i;
	uint8_t mask = 0,temp = 0;
	uint8_t *pChImage;
	tSymbol symbol;

	if(font->GetSymbol(font,ch,&symbol) != ERR_OK)
		return 0;
	pChImage = symbol.symbol;
	y = Y;
	for(yCnt=0;yCnt<(symbol.h);yCnt++){
		x = X;
		for(xCnt=0;xCnt<symbol.w;xCnt++){

			if(mask == 0){
				mask = 0x80;
				temp = *pChImage++;
			}
			//DrawPiexl(r,x,y,color2);

			if(temp & mask)

				DrawDot(x,y,0x01);
			else
				DrawDot(x,y,0x00);
			x++;
			mask >>= 1;
		}
		mask = 0;
		y ++;
	}
	return symbol.w;
}

int LCD_DrawString(int x,int y,tFont *font,const char *string)
{
	unsigned short ch;

	while(string && *string){
		if(*string < 127)
			ch = *string;
		else{
			ch = (string[0]<<8) + string[1];
			string ++;
		}
		string ++;

		if(x >= X_SIZE)
			return x;

		if((x+font->GetSymbolWidth(font,ch)) >= X_SIZE){
//			return x;

			x = 0;

			y += font->GetSymbolMaxHight(font);
		}
		x += LCD_DrawChar(x,y,font,ch);

	}

	return x;
}

void LCD_DrawFormat(int x,int y,tFont *font,const char *fmt,...)
{
	va_list ap;
	char tmp[64];

	va_start(ap,fmt);
	vsnprintf(tmp,sizeof(tmp),fmt,ap);
	LCD_DrawString(x,y,font,tmp);
	va_end(ap);
}
#endif

void LCD_Mark(int x,int y,int w,int h)
{
	#define BITXOR(byte,bit,val) byte ^= (val<<bit)

    int X,Y;
    for(Y=y;Y<y+h;Y++)
    	for(X=x;X<x+w;X++)
    		BITXOR(mDispBuffer[(Y>>3)][X],(7-(Y&0x07)),1);
}


int LCD_Drawbitmap(int X,int Y,int W,int H,uint8_t *pChImage)
{

	uint32_t xCnt,yCnt,x,y;//,i;
	uint8_t mask = 0,temp = 0;

	y = Y;
	for(yCnt=0;yCnt<(H);yCnt++){
		x = X;
		for(xCnt=0;xCnt<W;xCnt++){

			if(mask == 0){
				mask = 0x80;
				temp = *pChImage++;
			}
			//DrawPiexl(r,x,y,color2);
			if(temp & mask)
				DrawDot(x,y,0x01);
			else
				DrawDot(x,y,0x00);
			x++;
			mask >>= 1;
		}
		mask = 0;
		y ++;
	}
	return W;
}


const char _page[] = {0xb7,0xb6,0xb5,0xb4,0xb3,0xb2,0xb1,0xb0};
void LCD_Refresh(void)
{
	int __y;//,__x;
	char cmd[3];

	for (__y = 0; __y < 8; ++__y) {
		cmd[0] = _page[__y];
		cmd[1] = 0x10;
		cmd[2] = 0x00;

		LCD_CMD(cmd,3);

		LCD_DAT(&mDispBuffer[__y][0],X_SIZE);
	}
}
