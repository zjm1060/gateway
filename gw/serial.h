/*
 * serial.h
 *
 *  Created on: 2017Äê9ÔÂ12ÈÕ
 *      Author: zjm09
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <termios.h>

struct serial_conf{
	char *dev;
	int baud;
	char parity;
};

int serial_open(void *arg);
void serial_close(int fd);
int serial_read(int fd, void* buffer, size_t len, int timeout_ms);
int serial_write(int fd, void* buffer, size_t len);
int serial_wait(int fd,int timeout_ms);

#define IFLUSH	TCIFLUSH
#define OFLUSH	TCOFLUSH
#define IOFLUSH	TCIOFLUSH

#define serial_clear(fd,flag)	tcflush(fd,flag)

#endif /* SERIAL_H_ */
