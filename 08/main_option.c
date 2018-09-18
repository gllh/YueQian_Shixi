#include "main_option.h"

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
void get_xy(struct get_abs_xy *p)
{
	struct input_event myevent;
	bzero(&myevent, sizeof(myevent));
	bool x_flag = false;
	bool y_flag = true;
	while(1)
	{
		read(p->ts, &myevent, sizeof(myevent));
		if(myevent.type == EV_ABS)
		{
			if(myevent.code==ABS_X && x_flag==false && y_flag == true)
			{
				p->x = myevent.value;
				x_flag = true;
				y_flag = false;
			}
			if(myevent.code==ABS_Y && x_flag == true && y_flag == false)
			{
				p->y = myevent.value;
				x_flag = false;
				y_flag = true;
			}
			printf("X: %d Y:%d\n", p->x, p->y);
			if(myevent.code==ABS_PRESSURE && myevent.value==0)
				break;
		}
		if(myevent.type==EV_KEY && myevent.code==BTN_TOUCH && myevent.value==0)
			break;
	}
	return ;	
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
void usage(int argc, char **argv)
{
	if(argc != 4)
	{
		printf("参数不对\n");
		exit(0);
	}
}
int show_jpg(char *jpgpath) // ./showjpg xxx.jpg
{
	//获取文件属性
	struct stat info;
	bzero(&info, sizeof(info));
	stat(jpgpath, &info);
	
	//读取JPEG文件
	char *jpg_buffer = readjpg(jpgpath, info.st_size);

	// 声明解压缩结构体，以及错误管理结构体
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// 使用缺省的出错处理来初始化解压缩结构体
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	jpeg_mem_src(&cinfo, jpg_buffer, info.st_size);

	int ret = jpeg_read_header(&cinfo, true);
	if(ret != 1)
	{
		fprintf(stderr, "[%d]: jpeg_read_header failed: "
			"%s\n", __LINE__, strerror(errno));
		exit(1);
	}
	jpeg_start_decompress(&cinfo);

	struct imginfo imageinfo;
	imageinfo.width      = cinfo.output_width;
	imageinfo.height     = cinfo.output_height;
	imageinfo.pixel_size = cinfo.output_components;

	int row_stride = imageinfo.width * imageinfo.pixel_size;

	unsigned long rgb_size;
	unsigned char *rgb_buffer;
	rgb_size   = imageinfo.width * imageinfo.height * imageinfo.pixel_size;
	rgb_buffer = (unsigned char *)calloc(1, rgb_size);

	int line = 0;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = rgb_buffer +
				(cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

 	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);

	int lcd = open("/dev/fb0", O_RDWR);

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
	return 0;
}
void KillNoneLess(void){
	system("killall -KILL main_to_photo");
	system("killall -KILL main_to_camera");
	system("killall -KILL madplay");
	system("killall -KILL main_to_serical");
	system("killall -KILL main_to_music");
}
void mainpage(){
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
	int bmp =  open("/hehe/main.bmp", O_RDONLY);
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
	static int color_buff1[200*120*4];
	static int color_buff2[200*120*4];
	static int color_buff3[200*120*4];
	static int color_buff4[200*120*4];
	//颜色
	int a=0x00ff0000;
	int b=0x0000ff00;
	int c=0x000000ff;
	int d=0x00000000;
	//颜色进缓冲区
	for(int i=0; i<200*120; i++){
		color_buff1[i]=a;
		color_buff2[i]=b;
		color_buff3[i]=c;
		color_buff4[i]=d;
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
}
void pic_switch(void){
	int x,y;
	char buf[80];
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
		if(x>200 && x<400 && y>120 && y<480)
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
		if(x>400 && x<800 && y>0 && y<480)
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
		if (x>0&&x<200&&y>0&&y<120)
		{
			printf("此处相册返回\n");
			bzero(buf, sizeof(buf));
			sprintf(buf, "/hehe/project_main");
			system(buf);
		}
	}
	//关闭触摸屏
	close(ts);
}
void Strat_Engine_Hehe(void){
	KillNoneLess();
	char buf[80];
	int vol = -30;
	mainpage();
	struct get_abs_xy *touch = calloc(1, sizeof(struct get_abs_xy));
	touch->ts = init_touch();
	
	while(1)
	{
		get_xy(touch);
		printf("X: %d Y:%d\n", touch->x, touch->y);
		//左1----杀音乐
		if(touch->x>0 && touch->x<200 && touch->y>0 && touch->y<120)
		{
			touch->x=0;
			touch->y=0;
			system("killall -KILL madplay");
			sprintf(buf, "/hehe/project_main");
			system(buf);
		}
		//右1----相册
		if(touch->x>600 && touch->x<800 && touch->y>0 && touch->y<120)
		{
			touch->x=0; 
			touch->y=0;
			bzero(buf, sizeof(buf));
			system("killall -KILL madplay");
			printf("aaaaaaaaaaaaaa\n");
			system("killall -KILL madplay");
			sprintf(buf, "/hehe/main_to_photo");
			system(buf);
			printf("111111111111111111111111\n");
		}
		//右2----音乐
		if(touch->x>600 && touch->x<800 && touch->y>120 && touch->y<240)
		{
			touch->x=0; 
			touch->y=0;
			bzero(buf, sizeof(buf));
		    sprintf(buf, "/hehe/main_to_music");
			system(buf);
		}
		//右3---摄像头
		if(touch->x>600 && touch->x<800 && touch->y>240 && touch->y<360)
		{
			printf("准备摄像头\n");
			touch->x=0; 
			touch->y=0;
			system("killall -KILL madplay");
			system("killall -KILL main_to_photo");
			bzero(buf, sizeof(buf));
		    sprintf(buf, "/hehe/main_to_camera");
			system(buf);
			printf("摄像头部分程序走完\n");
		}
		if(touch->x>600 && touch->x<800 && touch->y>360 && touch->y<480)
		{
			touch->x=0;
			touch->y=0;
			bzero(buf, sizeof(buf));
			system("killall -KILL madplay");
			system("killall -KILL main_to_photo");
			bzero(buf, sizeof(buf));
		    sprintf(buf, "/hehe/main_to_serical");
			system(buf);
		}
	}	
}