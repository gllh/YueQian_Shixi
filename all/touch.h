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

struct get_abs_xy
{
	int x;
	int y;
	int ts;
};

int init_touch(void);
void get_xy(struct get_abs_xy *p);


#endif