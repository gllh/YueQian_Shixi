#include "touch.h"

//之后改程序名 music_main == >
//参数取消
//歌名代替
int main(int argc, char **argv){
	system("killall -KILL main_to_photo");
	system("killall -KILL main_to_camera");
	system("killall -KILL madplay");
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
			////
			int ts = init_touch();
			struct fb_var_screeninfo my_vinfo1;
			/////
			char *p = init_lcd(&my_vinfo1);
			system("killall -KILL madplay");
			// system("killall -KILL music_main");
			sprintf(buf, "/hehe/music_main %s -a %d &", "/lyx/SeeYouAgain.mp3", -30);
			system(buf);
		}
		//右1----相册
		if(touch->x>600 && touch->x<800 && touch->y>0 && touch->y<120)
		{
			touch->x=0; 
			touch->y=0;
			////
			int ts = init_touch();
			struct fb_var_screeninfo my_vinfo2;
			char *p = init_lcd(&my_vinfo2);
			///
			bzero(buf, sizeof(buf));
			system("killall -KILL madplay");
			printf("aaaaaaaaaaaaaa\n");
			system("killall -KILL madplay");
			//pic_switch();
			sprintf(buf, "/hehe/main_to_photo");
			system(buf);
			printf("111111111111111111111111\n");

			// sprintf(buf, "/lyx/madplay %s -a %d &", argv[1], vol);
			//printf("/llf/chongjiao/madplay %s -a %d\n", argc[1], vol);
			// system(buf);
		}
		//右2----音乐
		if(touch->x>600 && touch->x<800 && touch->y>120 && touch->y<240)
		{
			touch->x=0; 
			touch->y=0;
			bzero(buf, sizeof(buf));
			// sprintf(buf, "/hehe/main_to_photo");
			// system(buf);
		    sprintf(buf, "/lyx/madplay %s -a %d &", argv[1], vol);
			system(buf);
		}
		//右3---摄像头
		if(touch->x>600 && touch->x<800 && touch->y>240 && touch->y<360)
		{
			printf("准备摄像头\n");
			touch->x=0; 
			touch->y=0;

			////
			int ts = init_touch();
			struct fb_var_screeninfo my_vinfo2;
			char *p = init_lcd(&my_vinfo2);
			///
			system("killall -KILL madplay");
			system("killall -KILL main_to_photo");
			bzero(buf, sizeof(buf));
		    sprintf(buf, "/hehe/main_to_camera");
			system(buf);
			printf("摄像头部分程序走完\n");
		}
		if(touch->x>0 && touch->x<200 && touch->y>320 && touch->y<480)
		{
			// touch->x=0;
			// touch->y=0;
			// bzero(buf, sizeof(buf));
			// system("killall -KILL madplay");
			// // sprintf(buf, "/llf/chongjiao/madplay %s -a %d", argv[1], vol);
			// // //printf("/llf/chongjiao/madplay %s -a %d\n", argc[1], vol);
			// // system(buf);
			// printf("上一首\n");
		}
		//下一首
			//.........
		
		// //加声音
		// if(touch->x>600 && touch->x<800 && touch->y>0 && touch->y<160)
		// {
		// 	touch->x=0;
		// 	touch->y=0;
		// 	vol +=5;
		// }
		// //减声音
		// if(touch->x>600 && touch->x<800 && touch->y>320 && touch->y<480)
		// {
		// 	touch->x=0;
		// 	touch->y=0;
		// 	vol -=5;
		// }
	}	
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
	// if(argc < 2)
	// {
	// 	printf("参数不对！\n");
	// 	close(lcd);
	// 	munmap(p, 800*480*4);
	// 	exit(0);
	// }
	///////////////////////改路径////////////////////////////////////////////
	// int bmp =  open("./1.bmp", O_RDONLY);
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
	//颜色进缓冲区
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
	// for(int  y=0; y<120; y++)
	// 	{
	// 		for(int x=0; x<200; x++)
	// 		{
	// 			memcpy(p+4*x+800*4*y, &color_buff1[x+(119-y)*200], 4);
	// 		}
	// 	}
	// //右四块
	// 	for(int  y=360; y<480; y++)
	// 	{
	// 		for(int x=600; x<800; x++)
	// 		{
	// 			memcpy(p+4*x+800*4*y, &color_buff4[x+(479-y)*200], 4);
	// 		}
	// 	}
	// 	for(int y=240; y<360; y++)
	// 	{
	// 		for(int x=600; x<800; x++)
	// 		{
	// 			memcpy(p+4*x+800*4*y, &color_buff3[x+(350-y)*200], 4);
	// 		}
	// 	}
	// 	for(int y=120; y<240; y++)
	// 	{
	// 		for(int x=600; x<800; x++)
	// 		{
	// 			memcpy(p+4*x+800*4*y, &color_buff2[x+(239-y)*200], 4);
	// 		}
	// 	}
	// 	for(int y=0; y<120; y++)
	// 	{
	// 		for(int x=600; x<800; x++)
	// 		{
	// 			memcpy(p+4*x+800*4*y, &color_buff1[x+(119-y)*200], 4);
	// 		}
	// 	}
	// 5, 释放资源
	munmap(p, 800*480*4);
	// close(lcd);
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
			/* code */printf("此处相册返回\n");
			bzero(buf, sizeof(buf));
			//system("killall -KILL madplay");
			//system("killall -KILL music_main");
			sprintf(buf, "/hehe/music_main %s -a %d &", "/lyx/SeeYouAgain.mp3", -30);
			//printf("/llf/chongjiao/madplay %s -a %d\n", argc[1], vol);
			system(buf);
		}
	}
	//关闭触摸屏
	close(ts);
}