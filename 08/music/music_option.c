#include "music_option.h"

int init_touch(void)
{
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
	int lcd = open("/dev/fb0", O_RDWR);
	ioctl(lcd, FBIOGET_VSCREENINFO, vinfo);
	int bpp = vinfo->bits_per_pixel;
	int screensize = vinfo->xres * vinfo->yres * bpp/8;
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
	struct stat info;
	bzero(&info, sizeof(info));
	stat(jpgpath, &info);
	char *jpg_buffer = readjpg(jpgpath, info.st_size);

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
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
	return 0;
}
void KillNoneLess(void){
	system("killall -KILL music_main");
	system("killall -KILL main_to_photp");
	system("killall -KILL main_to_camera");
	system("killall -KILL madplay");
}
void music_mainpage(void){
	int lcd = open("/dev/fb0", O_RDWR);
	// 2, 映射一块恰当大小的内存
	char *p = mmap(NULL, // 不指定映射内存的位置，让系统自动选取
		       800*480*4, // 映射内存的大小
		       PROT_READ | PROT_WRITE, // 以读、写模式操作映射内存
		       MAP_SHARED, // 其他线程、进程可以共享访问这块内存
		       lcd, // LCD文件描述符
		       0    // 映射内存与LCD的偏移量，0代表完全重合
		       );
	int bmp =  open("/hehe/main_music_ui.bmp", O_RDONLY);
	char bmp_buf[800*480*3];
	int ret = read(bmp, bmp_buf, sizeof(bmp_buf));
	int lcd_buf[800*480*4];
	// 4, 铺满整个映射内存
	for(int i=0; i<800*480; i++)
	{
		lcd_buf[i]=bmp_buf[i*3] | bmp_buf[i*3+1]<<8 | bmp_buf[i*3+2]<<16;
	}
	
	static int color_buff1[200*120*4];
	//颜色
	int a=0x00ff0000;
	//颜色buff
	for(int i=0; i<200*120; i++){
		color_buff1[i]=a;
	}
	//背景图
	for(int y=0; y<480; y++)
	{
		for(int x=0; x<800; x++)
		{
			memcpy(p+4*x+800*4*y, &lcd_buf[x+(479-y)*800], 4);
		}
	}
	munmap(p, 800*480*4);
	close(lcd);
}
















