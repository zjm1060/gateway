/*
 * serial.c
 *
 *  Created on: 2017年8月30日
 *      Author: zjm09
 */
#include "includes.h"
#include "serial.h"

int serial_open(void *arg)
{
	struct serial_conf *ser = arg;
	struct termios newtio;

	int fd = open(ser->dev,O_RDWR);
	if(fd == -1){
		perror("Serial open");
		return -1;
	}

	tcgetattr(fd,&newtio);
	bzero(&newtio,sizeof(newtio));
	cfsetispeed(&newtio,ser->baud);
	cfsetospeed(&newtio,ser->baud);

    newtio.c_cflag |=CLOCAL|CREAD;
	newtio.c_cflag &= ~CSIZE; //* Mask the character size bits

	switch(ser->parity){
		default:
		case 'n':
			newtio.c_cflag &= ~PARENB;   //Clear parity enable
			newtio.c_iflag &= ~INPCK;   //Enable parity checking
			break;
		case 'o':
			newtio.c_cflag |= (PARODD | PARENB);
			newtio.c_iflag |= INPCK;             //Disnable parity checking
			break;
		case 'e':
			newtio.c_cflag |= PARENB;     /* Enable parity */
			newtio.c_cflag &= ~PARODD;   /* 转换为偶效验*/
			newtio.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 's':
			newtio.c_cflag &= ~PARENB;
			newtio.c_cflag &= ~CSTOPB;
			newtio.c_iflag |= INPCK;
			break;
	}


	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag |= CS8;

	newtio.c_cflag &= ~CSTOPB;

    newtio.c_cflag &= ~CRTSCTS;//disable hardware flow control;
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//*raw input
    newtio.c_oflag  &= ~OPOST;   //*raw output
    tcflush(fd,TCIFLUSH);		//clear input buffer
    newtio.c_cc[VTIME] = 10; //* inter-character timer unused
    newtio.c_cc[VMIN] = 0; //* blocking read until 0 character arrives

	tcsetattr(fd,TCSANOW,&newtio);

	return fd;
}

void serial_close(int fd)
{
	close(fd);
}

int serial_read(int fd, void* b, size_t len, int timeout_ms)
{
	fd_set set;
	struct timeval timeout;
	int rv;
	uint8_t *buffer = b;

	FD_ZERO(&set); /* clear the set */
	FD_SET(fd, &set); /* add our file descriptor to the set */

	timeout.tv_sec = timeout_ms/1000;
	timeout.tv_usec = (timeout_ms%1000)*1000;

	int bytes = 0;

	while(bytes < len){
		rv = select(fd + 1, &set, NULL, NULL, &timeout);
		if(rv == -1){
			return -1;
		}else if(rv == 0){
			return bytes;
		}else{
			rv = read( fd, &buffer[bytes], (size_t)(len - bytes) );
			bytes += rv;
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = 25*1000;
	}

	return bytes;
}

int serial_write(int fd, void* buffer, size_t len)
{
	return write(fd,buffer,len);
}

int serial_wait(int fd,int timeout_ms)
{
	fd_set read_set;
	struct timeval tv;

	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	FD_ZERO(&read_set);
	FD_SET(fd,&read_set);

	int rc = select((int) fd + 1, &read_set, NULL, NULL, &tv);
	if(rc > 0 && FD_ISSET(fd, &read_set)){
		return 1;
	}

	return -1;
}
