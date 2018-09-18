#include "lcdjpg.h"
#include "api_v4l2.h"
#include <string.h>

int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half);

int main(void)
{
	//打开摄像头
	linux_v4l2_device_init("/dev/video0");
	//启动摄像捕捉
	linux_v4l2_start_capturing();
	//开始捕捉
	FrameBuffer mybuffer;
	while(1)
	{	
		linux_v4l2_get_fream(&mybuffer);
		//显示
		lcd_draw_jpg( 0, 0, NULL, mybuffer.buf, mybuffer.length,0);
		
		//拍照....
	}
	return 0;
}
