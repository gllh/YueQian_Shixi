#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// //打开lcd判断
// int Open(const char *pathname, int flag){
// 	int lcd = open(pathname, O_RDWR);

// 	if(lcd == -1)
// 	{
// 		perror("打开LCD失败");	
// 		exit(0);
// 	}
// 	return lcd;
// }
// //参数判断
// void canshu(int argc, char **argv)
// {
// 	if(argc < 2)
// 	{
// 		printf("参数不对！\n");
// 		exit(0);
// 	}
// }
// //打开图片判断
// int Open_Pic(int argc,char **argv)
// {
// 	int bmp =  open(argv[1], O_RDONLY);
// 	if(bmp == -1)
// 	{
// 		perror("打开图片失败！");
// 		exit(0);	
// 	}
// }


int main(int argc, char **argv)
{
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

	// 3, 准备数据
	// if(argc < 2)
	// {
	// 	printf("参数不对！\n");
	// 	close(lcd);
	// 	munmap(p, 800*480*4);
	// 	exit(0);
	// }
	///////////////////////改路径////////////////////////////////////////////
	int bmp =  open("./1.bmp", O_RDONLY);
	if(bmp == -1)
	{
		perror("打开图片失败！");
		close(lcd);
		munmap(p, 800*480*4);
		exit(0);	
	}
	char bmp_buf[800*480*3];
	
	int ret = read(bmp, bmp_buf, sizeof(bmp_buf));
	if(ret <= 0)
	{
		perror("读取bmp数据失败！");
		close(lcd);
		close(bmp);
		munmap(p, 800*480*4);
		exit(0);
	}
	int lcd_buf[800*480*4];
	// 4, 铺满整个映射内存
	for(int i=0; i<800*480; i++)
	{
		lcd_buf[i]=bmp_buf[i*3] | bmp_buf[i*3+1]<<8 | bmp_buf[i*3+2]<<16;
	}
	//颜色buff
	// int color_buff[200*120*4][4];
	static int color_buff1[200*120*4];
	static int color_buff2[200*120*4];
	static int color_buff3[200*120*4];
	static int color_buff4[200*120*4];
	//颜色
	int a=0x00ff0000;
	int b=0x0000ff00;
	int c=0x000000ff;
	int d=0x00000000;
	// 
	// for(int i=0; i<200*120; i++)
	// {
	// 	for(int j=0;j<4;j++){
	// 		color_buff[i][j]=b[j];
	// 	}	
	// }
	for(int i=0; i<200*120; i++){
		color_buff1[i]=a;
		color_buff2[i]=b;
		color_buff3[i]=c;
		color_buff4[i]=d;
	}
	// for(int i=0; i<200*120; i++){
	// 	color_buff2[i]=b;
	// }
	// for(int i=0; i<200*120; i++){
	// 	color_buff3[i]=c;
	// }
	// for(int i=0; i<200*120; i++){
	// 	color_buff4[i]=d;
	// }
	for(int y=0; y<480; y++)
	{
		for(int x=0; x<800; x++)
		{
			memcpy(p+4*x+800*4*y, &lcd_buf[x+(479-y)*800], 4);
		}
	}
	//左返回块
	for(int  y=0; y<120; y++)
		{
			for(int x=0; x<200; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff1[x+(119-y)*200], 4);
			}
		}
	//右四块
		for(int  y=360; y<480; y++)
		{
			for(int x=600; x<800; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff4[x+(479-y)*200], 4);
			}
		}
		for(int y=240; y<360; y++)
		{
			for(int x=600; x<800; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff3[x+(350-y)*200], 4);
			}
		}
		for(int y=120; y<240; y++)
		{
			for(int x=600; x<800; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff2[x+(239-y)*200], 4);
			}
		}
		for(int y=0; y<120; y++)
		{
			for(int x=600; x<800; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff1[x+(119-y)*200], 4);
			}
		}
	// 5, 释放资源
	munmap(p, 800*480*4);
	close(lcd);

	return 0;	
}

