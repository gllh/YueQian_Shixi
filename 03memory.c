#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void)
{
	//打开LCD屏
	int lcd = open("/dev/fb0", O_RDWR);
	if(lcd == -1)
	{
		perror("打开LCD失败：\n");
		exit(0);
	}
	
	char *p = mmap(NULL, 800*480*4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd, 0);
	if(p == NULL)
	{
		perror("映射失败：");
		exit(0);
	}
	int i,j;
	int32_t color = 0x00ff0000;
	int32_t a=0x00000000;
	int32_t b=0x00ff0000;
	int32_t c=0x001f666f;
	int32_t d=0x00f1ff00;
	int32_t e=0x00ff0080;
	int32_t f=0x00107380;
	int32_t g=0x007010f0;
	int32_t h=0x00016680;
while(1){
		loop1(a,b,c,d,e,f,g,h);
		sleep(1);
		loop1(e,a,b,c,f,g,h,d);
		sleep(1);
		loop1(f,e,a,b,g,h,d,c);
		sleep(1);
		loop1(g,f,e,a,h,d,c,b);
		sleep(1);
		// for(i=0;i<240;i++){
		// 	for(j=0;j<200;j++){
		// 		int wg1=memcpy(p+4*j+800*4*i,&a,4); 
		// 	}
		// 	for(j=200;j<400;j++){
		// 		int wg2=memcpy(p+4*j+800*4*i,&b,4); 
		// 	}
		// 	for(j=400;j<600;j++){
		// 		int wg3=memcpy(p+4*j+800*4*i,&c,4); 
		// 	}
		// 	for(j=600;j<800;j++){
		// 		int wg4=memcpy(p+4*j+800*4*i,&d,4); 
		// 	}
		// }
		// for(i=240;i<480;i++){
		// 	for(j=0;j<200;j++){
		// 		int wg1=memcpy(p+4*j+800*4*i,&e,4); 
		// 	}
		// 	for(j=200;j<400;j++){
		// 		int wg2=memcpy(p+4*j+800*4*i,&f,4); 
		// 	}
		// 	for(j=400;j<600;j++){
		// 		int  wg3=memcpy(p+4*j+800*4*i,&g,4); 
		// 	}
		// 	for(j=600;j<800;j++){
		// 		int wg4=memcpy(p+4*j+800*4*i,&h,4); 
		// 	}
		// }
	}
	// for(int j=0; j<480; j++)
	// {
	// 	for(int i=0; i<800; i++)
	// 	{
	// 		memcpy(p+4*i+800*4*j , &color, 4);
	// 	}
	// }

	close(lcd);
	return 0;	
}
void loop1(int a,int b,int c,int d
		,int e,int f,int g,int h){
	int lcd = open("/dev/fb0", O_RDWR);
	char *p = mmap(NULL, 800*480*4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd, 0);
	int i,j;
	for(i=0;i<240;i++){
			for(j=0;j<200;j++){
				int wg1=memcpy(p+4*j+800*4*i,&a,4); 
			}
			for(j=200;j<400;j++){
				int wg2=memcpy(p+4*j+800*4*i,&b,4); 
			}
			for(j=400;j<600;j++){
				int wg3=memcpy(p+4*j+800*4*i,&c,4); 
			}
			for(j=600;j<800;j++){
				int wg4=memcpy(p+4*j+800*4*i,&d,4); 
			}
		}
		for(i=240;i<480;i++){
			for(j=0;j<200;j++){
				int wg1=memcpy(p+4*j+800*4*i,&e,4); 
			}
			for(j=200;j<400;j++){
				int wg2=memcpy(p+4*j+800*4*i,&f,4); 
			}
			for(j=400;j<600;j++){
				int  wg3=memcpy(p+4*j+800*4*i,&g,4); 
			}
			for(j=600;j<800;j++){
				int wg4=memcpy(p+4*j+800*4*i,&h,4); 
			}
		}
}