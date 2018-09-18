#include"serical_option.h"


#define DEVICE_NAME "/dev/ttySAC1"
int ser_fd;

int main(void)
{
	printf("进入zigbee======================\n");
	KillNoneLess();
	char buf1[80];
	struct get_abs_xy *touch = calloc(1, sizeof(struct get_abs_xy));
	touch->ts = init_touch();
		
	Back_Ground();

	ser_fd = open(DEVICE_NAME,O_RDWR);
	if(ser_fd == -1)
	{
		printf("serical open failed!\n");
		return -1;
	}
	set_serial_uart1(ser_fd);
	
	if(fcntl(ser_fd,F_SETFL,0)<0)	
	{
		exit(1);
	}
	tcdrain(ser_fd); 
	tcflush( ser_fd, TCIOFLUSH);
	
	unsigned char buf[4]={0};
	unsigned char count=0;
	pthread_t thread;
	pthread_create(&thread,NULL,fun,NULL);
	printf("ssssssssssssssssssssssssssssssssssssssssssssssss\n" );
	while(1)
	{
		get_xy(touch);
		printf("X: %d Y:%d\n", touch->x, touch->y);
		if(touch->x>600 && touch->x<800 && touch->y>120 && touch->y<240)
		{
			KillNoneLess();
			sprintf(buf1, "/hehe/project_main");
			system(buf1);
			printf("测完了\n");
			exit(1);
		}
		write(ser_fd,"11",3);
		sleep(1);
		write(ser_fd,"10",3);
		sleep(1);
		write(ser_fd,"21",3);
		sleep(1);
		write(ser_fd,"20",3);
		sleep(1);
	}
	close(ser_fd);
}

