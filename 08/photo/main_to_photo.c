#include "photo_option.h"

int main(int argc, char **argv){
	
	char buf[80];
	int vol = -30;
	photo_mainpage();
	struct get_abs_xy *touch = calloc(1, sizeof(struct get_abs_xy));
	touch->ts = init_touch();
	
	while(1)
	{
		photo_mainpage();
		pic_switch();
	}	
}

void photo_mainpage(){
	int lcd = open("/dev/fb0", O_RDWR);
	// 2, 映射一块恰当大小的内存
	char *p = mmap(NULL, // 不指定映射内存的位置，让系统自动选取
		       800*480*4, // 映射内存的大小
		       PROT_READ | PROT_WRITE, // 以读、写模式操作映射内存
		       MAP_SHARED, // 其他线程、进程可以共享访问这块内存
		       lcd, // LCD文件描述符
		       0    // 映射内存与LCD的偏移量，0代表完全重合
		       );
	//颜色buff
	static int color_buff1[200*120*4];
	//颜色
	int a=0x00ff0000;
	for(int i=0; i<200*120; i++){
		color_buff1[i]=a;
	}
	//左返回块
	for(int  y=0; y<120; y++)
		{
			for(int x=0; x<200; x++)
			{
				memcpy(p+4*x+800*4*y, &color_buff1[x+(119-y)*200], 4);
			}
		}
	munmap(p, 800*480*4);
}
void pic_switch(void){
	photo_mainpage();
	int x,y;
	char buf[80];
	struct fb_var_screeninfo my_vinfo;
	
	int i = 0;
	
	char *img_pathname[] = {"/hehe/image/12.jpg",
							"/hehe/image/1.bmp",
							"/hehe/image/2.jpg",
							"/hehe/image/3.bmp",
							"/hehe/image/4.jpg",
							"/hehe/image/5.bmp"};
	
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
		if(x>0 && x<400 && y>120 && y<480||x>200&&x<400&&y>0&&y<120)
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
				//s==================================================
				photo_mainpage();
				//s=============================
				display(img_pathname[i], //图片路径
								  p, //映射内存的地址
						   &my_vinfo, //lcd硬件参数
			                      0, //x轴的偏移量（相对于坐标原点）
								   0 //y轴的偏移量
				);
			}
			if(strstr(img_pathname[i],".jpg") || strstr(img_pathname[i],".jpeg"))
			{
				//s==================================================
				photo_mainpage();
				//s=============================
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
				//s==================================================
				photo_mainpage();
				//s=============================
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