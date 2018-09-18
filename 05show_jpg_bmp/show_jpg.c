#include "jpg.h"

char *readjpg(char const *filename, int size)
{
	char *jpg_buffer = calloc(1, size);

	// 不断读取JPG信息，放入jpg_buffer
	int fd = open(filename, O_RDONLY);

	int n = 0;
	while(size > 0)
	{
		n = read(fd, jpg_buffer+n, size);	

		size -= n;
	}

	close(fd);
	return jpg_buffer;
}

int show_jpg(char *jpgpath) // ./showjpg xxx.jpg
{
	//获取文件属性
	struct stat info;
	bzero(&info, sizeof(info));
	stat(jpgpath, &info);
	
	//读取JPEG文件
	char *jpg_buffer = readjpg(jpgpath, info.st_size);

	// 声明解压缩结构体，以及错误管理结构体
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// 使用缺省的出错处理来初始化解压缩结构体
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// 配置该cinfo，使其从jpg_buffer中读取jpg_size个字节
	// 这些数据必须是完整的JPEG数据
	jpeg_mem_src(&cinfo, jpg_buffer, info.st_size);


	// 读取JPEG文件的头，并判断其格式是否合法
	int ret = jpeg_read_header(&cinfo, true);
	if(ret != 1)
	{
		fprintf(stderr, "[%d]: jpeg_read_header failed: "
			"%s\n", __LINE__, strerror(errno));
		exit(1);
	}

	// 开始解压
	jpeg_start_decompress(&cinfo);

	// 将图片的宽、高、深度信息保存起来
	struct imginfo imageinfo;
	imageinfo.width      = cinfo.output_width;
	imageinfo.height     = cinfo.output_height;
	imageinfo.pixel_size = cinfo.output_components;

	int row_stride = imageinfo.width * imageinfo.pixel_size;

	// 根据图片的尺寸大小，分配一块相应的内存bmp_buffer
	// 用来存放从jpg_buffer解压出来的图像数据
	unsigned long rgb_size;
	unsigned char *rgb_buffer;
	rgb_size   = imageinfo.width * imageinfo.height * imageinfo.pixel_size;
	rgb_buffer = (unsigned char *)calloc(1, rgb_size);

	// 循环地将图片的每一行读出并解压到bmp_buffer中
	int line = 0;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = rgb_buffer +
				(cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

	// 解压完了，将jpeg相关的资源释放掉
 	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);

	// 将rgb_buffer中的数据，妥善地放到lcd上去显示
	int lcd = open("/dev/fb0", O_RDWR);


	// 获取LCD的屏幕参数信息
	struct fb_var_screeninfo vinfo;
	bzero(&vinfo, sizeof(vinfo));
	ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);

	int lcd_w = vinfo.xres;
	int lcd_h = vinfo.yres;
	int lcd_bpp = vinfo.bits_per_pixel;

	char *p = mmap(NULL, lcd_w * lcd_h * lcd_bpp/8, PROT_READ|PROT_WRITE,
			MAP_SHARED, lcd, 0);


	int w = imageinfo.width;
	int h = imageinfo.height;

	for(int j=0; j<480 && j<h; j++)
	{
		for(int i=0; i<800 && i<w; i++)
		{
			memcpy(p + 4*i + 800*4*j+vinfo.red.offset/8,  rgb_buffer + 3*i + w*3*j+0, 1);
			memcpy(p + 4*i + 800*4*j+vinfo.green.offset/8,rgb_buffer + 3*i + w*3*j+1, 1);
			memcpy(p + 4*i + 800*4*j+vinfo.blue.offset/8, rgb_buffer + 3*i + w*3*j+2, 1);
		}
	}

	munmap(p, 800*480*4);
	free(rgb_buffer);
	close(lcd);

	return 0;
}
