#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <linux/input.h>

int main(void)
{
	//打开触摸屏
	int ts = open("/dev/input/event0", O_RDONLY);
	if(ts == -1){
		perror("打开触摸屏失败：");
		exit(0);
	}
	
	struct input_event myevent;
	bzero(&myevent, sizeof(myevent));

	while(1){
		
		if(myevent.type==EV_ABS){
			if(myevent.code==ABS_X){
				printf("X=%d",myevent.value);
			}
			usleep(20);
			if(myevent.close==ABS_Y){
				printf("Y=%d",myevent.value);
			}
			usleep(20);
		}
	}
	close(ts);
	return 0;	
}
