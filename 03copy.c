#include <stdio.h>
#include <stdlib.h>

int mian(int argc,int *argv[])
{
	char input=argv[0];
	int fd=open(input,O_RDWR);
	int bufferl;
	rename(input,argv[1]);
	int re=read(fd,buffer,sizeof(buffer));
	printf("file%d\n",buffer );
	return 0;
}