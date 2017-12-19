/*
 * Font.h
 *
 *  Created on: 2011-10-17
 *      Author: zjm
 */

#ifndef FONT_H_
#define FONT_H_

typedef struct{
	int w;
	int h;
	const unsigned char *symbol;
}tSymbol;

typedef struct __Font{
	int w;
	int h;
	short *tab_ascii;
	const unsigned char *symbol_ascii;
	short *tab_hz;
	unsigned char *symbol_hz;

	int (*GetSymbol)(struct __Font *,unsigned short,tSymbol *);
	int (*GetSymbolWidth)(struct __Font *,unsigned short);
	int (*GetSymbolHight)(struct __Font *,unsigned short);
	int (*GetSymbolMaxHight)(struct __Font *);
}tFont;

extern tFont Font_6X12;
extern tFont Font_12x24;

#endif /* FONT_H_ */
