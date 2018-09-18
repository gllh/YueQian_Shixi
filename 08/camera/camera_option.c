#include "camera_option.h"

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

void get_xy(struct get_abs_xy *p){	
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
void KillNoneLess(void){
	system("killall -KILL project_main");
	system("killall -KILL main_to_photo");
	system("killall -KILL main_to_music");
	system("killall -KILL madplay");
}