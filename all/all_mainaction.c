#include "touch.h"

int main()
{
	char buf[80];
	int vol = -30;
	struct get_abs_xy *touch = calloc(1, sizeof(struct get_abs_xy));
	touch->ts = init_touch();
	
	while(1)
	{
		get_xy(touch);
		printf("X: %d Y:%d\n", touch->x, touch->y);
		if(touch->x>0 && touch->x<200 && touch->y>0 && touch->y<100)
		{
			touch->x=0;
			touch->y=0;
			system("killall -KILL madplay");
		}
		if(touch->x>200 && touch->x<400 && touch->y>0 && touch->y<100)
		{
			touch->x=0;
			touch->y=0;
			bzero(buf, sizeof(buf));
			sprintf(buf, "/llf/chongjiao/madplay %s -a %d &", argv[1], vol);
			//printf("/llf/chongjiao/madplay %s -a %d\n", argc[1], vol);
			system(buf);
		}
		if(touch->x>0 && touch->x<200 && touch->y>320 && touch->y<480)
		{
			touch->x=0;
			touch->y=0;
			bzero(buf, sizeof(buf));
			system("killall -KILL madplay");
			// sprintf(buf, "/llf/chongjiao/madplay %s -a %d", argv[1], vol);
			// //printf("/llf/chongjiao/madplay %s -a %d\n", argc[1], vol);
			// system(buf);
			printf("上一首\n");
		}
		//下一首
			//.........
		
		//加声音
		if(touch->x>600 && touch->x<800 && touch->y>0 && touch->y<160)
		{
			touch->x=0;
			touch->y=0;
			vol +=5;
		}
		//减声音
		if(touch->x>600 && touch->x<800 && touch->y>320 && touch->y<480)
		{
			touch->x=0;
			touch->y=0;
			vol -=5;
		}
	}
	
}