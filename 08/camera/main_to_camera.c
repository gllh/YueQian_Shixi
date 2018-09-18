#include "camera_option.h"
#include <pthread.h>

int main(int argc, char **argv){
	KillNoneLess();
	char buf[80];
	int vol = -30;
	struct get_abs_xy *touch = calloc(1, sizeof(struct get_abs_xy));
	touch->ts = init_touch();
	
	while(1)
	{
		show_cam();
	}	
}