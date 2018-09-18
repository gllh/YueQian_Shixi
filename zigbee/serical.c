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
#include <string.h>


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
	char message[7];
	while(1)
	{
		memset(message,0,6);
		read(ser_fd,message,6);
		printf("原始message=%s\n",message);

		if (strcmp("T25H69", message) == 0){
			printf("str_1 is equal to str_2. \n");
		}
		
		char tempture[2];
		strncpy(tempture, message, 2);
		int ha=atoi(tempture);
		printf("温度：%d\n",ha);
		

		char wet[2];
		strncpy(wet, message+2, 2);
		int he=atoi(wet);
		printf("湿度：%d\n",he );
		
		tcflush( ser_fd, TCIOFLUSH); //?§ä???
		mainpagetemp(ha*8);
		mainpagewet(he*8);
	}
	
}

	
int main(void)
{
	back();
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
void mainpagetemp(int te){
	int lcd = open("/dev/fb0", O_RDWR);

	// 2, 映射一块恰当大小的内存
	char *p = mmap(NULL, // 不指定映射内存的位置，让系统自动选取
		       800*480*4, // 映射内存的大小
		       PROT_READ | PROT_WRITE, // 以读、写模式操作映射内存
		       MAP_SHARED, // 其他线程、进程可以共享访问这块内存
		       lcd, // LCD文件描述符
		       0    // 映射内存与LCD的偏移量，0代表完全重合
		       );
	// 4, 铺满整个映射内存
	//颜色buff
	int color_buff1[te*29*4];
	//颜色
	int a=0x00ff0000;
	//颜色进缓冲区
	for(int i=0; i<te*30; i++){
		color_buff1[i]=a;
	}
	//左返回块
	for(int  y=0; y<30; y++)
		{
			for(int x=0; x<te; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff1[x+(29-y)*te], 4);
			}
		}
	// 5, 释放资源
	munmap(p, te*30*4);
	 // close(lcd);
}
void mainpagewet(int te){
	int lcd = open("/dev/fb0", O_RDWR);

	// 2, 映射一块恰当大小的内存
	char *p = mmap(NULL, // 不指定映射内存的位置，让系统自动选取
		       800*480*4, // 映射内存的大小
		       PROT_READ | PROT_WRITE, // 以读、写模式操作映射内存
		       MAP_SHARED, // 其他线程、进程可以共享访问这块内存
		       lcd, // LCD文件描述符
		       0    // 映射内存与LCD的偏移量，0代表完全重合
		       );
	// 4, 铺满整个映射内存
	//颜色buff
	int color_buff1[te*29*4];
	//颜色
	int a=0x00ff0000;
	//颜色进缓冲区
	for(int i=0; i<te*30; i++){
		color_buff1[i]=a;
	}
	//左返回块
	for(int  y=450; y<480; y++)
		{
			for(int x=0; x<te; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff1[x+(479-y)*te], 4);
			}
		}
	// 5, 释放资源
	munmap(p, te*30*4);
	 // close(lcd);
}
void back(){
	int lcd = open("/dev/fb0", O_RDWR);
	// 2, 映射一块恰当大小的内存
	char *p = mmap(NULL, // 不指定映射内存的位置，让系统自动选取
		       800*480*4, // 映射内存的大小
		       PROT_READ | PROT_WRITE, // 以读、写模式操作映射内存
		       MAP_SHARED, // 其他线程、进程可以共享访问这块内存
		       lcd, // LCD文件描述符
		       0    // 映射内存与LCD的偏移量，0代表完全重合
		       );

	int bmp =  open("/lyx/temp.bmp", O_RDONLY);
	
	char bmp_buf[800*480*3];
	
	int ret = read(bmp, bmp_buf, sizeof(bmp_buf));
	
	int lcd_buf[800*480*4];
	// 4, 铺满整个映射内存
	for(int i=0; i<800*480; i++)
	{
		lcd_buf[i]=bmp_buf[i*3] | bmp_buf[i*3+1]<<8 | bmp_buf[i*3+2]<<16;
	}
	for(int y=0; y<480; y++)
	{
		for(int x=0; x<800; x++)
		{
			memcpy(p+4*x+800*4*y, &lcd_buf[x+(479-y)*800], 4);
		}
	}
}
