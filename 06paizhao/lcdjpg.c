#include <stdio.h>   	
#include <fcntl.h>		 	 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "lcdjpg.h"

#if EN_LCD_SHOW_JPG
#include "jpeglib.h"
#endif

//#include "api_v4l2.h"
unsigned long file_size_get(const char *pfile_path);
static char g_color_buf[FB_SIZE]={0};

static int  g_fb_fd;
static int *g_pfb_memory;

/* video_chat.c ���л���ʾ������ */
volatile int g_jpg_in_jpg_x;
volatile int g_jpg_in_jpg_y;

//LCD����
void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color)
{
	*(g_pfb_memory+y*800+x)=color;
}

#if EN_LCD_SHOW_JPG
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path,char *pjpg_buf,unsigned int jpg_buf_size,unsigned int jpg_half)  
{
	//��ʼ��LCD
	g_fb_fd = open("/dev/fb0", O_RDWR);
	
	if(g_fb_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}

	g_pfb_memory  = (int *)mmap(	NULL, 					//ӳ�����Ŀ�ʼ��ַ������ΪNULLʱ��ʾ��ϵͳ����ӳ��������ʼ��ַ
									FB_SIZE, 									//ӳ�����ĳ���
									PROT_READ|PROT_WRITE, 		//���ݿ��Ա���ȡ��д��
									MAP_SHARED,							//�����ڴ�
									g_fb_fd, 									//��Ч���ļ�������
									0												//��ӳ��������ݵ����
								);
	
	/*���������󣬴��������*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	
			 int	jpg_fd;
	unsigned int 	jpg_size;
	
	unsigned int 	jpg_width;
	unsigned int 	jpg_height;
	

	if(pjpg_path!=NULL)
	{
		/* ����jpg��Դ��Ȩ�޿ɶ���д */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* ��ȡjpg�ļ��Ĵ�С */
		jpg_size=file_size_get(pjpg_path);	

		/* Ϊjpg�ļ������ڴ�ռ� */	
		pjpg = malloc(jpg_size);

		/* ��ȡjpg�ļ��������ݵ��ڴ� */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
	{
		jpg_size = jpg_buf_size;
		
		pjpg = pjpg_buf;
	}

	/*ע�������*/
	cinfo.err = jpeg_std_error(&jerr);
	
	/*��������*/
	jpeg_create_decompress(&cinfo);

	/*ֱ�ӽ����ڴ�����*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*���ļ�ͷ*/
	jpeg_read_header(&cinfo, TRUE);

	/*��ʼ����*/
	jpeg_start_decompress(&cinfo);	
	
	
	if(jpg_half)
	{
		x_e	= x_s+(cinfo.output_width/2);
		y_e	= y  +(cinfo.output_height/2);		
		
		/*����������*/
		while(cinfo.output_scanline < cinfo.output_height)
		{		
			pcolor_buf = g_color_buf;
			
			/* ��ȡjpgһ�е�rgbֵ */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);			
			
			/* �ٶ�ȡjpgһ�е�rgbֵ */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);

			for(i=0; i<(cinfo.output_width/2); i++)
			{
				/* ��ȡrgbֵ */
				color = 		*(pcolor_buf+2);
				color = color | *(pcolor_buf+1)<<8;
				color = color | *(pcolor_buf)<<16;
				
				/* ��ʾ���ص� */
				lcd_draw_point(x,y,color);
				
				pcolor_buf +=6;
				
				x++;
			}
			
			/* ���� */
			y++;					
			
			x = x_s;	
		}
	}
	else
	{
		x_e	= x_s+cinfo.output_width;
		y_e	= y  +cinfo.output_height;	

		/*����������*/
		while(cinfo.output_scanline < cinfo.output_height )
		{		
			pcolor_buf = g_color_buf;
			
			/* ��ȡjpgһ�е�rgbֵ */
			jpeg_read_scanlines(&cinfo,(JSAMPARRAY)&pcolor_buf,1);
			
			for(i=0; i<cinfo.output_width; i++)
			{
				/* ��ȡrgbֵ */
				color = 		*(pcolor_buf+2);
				color = color | *(pcolor_buf+1)<<8;
				color = color | *(pcolor_buf)<<16;
				
				/* ��ʾ���ص� */
				lcd_draw_point(x,y,color);
				
				pcolor_buf +=3;
				
				x++;
			}
			
			/* ���� */
			y++;			
			
			x = x_s;
			
		}		
	}
	
	/*�������*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* �ر�jpg�ļ� */
		close(jpg_fd);	
		
		/* �ͷ�jpg�ļ��ڴ�ռ� */
		free(pjpg);		
	}
	//LCD�ر�
	/* ȡ���ڴ�ӳ�� */
	munmap(g_pfb_memory, FB_SIZE);
	
	/* �ر�LCD�豸 */
	close(g_fb_fd);
	return 0;
}
#endif

