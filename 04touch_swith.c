#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <linux/input.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "jpeglib.h"

struct imginfo
{
	int width;
	int height;
	int pixel_size;
};

int main(void)
{
	int x,y;
	//打开触摸屏
	int ts = open("/dev/input/event0", O_RDONLY);
	if(ts == -1){
		perror("打开触摸屏失败：");
		exit(0);
	}
	
	struct input_event myevent;
	bzero(&myevent, sizeof(myevent));
	bool x_flag = false;
	bool y_flag = true;

	while(1){
		int loop=1;
		read(ts, &myevent, sizeof(myevent));
		if(myevent.type == EV_ABS)
		{
			if(myevent.code==ABS_X && x_flag==false && y_flag == true)
			{
				x = myevent.value;
				x_flag = true;
				y_flag = false;
			}

			if(myevent.code==ABS_Y && x_flag == true && y_flag == false)
			{
				y = myevent.value;
				x_flag = false;
				y_flag = true;
			}

			if(myevent.code==ABS_PRESSURE && myevent.value==0)
				break;
		}
		if(myevent.type==EV_KEY && myevent.code==BTN_TOUCH && myevent.value==0)
			break;
		if(x<400){
			loop--;
			if(loop==-1){
				loop=4;
			}
			ChangePicture(loop);
		}
		else{
			loop++;
			if(loop=5){
				loop=0;
			}
			ChangePicture(loop);
		}		
	}
	close(ts);
	return 0;	
}
void ChangePicture(int i){
	char *pathname={"./pics/1.bmp",
					"./pics/2.jpg",
					"./pics/3.bmp",
					"./pics/4.jpg",
					"./pics/5.bmp",}
		if(strstr(pathname[i],"bmp")!=NULL){
			Show_Bmp(pathname[i]);
		}
		if(strstr(pathname[i],"jpg")!=NULL){
			Show_Jpg(pathname[i]);
		}
}
//bmp
void Show_Bmp(char path){
	int lcd = open("/dev/fb0", O_RDWR);

	// 2, 映射一块恰当大小的内存
	char *p = mmap(NULL, // 不指定映射内存的位置，让系统自动选取
		       800*480*4, // 映射内存的大小
		       PROT_READ | PROT_WRITE, // 以读、写模式操作映射内存
		       MAP_SHARED, // 其他线程、进程可以共享访问这块内存
		       lcd, // LCD文件描述符
		       0    // 映射内存与LCD的偏移量，0代表完全重合
		       );

	// 3, 准备数据
	
	int bmp =  open(path, O_RDONLY);
	
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
	int lcd_buf[800*480];
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

	// 5, 释放资源
	munmap(p, 800*480*4);
	close(lcd);
}
//read jpg
char *readjpg(char const *filename, int size)
{
	char *jpg_buffer = calloc(1, size);

	// 不断读取JPG信息，放入jpg_buffer
	int fd = open(filename, O_RDONLY);

	int n = 0;
	while(size > 0)
	{
		n = read(fd, jpg_buffer+n, size);	

		size -= n;
	}

	close(fd);
	return jpg_buffer;
}
void Show_Jpg(char path){
	struct stat info;
	bzero(&info, sizeof(info));
	stat(path, &info);

	char *jpg_buffer = readjpg(path, info.st_size);

	// 声明解压缩结构体，以及错误管理结构体
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// 使用缺省的出错处理来初始化解压缩结构体
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// 配置该cinfo，使其从jpg_buffer中读取jpg_size个字节
	// 这些数据必须是完整的JPEG数据
	jpeg_mem_src(&cinfo, jpg_buffer, info.st_size);


	// 读取JPEG文件的头，并判断其格式是否合法
	int ret = jpeg_read_header(&cinfo, true);
	if(ret != 1)
	{
		fprintf(stderr, "[%d]: jpeg_read_header failed: "
			"%s\n", __LINE__, strerror(errno));
		exit(1);
	}

	// 开始解压
	jpeg_start_decompress(&cinfo);

	// 将图片的宽、高、深度信息保存起来
	struct imginfo imageinfo;
	imageinfo.width      = cinfo.output_width;
	imageinfo.height     = cinfo.output_height;
	imageinfo.pixel_size = cinfo.output_components;

	int row_stride = imageinfo.width * imageinfo.pixel_size;

	// 根据图片的尺寸大小，分配一块相应的内存bmp_buffer
	// 用来存放从jpg_buffer解压出来的图像数据
	unsigned long rgb_size;
	unsigned char *rgb_buffer;
	rgb_size   = imageinfo.width * imageinfo.height * imageinfo.pixel_size;
	rgb_buffer = (unsigned char *)calloc(1, rgb_size);

	// 循环地将图片的每一行读出并解压到bmp_buffer中
	int line = 0;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = rgb_buffer +
				(cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

	// 解压完了，将jpeg相关的资源释放掉
 	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);

	// 将rgb_buffer中的数据，妥善地放到lcd上去显示
	int lcd = open("/dev/fb0", O_RDWR);


	// 获取LCD的屏幕参数信息
	struct fb_var_screeninfo vinfo;
	bzero(&vinfo, sizeof(vinfo));
	ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);

	int lcd_w = vinfo.xres;
	int lcd_h = vinfo.yres;
	int lcd_bpp = vinfo.bits_per_pixel;

	char *p = mmap(NULL, lcd_w * lcd_h * lcd_bpp/8, PROT_READ|PROT_WRITE,
			MAP_SHARED, lcd, 0);


	int w = imageinfo.width;
	int h = imageinfo.height;

	for(int j=0; j<480 && j<h; j++)
	{
		for(int i=0; i<800 && i<w; i++)
		{
			memcpy(p + 4*i + 800*4*j+vinfo.red.offset/8,  rgb_buffer + 3*i + w*3*j+0, 1);
			memcpy(p + 4*i + 800*4*j+vinfo.green.offset/8,rgb_buffer + 3*i + w*3*j+1, 1);
			memcpy(p + 4*i + 800*4*j+vinfo.blue.offset/8, rgb_buffer + 3*i + w*3*j+2, 1);
		}
	}

	munmap(p, 800*480*4);
	free(rgb_buffer);
	close(lcd);
}