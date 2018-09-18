#include "music_option.h"

int main(int argc, char **argv){
	KillNoneLess();
	char buf[80];
	int vol = -30;
	music_mainpage();
	struct get_abs_xy *touch = calloc(1, sizeof(struct get_abs_xy));
	touch->ts = init_touch();
	int loop=0;
	while(1)
	{
		printf("dfffffffffffffffffff\n");
		//接收触摸坐标
		sleep(1);
		get_xy(touch);
		printf("X: %d Y:%d\n", touch->x, touch->y);
		//左1----返回back
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

			sprintf(buf, "/hehe/project_main");

			system(buf);
		}
		//播放button
		if(touch->x>600 && touch->x<800 && touch->y>240 && touch->y<360)
		{
			sprintf(buf, "/hehe/madplay %s -a %d &", "/lyx/SeeYouAgain.mp3", -30);
			system("killall -KILL project_main");
			system("killall -KILL main_to_camera");
			system(buf);
		}
		//右下1---next跳下一歌曲
		if(touch->x>600 && touch->x<800 && touch->y>360 && touch->y<480)
		{
			touch->x=0;
			touch->y=0;

			int ts = init_touch();
			struct fb_var_screeninfo my_vinfo1;

			char *p = init_lcd(&my_vinfo1);
			char *a[]={"/lyx/summer.mp3","/lyx/SeeYouAgain.mp3","/lyx/flower.mp3"};
			system("killall -KILL madplay");
			sleep(1);
			sprintf(buf, "/lyx/madplay %s -a %d &", a[loop], -30);
			loop++;
			if(loop==3){
				loop=0;
			}
			system(buf);
		}
		//上一首
		if(touch->x>400 && touch->x<600 && touch->y>360 && touch->y<480)
		{
			touch->x=0;
			touch->y=0;
			////
			int ts = init_touch();
			struct fb_var_screeninfo my_vinfo1;
			/////
			char *p = init_lcd(&my_vinfo1);
			char *a[]={"/lyx/summer.mp3","/lyx/SeeYouAgain.mp3","/lyx/flower.mp3"};
			system("killall -KILL madplay");
			sleep(1);

			sprintf(buf, "/lyx/madplay %s -a %d &", a[loop], -30);
			loop--;
			if(loop==-1){
				loop=2;
			}
			system(buf);
		}
		
	}	
}
