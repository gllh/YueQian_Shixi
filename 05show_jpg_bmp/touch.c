#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <linux/input.h>

#include "bmp.h"
#include "jpg.h"

/*初始化LCD屏*/

char *init_lcd(struct fb_var_screeninfo *vinfo)
{
	//打开LCD屏
	int lcd = open("/dev/fb0", O_RDWR);
	
	//获取LCD屏硬件参数
	ioctl(lcd, FBIOGET_VSCREENINFO, vinfo);
	//LCD屏像素点的位深
	int bpp = vinfo->bits_per_pixel;
	//LCD屏可显示的像素点个素（字节）
	int screensize = vinfo->xres * vinfo->yres * bpp/8;
	//给LCD申请显存
	char *fbmem = mmap(NULL, screensize, PROT_READ|PROT_WRITE,MAP_SHARED, lcd, 0);
	if(fbmem == MAP_FAILED)
	{
		perror("映射显存失败");
		exit(0);
	}
	return fbmem;
}

/*判断参数是否正确*/
void usage(int argc, char **argv)
{
	if(argc != 4)
	{
		printf("参数不对\n");
		exit(0);
	}
}

int init_touch(void)
{
	//打开触摸屏
	int ts = open("/dev/input/event0", O_RDONLY);
	if(ts == -1)
	{
		perror("打开触摸屏失败：");
		exit(0);
	}
	return ts;
}

void get_xy_switch(int ts, int *x, int *y)
{
	
	struct input_event myevent;
	bzero(&myevent, sizeof(myevent));
	bool x_flag = false;
	bool y_flag = true;

	while(1)
	{
		read(ts, &myevent, sizeof(myevent));
		
		if(myevent.type == EV_ABS)
		{
			if(myevent.code==ABS_X && x_flag==false && y_flag == true)
			{
				*x = myevent.value;
				x_flag = true;
				y_flag = false;
			}

			if(myevent.code==ABS_Y && x_flag == true && y_flag == false)
			{
				*y = myevent.value;
				x_flag = false;
				y_flag = true;
			}

			if(myevent.code==ABS_PRESSURE && myevent.value==0)
				break;
		}
		if(myevent.type==EV_KEY && myevent.code==BTN_TOUCH && myevent.value==0)
			break;
		
	}

	
	return ;	
}


int main(int argc,char **argv)
// int main()
{
	int x,y;
	struct fb_var_screeninfo my_vinfo;
	
	int i = 0;
	
	char *img_pathname[] = {"/lyx/image/12.jpg",
							"/lyx/image/1.bmp",
							"/lyx/image/2.bmp",
							"/lyx/image/3.bmp",
							"/lyx/image/4.bmp",
							"/lyx/image/5.bmp"};
	
	//初始化LCD屏
	char *p = init_lcd(&my_vinfo);
	
	//初始化触摸屏
	int ts = init_touch();
	
	//先显示一张图片
	display(img_pathname[i], p, &my_vinfo, 0, 0);
	while(1)
	{
		//获取触摸屏坐标
		get_xy_switch(ts, &x, &y);
		printf("X:%d Y:%d\n", x, y);
		
		//对坐标点进行处理
		if(x>0 && x<100 && y>160 && y<320)
		{
			//重置坐标，为下一次获取做准备
			x = 0;
			y = 0;
			
			//计数
			i--;
			if(i<0)
				i=4;
			//显示
			if(strstr(img_pathname[i],".bmp"))
			{
				display(img_pathname[i], //图片路径
								  p, //映射内存的地址
						   &my_vinfo, //lcd硬件参数
			                      0, //x轴的偏移量（相对于坐标原点）
								   0 //y轴的偏移量
				);
			}
			if(strstr(img_pathname[i],".jpg") || strstr(img_pathname[i],".jpeg"))
			{
				show_jpg(img_pathname[i]);
			}
		}
		if(x>700 && x<800 && y>160 && y<320)
		{
			x = 0;
			y = 0;
			i++;
			if(i>4)
				i=0;
			if(strstr(img_pathname[i],".bmp"))
			{
				display(img_pathname[i], //图片路径
								  p, //映射内存的地址
						   &my_vinfo, //lcd硬件参数
			                      0, //x轴的偏移量（相对于坐标原点）
								   0 //y轴的偏移量
				);
			}
			if(strstr(img_pathname[i],".jpg") || strstr(img_pathname[i],".jpeg"))
			{
				show_jpg(img_pathname[i]);
			}
		}
	}
	//关闭触摸屏
	close(ts);
	
	return 0;
}





















