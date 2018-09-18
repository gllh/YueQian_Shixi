#ifndef __TOUCH_H_
#define __TOUCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <linux/input.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#include "jpeglib.h"

#include "bmp.h"
#include "jpg.h"

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
void Strat_Engine_Hehe(void);
int show_jpg(char *jpgpath);
void KillNoneLess(void);
void pic_switch(void);
int init_touch(void);
void mainpage(void);

#endif