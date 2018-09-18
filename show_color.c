#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
int main(void){
	int fd=open("/dev/fb0",O_RDWR);
	if(fd==-1){
		printf("无法打开");
	}
	int a=0x00000000;
	int b=0x00ff0000;
	int c=0x00ffffff;
	int d=0x00ffff00;
	int e=0x00ff0080;
	int f=0x00007380;
	int g=0x007000f0;
	int h=0x00006680;
	int i,j;
//germany glag
	// for(i=0;i<800*160;i++){
	// 	int wg=write(fd,&black,4);
	// }
	// for(i=800*160;i<800*320;i++){
	// 	int wr=write(fd,&red,4);
	// }
	// for(i=800*320;i<800*480;i++){
	// 	int wg=write(fd,&yellow,4);
	// }
// 块
	// while(1){
	// 	for(i=0;i<240;i++){
	// 		for(j=0;j<200;j++){
	// 			int wg1=write(fd,&red,4); 
	// 		}
	// 		for(j=200;j<400;j++){
	// 			int wg2=write(fd,&black,4); 
	// 		}
	// 		for(j=400;j<600;j++){
	// 			int wg3=write(fd,&yellow,4); 
	// 		}
	// 		for(j=600;j<800;j++){
	// 			int wg4=write(fd,&deepblue,4); 
	// 		}
	// 	}
	// 	for(i=240;i<480;i++){
	// 		for(j=0;j<200;j++){
	// 			int wg1=write(fd,&black,4); 
	// 		}
	// 		for(j=200;j<400;j++){
	// 			int wg2=write(fd,&yellow,4); 
	// 		}
	// 		for(j=400;j<600;j++){
	// 			int  wg3=write(fd,&deepblue,4); 
	// 		}
	// 		for(j=600;j<800;j++){
	// 			int wg4=write(fd,&red,4); 
	// 		}
	// 	}
	// }
	//loop
	while(1){
		loop1(a,b,c,d,e,f,g,h);
		sleep(2);
		loop1(e,a,b,c,f,g,h,d);
		sleep(2);
		loop1(f,e,a,b,g,h,d,c);
		sleep(2);
		loop1(g,f,e,a,h,d,c,b);
		sleep(2);
	}	
	return 0;
}
void loop1(int a,int b,int c,int d
		,int e,int f,int g,int h){
	int fd=open("/dev/fb0",O_RDWR);
	int i,j;
	for(i=0;i<240;i++){
			for(j=0;j<200;j++){
				int wg1=write(fd,&a,4); 
			}
			for(j=200;j<400;j++){
				int wg2=write(fd,&b,4); 
			}
			for(j=400;j<600;j++){
				int wg3=write(fd,&c,4); 
			}
			for(j=600;j<800;j++){
				int wg4=write(fd,&d,4); 
			}
			// usleep(2);
			//  int l=lseek(fd,800*3,SEEK_CUR);
		}
		for(i=240;i<480;i++){
			for(j=0;j<200;j++){
				int wg1=write(fd,&e,4); 
			}
			for(j=200;j<400;j++){
				int wg2=write(fd,&f,4); 
			}
			for(j=400;j<600;j++){
				int wg3=write(fd,&g,4); 
			}
			for(j=600;j<800;j++){
				int wg4=write(fd,&h,4); 
			}
			// usleep(2);
			//  int l=lseek(fd,800*3,SEEK_CUR);
		}
}
// void loop2(int a,int b,int c,int d){
// 	int fd=open("/dev/fb0",O_RDWR);
// 	int i,j;
// 	for(i=240;i<480;i++){
// 			for(j=0;j<200;j++){
// 				int wg1=write(fd,&a,4); 
// 			}
// 			for(j=200;j<400;j++){
// 				int wg2=write(fd,&b,4); 
// 			}
// 			for(j=400;j<600;j++){
// 				int wg3=write(fd,&c,4); 
// 			}
// 			for(j=600;j<800;j++){
// 				int wg4=write(fd,&d,4); 
// 			}
// 			// usleep(2);
// 			//  int l=lseek(fd,800*3,SEEK_CUR);
// 		}
// }
