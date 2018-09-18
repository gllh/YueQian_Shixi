#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
int main(void){
	int fd =open("./hi.txt",O_RDWR);
	char buffer[80];
	int input;
	printf("选择操作1：读2：写\n");	
	scanf("%d",&input);
	switch(input)
	{
		case 1:
		int re=read(fd,buffer,sizeof(buffer));
		printf("文件的内容为：%s",buffer);
		close(fd);
		break;
		case 2:
		printf("输入写入内容：\n");
		scanf("%s",&buffer);
		int wr=write(fd,buffer,sizeof(buffer));
		close(fd);
		break;
	}
	return 0;
}

