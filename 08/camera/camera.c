//包含摄像头及其他头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include "lcdjpg.h"
#include "api_v4l2.h" 
#include "camera_option.h"

int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half);

int iphonex;
struct sockaddr_in phoneaddr;
//线程的功能函数，接收来自手机app发送的控制命令
void *myrecvmsg(void *arg)
{
	char buf[200];
	int addrsize=sizeof(phoneaddr);
	while(1)
	{
		bzero(buf,200);//清零
		//聊天，收发数据，接收手机发送过来的控制命令
		recvfrom(iphonex,buf,200,0,(struct sockaddr *)&phoneaddr,&addrsize);
		printf("phone send msg is:%s ip:%s  port:%hu\n",buf,inet_ntoa(phoneaddr.sin_addr),ntohs(phoneaddr.sin_port));
	}
}
void *get_abs(void *arg)
{
	struct get_abs_xy *p = (struct get_abs_xy *)arg;
	
	while(1)
	{
		get_xy(p);
	}
}
void show_cam(void)
{
	shoot_ui();
	///初始化获取坐标结构体、
	struct get_abs_xy *tmp = calloc(1, sizeof(struct get_abs_xy));
	tmp->x=0;
	tmp->y=0;
	tmp->ts=init_touch();
	//买手机--->创建套接字
	iphonex = socket( AF_INET, SOCK_DGRAM,0);
	
	// 定义一个ipv4地址结构体变量
	struct sockaddr_in bindaddr;
	bindaddr.sin_family = AF_INET; 
	bindaddr.sin_addr.s_addr = inet_addr("192.168.0.66");//绑定开发板无线网卡的ip
	bindaddr.sin_port = htons(2234); //绑定端口号
	//复用？？？？？？？？？？？？？？？？？？？？？？
	int opt = 1;
	// setsockopt( iphonex, SOL_SOCKET,SO_REUSEADDR, 
	// 				(const void *)&opt, sizeof(opt) );
	//买卡--->绑定自己的ip
	int ret = bind(iphonex, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
	if(ret == -1)
	{
		printf("绑定失败！\n");
		exit(0);
	}
	//打开摄像头
	linux_v4l2_device_init("/dev/video0");
	//启动摄像捕捉
	linux_v4l2_start_capturing();
	//文件++
	int photo_id=0;
	//初始化获取坐标结构体、
	struct get_abs_xy get_x_y;
	get_x_y.x=0;
	get_x_y.y=0;
	get_x_y.ts=init_touch();

	//创建线程，专门获取触摸屏坐标
	pthread_t ts_pid;
	pthread_create( &ts_pid, NULL, get_abs, (void *)&get_x_y);
	printf("xxxxxx%d\n",get_x_y.x );
	sleep(1);

	//创建线程，专门接受手机端发送过来的信息
	pthread_t recv_pid;
	pthread_create( &recv_pid, NULL, myrecvmsg, NULL);
	sleep(1);
	
	FrameBuffer mybuf;
	while(1)
	{	
		//开始捕捉
		linux_v4l2_get_fream(&mybuf);
		//开发板显示摄像头拍摄的画面
		lcd_draw_jpg(0,0,NULL,mybuf.buf,mybuf.length,0);
		//111111将实时画面传输给手机
		sendto(iphonex,mybuf.buf,mybuf.length,0,(struct sockaddr *)&phoneaddr,sizeof(phoneaddr));
		printf("X: %d Y: %d \n", get_x_y.x, get_x_y.y);
		char pathstart[]={"./camera"};
		char pathend[]={".jpg"};
		//拍照///////////////////////////////////////////////////////////////////////////	 
		if(get_x_y.x>610 && get_x_y.x<800 && get_x_y.y>0 && get_x_y.y<480)
		{
			photo_id++;
			get_x_y.x=0;
			get_x_y.y=0;
			int fd = open("./camera.jpg", O_RDWR | O_CREAT | O_TRUNC, 0666);
			// int fd = open(pathstart, O_RDWR | O_CREAT | O_TRUNC, 0666);
			if(fd<0)
			{
				perror("拍照失败！");
				exit(0);
			}
			write(fd, mybuf.buf, mybuf.length);
			printf("拍完了\n");
		}
		if(get_x_y.x>0 && get_x_y.x<200 && get_x_y.y>0 && get_x_y.y<120)
		{
			close(iphonex);
			int linux_v4l2_stop_capturing();
			int linux_v4l2_device_uinit();
			char buf[80];
			sprintf(buf, "/hehe/project_main");
			system(buf);
			exit(1);
		}
	}
}
void shoot_ui(void){
	int lcd = open("/dev/fb0", O_RDWR);
	if(lcd == -1)
	{
		perror("打开LCD失败");	
		exit(0);
	}
	// 2, 映射一块恰当大小的内存
	char *p = mmap(NULL, // 不指定映射内存的位置，让系统自动选取
		       800*480*4, // 映射内存的大小
		       PROT_READ | PROT_WRITE, // 以读、写模式操作映射内存
		       MAP_SHARED, // 其他线程、进程可以共享访问这块内存
		       lcd, // LCD文件描述符
		       0    // 映射内存与LCD的偏移量，0代表完全重合
		       );
	int lcd_buf[800*480*4];
	// 4, 铺满整个映射内存
	//颜色buff
	static int color_buff1[200*120*4];
	//颜色
	int a=0x00000000;
	for(int i=0; i<200*120; i++){
		color_buff1[i]=a;
	}
	//右四块
		for(int  y=0; y<480; y++)
		{
			for(int x=0; x<800; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff1[x+(479-y)*200], 4);
			}
		}
		
	// 5, 释放资源
	munmap(p, 800*480*4);
}