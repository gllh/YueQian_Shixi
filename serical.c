#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


#define DEVICE_NAME "/dev/ttySAC1"
int ser_fd;
int set_serial_uart1(int ser_fd)
{

	struct termios termios_new;
	bzero( &termios_new, sizeof(termios_new));
	/*????  RAW??*/
	cfmakeraw(&termios_new);
	/*
	termios_new->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP| INLCR | IGNCR | ICRNL | IXON);
	//??????¨¬????
	termios_new->c_oflag &= ~OPOST;
	termios_new->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	
	//???§µ?b8¦Ë??¦Ë
	termios_new->c_cflag &= ~(CSIZE | PARENB);
	termios_new->c_cflag |= CS8;
	*/
	
	/*??¨º?115200*/
	termios_new.c_cflag=(B115200);
	termios_new.c_cflag |= CLOCAL | CREAD;
	
	/*8¦Ë??¦Ë*/
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8;

	/*???§µ?¦Ë*/
	termios_new.c_cflag &= ~PARENB;

	/*1¦Ë??¦Ë*/
	termios_new.c_cflag &= ~CSTOPB;
	/*???????*/
	tcflush( ser_fd,TCIOFLUSH);
	termios_new.c_cc[VTIME] = 0;	//?????
	termios_new.c_cc[VMIN] = 4;	//???????
	tcflush ( ser_fd, TCIOFLUSH);
	/*??????*/
	tcsetattr( ser_fd ,TCSANOW,&termios_new);
}

void *fun(void *arg)
{
	char message[6];
	while(1)
	{
		memset(message,0,6);
		read(ser_fd,message,6);
		printf("message = %s\n",message);
		tcflush( ser_fd, TCIOFLUSH); //?§ä???
	}
	
}
	
int main(void)
{
	
	//????
	ser_fd = open(DEVICE_NAME,O_RDWR);
	if(ser_fd == -1)
	{
		printf("serical open failed!\n");
		return -1;
	}
	set_serial_uart1(ser_fd);
	
	if(fcntl(ser_fd,F_SETFL,0)<0)	//??¡¦??????
	{
		exit(1);
	}
	tcdrain(ser_fd);   //??????
	tcflush( ser_fd, TCIOFLUSH); //?§ä???
	
	unsigned char buf[4]={0};
	unsigned char count=0;
	pthread_t thread;
	pthread_create(&thread,NULL,fun,NULL);
	while(1)
	{
		write(ser_fd,"11",3);
		sleep(1);
		write(ser_fd,"10",3);
		sleep(1);
		write(ser_fd,"21",3);
		sleep(1);
		write(ser_fd,"20",3);
		sleep(1);
	}
	
	close(ser_fd);
	
}
