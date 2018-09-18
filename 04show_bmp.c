#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>

#include "04bmp.h"

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

int main(int argc, char **argv) // ./showimg [image] xoffset yoffset 
{
	//判断参数是否正确
	usage(argc, argv);
	
	//定义lcd硬件参数结构体
	struct fb_var_screeninfo vinfo;
	bzero(&vinfo, sizeof(vinfo));
	//初始化LCD屏
	char *fbmem = init_lcd(&vinfo);
	
	//按要求显示bmp图片
	display(argv[1], fbmem, &vinfo, atoi(argv[2]), atoi(argv[3]));

	return 0;
}
