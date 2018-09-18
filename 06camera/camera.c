#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

#include "lcdjpg.h"
#include "api_v4l2.h" //包含摄像头的头文件

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


int main(void)
{
	//买手机--->创建套接字
	iphonex = socket( AF_INET, SOCK_DGRAM,0);
	
	
	// 定义一个ipv4地址结构体变量
	struct sockaddr_in bindaddr;
	bindaddr.sin_family = AF_INET; 
	bindaddr.sin_addr.s_addr = inet_addr("192.168.0.66");//绑定开发板无线网卡的ip
	bindaddr.sin_port = htons(2234); //绑定端口号
	
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
	//开始捕捉
	
	//创建线程，专门接受手机端发送过来的信息
	pthread_t p_id;
	pthread_create( &p_id, NULL, myrecvmsg, NULL);
	
	FrameBuffer mybuf;
	while(1)
	{	
		linux_v4l2_get_fream(&mybuf);
		
		//开发板显示摄像头拍摄的画面
		lcd_draw_jpg(0,0,NULL,mybuf.buf,mybuf.length,0);
		//将画面传输给手机
		sendto(iphonex,mybuf.buf,mybuf.length,0,(struct sockaddr *)&phoneaddr,sizeof(phoneaddr));
		printf("phone ip:%s  port:%hu\n",inet_ntoa(phoneaddr.sin_addr),ntohs(phoneaddr.sin_port));	
		//拍照....                                
	}
	return 0;
}