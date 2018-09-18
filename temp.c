#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
	int lcd=Open_Lcd("/dev/fb0",1);
	CanShu(argc,argv);	
	int bmp=Open_Bmp(argc,argv);	
	Mem_Deal(bmp,lcd);
pp
	return 0;	
}
//打开lcd判断
int Open_Lcd(const char *pathname, int flag){
	int lcd = open(pathname, O_RDWR);
	if(lcd == -1){
		perror("打开LCD失败");	
		exit(0);
	}
	return lcd;
}
//参数判断
void CanShu(int argc, char **argv){
	if(argc < 2){
		printf("参数不对！\n");
		exit(0);
	}
}
//打开图片判断
int Open_Bmp(int argc,char **argv){
	int bmp =  open(argv[1], O_RDONLY);
	if(bmp == -1){
		perror("打开图片失败！");
		exit(0);	
	}
	return bmp;
}
//内存处理封装
void Mem_Deal(int bmp,int lcd){
	char *p = mmap(NULL, // 不指定映射内存的位置，让系统自动选取
		       800*480*4, // 映射内存的大小
		       PROT_READ | PROT_WRITE, // 以读、写模式操作映射内存
		       MAP_SHARED, // 其他线程、进程可以共享访问这块内存
		       lcd, // LCD文件描述符
		       0    // 映射内存与LCD的偏移量，0代表完全重合
		       );
	char bmp_buf[800*480*3];
	
	int ret = read(bmp, bmp_buf, sizeof(bmp_buf));
	if(ret <= 0){
		perror("读取bmp数据失败！");
		close(lcd);
		close(bmp);
		munmap(p, 800*480*4);
		exit(0);
	}
	int lcd_buf[800*480*4];
	// 4, 铺满整个映射内存
	for(int i=0; i<800*480; i++){
		lcd_buf[i]=bmp_buf[i*3] | bmp_buf[i*3+1]<<8 | bmp_buf[i*3+2]<<16;
	}

	for(int y=0; y<480; y++){
		for(int x=0; x<800; x++){
			memcpy(p+4*x+800*4*y, &lcd_buf[x+(479-y)*800], 4);
		}
	}
	munmap(p, 800*480*4);
	close(lcd);
}