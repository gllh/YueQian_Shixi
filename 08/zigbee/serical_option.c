#include "serical_option.h"
int ser_fd;

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
int set_serial_uart1(int ser_fd)
{

	struct termios termios_new;
	bzero( &termios_new, sizeof(termios_new));
	cfmakeraw(&termios_new);

	termios_new.c_cflag=(B115200);
	termios_new.c_cflag |= CLOCAL | CREAD;
	termios_new.c_cflag &= ~CSIZE;
	termios_new.c_cflag |= CS8;
	termios_new.c_cflag &= ~PARENB;
	termios_new.c_cflag &= ~CSTOPB;

	tcflush( ser_fd,TCIOFLUSH);
	termios_new.c_cc[VTIME] = 0;	
	termios_new.c_cc[VMIN] = 4;	
	tcflush ( ser_fd, TCIOFLUSH);

	tcsetattr( ser_fd ,TCSANOW,&termios_new);
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
	char *p = mmap(NULL, 
		       800*480*4,
		       PROT_READ | PROT_WRITE, 
		       MAP_SHARED, 
		       lcd, 
		       0    
		       );
	int color_buff1[te*29*4];
	int a=0x000000ff;
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
	munmap(p, te*30*4);
}
//serical背景图
void Back_Ground(){
	int lcd = open("/dev/fb0", O_RDWR);
	char *p = mmap(NULL, 
		       800*480*4,
		       PROT_READ | PROT_WRITE, 
		       MAP_SHARED, 
		       lcd, 
		       0    
		       );
	int bmp =  open("/hehe/temp.bmp", O_RDONLY);
	char bmp_buf[800*480*3];
	int ret = read(bmp, bmp_buf, sizeof(bmp_buf));
	int lcd_buf[800*480*4];
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
//杀无关确保正常
void KillNoneLess(void){
	system("killall -KILL main_to_music");
	system("killall -KILL project_main");
	system("killall -KILL main_to_photp");
	system("killall -KILL main_to_camera");
	system("killall -KILL madplay");
}
//接收温湿度及转换integer型
void *fun(void *arg)
{
	char buf1[80];
	struct get_abs_xy *touch = calloc(1, sizeof(struct get_abs_xy));
	touch->ts = init_touch();
	char message[7];
	printf("kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk\n");
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
		
		tcflush( ser_fd, TCIOFLUSH); 
		mainpagetemp(ha*8);
		mainpagewet(he*8);
	}
}	