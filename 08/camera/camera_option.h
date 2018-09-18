#ifndef __TOUCH_H_
#define __TOUCH_H_

#include <linux/input.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>


#include "jpeglib.h"
#include <pthread.h>

struct get_abs_xy
{
	int x;
	int y;
	int ts;
};

char *init_lcd(struct fb_var_screeninfo *vinfo);
void get_xy_switch(int ts, int *x, int *y);
void get_xy(struct get_abs_xy *p);
void usage(int argc, char **argv);
int show_jpg(char *jpgpath);
void KillNoneLess(void);
void pic_switch(void);
void mainpage(void);
int init_touch(void);
void show_cam(void);
void shoot_ui(void);

#endif