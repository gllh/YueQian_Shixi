#ifndef __JPG_H_
#define __JPG_H_

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <linux/input.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdbool.h>

#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "jpeglib.h"

struct imginfo
{
	int width;
	int height;
	int pixel_size;
};

char *readjpg(char const *filename, int size);
int show_jpg(char *jpgpath);


#endif