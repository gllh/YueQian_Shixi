#include"api_v4l2.h"
#include"lcdjpg.h"
#include<string.h>

int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half);  

int main(void){
	linux_v4l2_open_device("/dev/video0");
	linux_v4l2_start_capturing();
	FrameBuffer myframe;
	while(1){
		linux_v4l2_get_fream(&myframe);
		lcd_draw_jpg(0,0,NULL,myframe.buf,myframe.length,0);
	}
	return 0;
}