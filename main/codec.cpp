#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <linux/fb.h>
#include <getopt.h>
#include <termios.h>
#include <png.h>

#include "codec.h"
#include "hulib.h"
#include <assert.h>
#ifndef uint32_t
#define uint32_t unsigned long
#endif
int image_write_to_snap(image * img, const char * path);
int image_read_from_snap(image * img, const char * path);

void memcpy_rotate180(void *dst,const void *src,size_t count)
{
    assert(dst!=NULL && src!=NULL);

    int num = count/4-1;
    uint32_t* tmp =(uint32_t* ) dst;
     uint32_t* s = (uint32_t* )src+num;
    for(size_t i=0;i<num;i++){
    	*(tmp++)=*(s--);
    }

}
void memcpy_rotate90(void *dst,void *src,unsigned int width,unsigned int height)
{
	 assert(dst!=NULL && src!=NULL);
    uint32_t* pd =(uint32_t* ) dst;
     uint32_t* ps = (uint32_t* )src+width*height-1;
     int dst_offset=0;
     int src_offset=0;
    for(size_t i=0;i<width;i++){
    	for(int j=0;j<height;j++){
    		*(pd+dst_offset+j)=*(ps+src_offset+i);
    		src_offset-=width;
    	}
    	src_offset=0;
    	dst_offset+=height;
    }
}





int access_Image(const char * filename)
{
	if (access(filename, F_OK) == 0 || access(hustr("cache/%s.snap", filename), F_OK) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int codec_to_Image(image * enode, const char * filename)
{
	if (filename == NULL)
	{
		errexitf("codec_to_Image path is unvalid [%s] [%s]\r\n", filename, enode->path.nstr());
	}
	enode->lock();

	if (image_read_from_snap(enode, filename) == 0)
	{
		log_i("codec_to_Image %s load snap\r\n", filename);
	}
	else
	{
		log_i("codec_to_Image %s load file\r\n", filename);

		const char * type = strrchr(filename, '.') + 1;
		if (strcasecmp(type, "png") == 0)
		{
			//debug_timer();
			//printf("$$$HU$$$ JPG\r\n");

			if (pngCodec_to_image(enode, filename))
			{
				errexitf("pngCodec_to_image %s", filename);
			}
			//debug_timer(filename);
		}
		//	else if (strcasecmp(type, "bmp") == 0)
		//	{
		//		//printf("$$$HU$$$ BMP\r\n");
		//		if (bmpCodec_to_image(enode, filename))
		//		{
		//			errexitf("bmpCodec_to_image %s", filename);
		//		}
		//	}
		//	else if(strcasecmp(strrchr(filename,'.')+1,"jpg")==0)
		//	{
		//		//printf("$$$HU$$$ JPG\r\n");
		//		if (jpegCodec_to_image(enode,filename))
		//		{
		//			exit(1);
		//		}
		//	}
		else
		{
			errexitf("unknow file [%s] type %s\r\n", filename, type);
		}
	}

	enode->unlock();

	return 0;
}

/******************************ͼƬ���*********************************/
//typedef struct _pic_data pic_data;
//struct _pic_data
//{
// int width, height; /* �ߴ� */
// int bit_depth;  /* λ�� */
// int flag;   /* һ����־����ʾ�Ƿ���alphaͨ�� */
//
// unsigned char **rgba; /* ͼƬ���� */
//};
/**********************************************************************/
#define PNG_BYTES_TO_CHECK 4
#define HAVE_ALPHA 1
#define NO_ALPHA 0

int pngCodec_to_image(image * sobj, const char *filepath)
{
	FILE *pic_fp;
	pic_fp = fopen(filepath, "rb");
	if (pic_fp == NULL)
	{
		errexitf("open %s %s \r\n", filepath, strerror(errno));
	}

	png_structp png_ptr;
	png_infop info_ptr;
	char buf[PNG_BYTES_TO_CHECK];
	int temp;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	info_ptr = png_create_info_struct(png_ptr);

	setjmp(png_jmpbuf(png_ptr));

	temp = fread(buf, 1, PNG_BYTES_TO_CHECK, pic_fp);
	temp = png_sig_cmp((png_byte*) buf, (png_size_t) 0, PNG_BYTES_TO_CHECK);

	if (temp != 0)
		return 1;

	rewind(pic_fp);
	png_init_io(png_ptr, pic_fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

	int color_type, channels, bit_depth;

	/*��ȡ��ȣ��߶ȣ�λ���ɫ����*/
	channels = png_get_channels(png_ptr, info_ptr); /*��ȡͨ����*/
	bit_depth = png_get_bit_depth(png_ptr, info_ptr); /* ��ȡλ�� */
	color_type = png_get_color_type(png_ptr, info_ptr); /*��ɫ����*/

	//printf("channels=%d bit_depth=%d color_type=%d\r\n",channels,bit_depth,color_type);
	int i, j;
	int pos = 0;
	/* row_pointers��߾���rgba��� */
	png_bytep* row_pointers;
	row_pointers = png_get_rows(png_ptr, info_ptr);
	int w = png_get_image_width(png_ptr, info_ptr);
	int h = png_get_image_height(png_ptr, info_ptr);
	sobj->SetBuffer(w, h);

	if (channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		struct PNG_ARGB
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		}__attribute__ ((packed));
		PNG_ARGB ** pix = (PNG_ARGB **) row_pointers;
		Color32 * dst_pix;
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{

				dst_pix = &sobj->pixels(x, y);
				dst_pix->r = pix[y][x].r;
				dst_pix->g = pix[y][x].g;
				dst_pix->b = pix[y][x].b;
				dst_pix->a = pix[y][x].a;

			}
		}
	}
	else if (channels == 3 || color_type == PNG_COLOR_TYPE_RGB)
	{
		struct PNG_RGB
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
		}__attribute__ ((packed));
		PNG_RGB ** pix = (PNG_RGB **) row_pointers;
		Color32 * dst_pix ;
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				 dst_pix = &sobj->pixels(x, y);
				dst_pix->r = pix[y][x].r;
				dst_pix->g = pix[y][x].g;
				dst_pix->b = pix[y][x].b;
				dst_pix->a = 255;

			}
		}

	}
	else
		return 1;

	fclose(pic_fp);
	/* �������ռ�õ��ڴ� */
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	return 0;
}

/* 将puc_data结构中的数据写入至png文件 */
int pngEndec_to_image(const char *file_name, image * graph)
{
	printf("write_png_file save to %s\r\n", file_name);
	int j, i, temp, pos;
	png_byte color_type;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep * row_pointers;
	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
	{
		errexitf("[write_png_file] File %s could not be opened for writing", file_name);
	}

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		huErrExit("[write_png_file] png_create_write_struct failed");
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		huErrExit("[write_png_file] png_create_info_struct failed");
	}
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		huErrExit("[write_png_file] Error during init_io");
	}
	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		huErrExit("[write_png_file] Error during writing header");
	}
	/* 判断要写入至文件的图片数据是否有透明度，来选择色彩类型 */
	color_type = PNG_COLOR_TYPE_RGB_ALPHA;

	png_set_IHDR(png_ptr, info_ptr, graph->GetWidth(), graph->GetHeight(), 8, color_type, PNG_INTERLACE_NONE,
	PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		huErrExit("[write_png_file] Error during writing bytes");
	}
	temp = (4 * graph->GetWidth());

	pos = 0;
	row_pointers = (png_bytep*) malloc(graph->GetHeight() * sizeof(png_bytep));
	Color32 * tmp;
	for (i = 0; i < graph->GetHeight(); i++)
	{
		row_pointers[i] = (png_bytep) malloc(sizeof(unsigned char) * temp);
		for (j = 0; j < temp; j += 4)
		{
			tmp = graph->GetPix(pos);
			row_pointers[i][j] = tmp->r; // red
			row_pointers[i][j + 1] = tmp->g; // green
			row_pointers[i][j + 2] = tmp->b; // blue
			row_pointers[i][j + 3] = tmp->a; // alpha

//			row_pointers[i][j] = ((IMG_PIX *) graph->pSrcBuffer + pos)->u8Red; // red
//			row_pointers[i][j + 1] = ((IMG_PIX *) graph->pSrcBuffer + pos)->u8Green; // green
//			row_pointers[i][j + 2] = ((IMG_PIX *) graph->pSrcBuffer + pos)->u8Blue; // blue
//			row_pointers[i][j + 3] = ((IMG_PIX *) graph->pSrcBuffer + pos)->u8Alpha; // alpha
			++pos;
		}
	}
	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		huErrExit("[write_png_file] Error during end of write");
	}
	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	for (j = 0; j < graph->GetHeight(); j++)
		free(row_pointers[j]);
	free(row_pointers);

	fclose(fp);
	return 0;
}

int image_write_to_snap(image * img, const char * rawpath)
{
	hustr path("cache/%s.snap", rawpath);
	restart: FILE * fp = fopen(path, "w+");
	if (fp == NULL)
	{
		if (errno == ENOENT)
		{
			char str[strlen(path) + 1];
			strcpy(str, path);
			char*p = strrchr(str, '/');
			*p = 0;
			system(hustr("mkdir -p %s", str));
			goto restart;
		}
		else
		{
			errexitf("image_write_to_snap open %s:%d %s\r\n", path.c_str(), errno, strerror(errno));
		}

	}
	img->LoadResource();
	fseek(fp, SEEK_SET, 0);
	fwrite(&img->SrcSize, sizeof(unsigned long), 1, fp);
	fwrite(&img->u32Width, sizeof(unsigned int), 1, fp);
	fwrite(&img->u32Height, sizeof(unsigned int), 1, fp);
	fwrite(&img->u32Stride, sizeof(unsigned int), 1, fp);

	unsigned int reserved[17];
	fwrite(&reserved, sizeof(unsigned int), 17, fp);
	fwrite(img->pSrcBuffer, 1, img->SrcSize, fp);

	fclose(fp);
	return 0;
}

int image_read_from_snap(image * img, const char * rawpath)
{
	hustr path("cache/%s.snap", rawpath);
	if (access(path, F_OK) != 0)
	{
		return -1;
	}
	FILE * fp = fopen(path, "r");
	if (fp == NULL)
	{
		errexitf("image_read_from_snap open %s:%d %s\r\n", path.c_str(), errno, strerror(errno));
	}
	fseek(fp, SEEK_SET, 0);
	fread(&img->SrcSize, sizeof(unsigned long), 1, fp);
	fread(&img->u32Width, sizeof(unsigned int), 1, fp);
	fread(&img->u32Height, sizeof(unsigned int), 1, fp);
	fread(&img->u32Stride, sizeof(unsigned int), 1, fp);
	unsigned int reserved[17];
	fread(&reserved, sizeof(unsigned int), 17, fp);
	img->SetBuffer(img->GetWidth(), img->GetHeight());
	fread(img->pSrcBuffer, 1, img->SrcSize, fp);

	fclose(fp);
	return 0;
}
//int jpegCodec_to_image( image * sobj,const char *filename)
//{
//	//TEST_DECODE_DOWNSCALE
//	unsigned long BufferSize, bufferCount,readSize;
//	jpeg_param_t jpeg_param;
//	jpeg_info_t *jpeginfo;
//	unsigned char *pJpegBuffer = NULL, *pSRCbuffer = NULL, *pJpegSWaddr = NULL;
//	int jpg_codec_fd;
//	__u32 jpeg_buffer_size;
//	int enc_reserved_size;
//	int ret = 0;
//	int i,len, jpeginfo_size;
//	int width,height, parser;
//	FILE *fp;
//
//	for(i=0;i<100;i++)
//	{
//		jpg_codec_fd = open("/dev/video1", O_RDWR);
//		if (jpg_codec_fd < 0)
//		{
//			continue;
//		}
//		break;
//	}
//	if (jpg_codec_fd < 0)
//	{
//		printf("JPEG Engine is busy\n");
//		exit(-1);
//	}
//	/* allocate memory for JPEG engine */
//	ioctl(jpg_codec_fd, JPEG_GET_JPEG_BUFFER, (__u32)&jpeg_buffer_size);
//
//	//printf("\tjpeg engine memory buffer: 0x%X\n",jpeg_buffer_size);
//
//	pJpegBuffer = (unsigned char *)mmap(NULL, jpeg_buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, jpg_codec_fd, 0);
//
//	if(pJpegBuffer == MAP_FAILED)
//	{
//		printf("JPEG Map Failed!\n");
//		exit(-1);
//	}
//	else
//	{
//	//	printf("\tGet memory from jpeg engine: 0x%X\n",jpeg_buffer_size);
//	}
//
//	memset((void*)&jpeg_param, 0, sizeof(jpeg_param_t));
//	jpeginfo_size = sizeof(jpeg_info_t) + sizeof(__u32);
//	jpeginfo = (jpeg_info_t *)malloc(jpeginfo_size);
//
//	int DecOPWbuffersize;
//
//	jpeg_param.encode = 0;			/* Decode Operation */
//	jpeg_param.src_bufsize = jpeg_buffer_size/2;	/* Src buffer size (Bitstream buffer size for JPEG engine) */
//	jpeg_param.dst_bufsize = jpeg_buffer_size/2; /* Dst buffer size (Decoded Raw data buffer size for JPEG engine) */
//	jpeg_param.decInWait_buffer_size = 0;	/* Decode input Wait buffer size (Decode input wait function disable when								   decInWait_buffer_size is 0) */
//	jpeg_param.decopw_en = 0;
//	jpeg_param.windec_en = 0;
//
//
//	//jpeg_param.dec_stride = xres; /* Enable stride function */
//
//
//	/* Total buffer size for JPEG engine */
//
//	BufferSize = (jpeg_param.src_bufsize + jpeg_param.dst_bufsize);
//
//	if(BufferSize > jpeg_buffer_size)
//	{
//		printf("JPEG Engine Buffer isn't enough\n");
//		exit(-1);
//	}
//
//	/* Clear buffer */
//
//	//memset(pJpegBuffer, 0x77, BufferSize);
//
//	/* Open JPEG file */
//	fp = fopen(filename, "r");
//	if(fp == NULL)
//	{
//	    printf("open %s error : %s\n", filename,strerror(errno));
//	    exit(-1);
//	}
//
//	pSRCbuffer = pJpegBuffer;
//	bufferCount = 0;
//	parser = 0;
//	//printf("JPEG Header Parser:\n");
//	/* Read Bitstream to JPEG engine src buffer */
//	while(!feof(fp))
//	{
//		fd_set writefds;
//		struct timeval tv;
//		int result;
//		tv.tv_sec		= 0;
//		tv.tv_usec		= 0;
//		FD_ZERO( &writefds );
//		FD_SET( jpg_codec_fd , &writefds );
//		tv.tv_sec		= 0;
//		tv.tv_usec		= 0;
//
//		select( jpg_codec_fd + 1 , NULL , &writefds , NULL, &tv );
//		if( FD_ISSET( jpg_codec_fd, &writefds ))
//		{
//			readSize = fread(pSRCbuffer, 1, 4096 , fp);
//			pSRCbuffer += readSize;
//			bufferCount += readSize;
//		}
//		if(!parser)
//		{
//			result = ParsingJPEG(pJpegBuffer, bufferCount, &width, &height);
//			if(!result)
//			{
//				//printf("\t->Width %d, Height %d\n", width,height);
//				parser = 1;
//			}
//			else{}
//				//printf("\t->Can't get image siz in %5d byte bistream\n", bufferCount);
//		}
//
//		if( bufferCount > jpeg_param.src_bufsize)
//		{
//			printf("Bitstream size is larger than src buffer, %d!!\n",bufferCount);
//			return 0;
//		}
//	}
//	printf("Bitstream is %d Bytes\n",bufferCount);
//
//	if(bufferCount % 4)
//		bufferCount = (bufferCount & ~0x3) + 4;
//
//
//	printf("Set Src Buffer is %d Bytes\n",bufferCount);
//
//	jpeg_param.src_bufsize = bufferCount;	/* Src buffer size (Bitstream buffer size for JPEG engine) */
//	jpeg_param.dst_bufsize = BufferSize - bufferCount;	/* Dst buffer size (Decoded Raw data buffer size for JPEG engine) */
//
//
//
//	jpeg_param.buffersize = 0;		/* only for continuous shot */
//		jpeg_param.buffercount = 1;
//
//	/* Set decode output format: RGB555/RGB565/RGB888/YUV422/PLANAR_YUV */
//	jpeg_param.decode_output_format = DRVJPEG_DEC_PRIMARY_PACKET_RGB565;
//
//
//
//	/* Set operation property to JPEG engine */
//	if((ioctl(jpg_codec_fd, JPEG_S_PARAM, &jpeg_param)) < 0)
//	{
//		fprintf(stderr,"set jpeg param failed:%d\n",errno);
//		ret = -1;
//		goto out;
//	}
//
//
//	/* Trigger JPEG engine */
//	if((ioctl(jpg_codec_fd, JPEG_TRIGGER, NULL)) < 0)
//	{
//		fprintf(stderr,"trigger jpeg failed:%d\n",errno);
//		ret = -1;
//		goto out;
//	}
//
//	/* Get JPEG decode information */
//	len = read(jpg_codec_fd, jpeginfo, jpeginfo_size);
//
//	if(len<0) {
//		fprintf(stderr, "read data error errno=%d\n", errno);
//		ret = -1;
//		goto out;
//	}
//	//printf("JPEG: Width %d, Height %d\n",jpeginfo->width, jpeginfo->height);
//
//	if(jpeginfo->state == JPEG_DECODED_IMAGE)
//	{
//
//		sobj->SrcSize=jpeginfo->image_size[0];
//
//		//sobj->srcBG_fd = -1;
//		sobj->pSrcBuffer = NULL;
//		sobj->srcFormat = eDRVBLT_SRC_RGB565;
//		int err = posix_memalign(&sobj->pSrcBuffer, 4, sobj->SrcSize);
//		if (err)
//		{
//				errexitf("posix_memalign failed: %s\n", strerror(err));
//		}
//
//
//		memcpy(sobj->pSrcBuffer,(char*)(pJpegBuffer + jpeg_param.src_bufsize), sobj->SrcSize);
//		sobj->u32Stride=jpeginfo->width<<1;
//		sobj->get_width()=jpeginfo->width;
//		sobj->get_height()=jpeginfo->height;
//
//		printf("jpeg codec filename=%s %d %d\r\n",filename,sobj->get_width(),sobj->get_height());
//
//	}
//	else if(jpeginfo->state == JPEG_DECODE_ERROR)
//		printf("Decode Error\n");
//	else if(jpeginfo->state == JPEG_MEM_SHORTAGE)
//		printf("Memory Shortage\n");
//	else if(jpeginfo->state == JPEG_DECODE_PARAM_ERROR)
//		printf("Decode Parameter Error\n");
//
//
//out:
//	if(pJpegBuffer)
//		munmap(pJpegBuffer, jpeg_buffer_size);
//
//	close(jpg_codec_fd);
//	fclose(fp);
//	free(jpeginfo);
//	return 0;
//
//}

int  ProcArea(image * dst_img, image * rsc_img, int & src_x, int & src_y, int & cp_width, int & cp_height, int & dst_x, int & dst_y)
{
	if (cp_width <= 0 || cp_height <= 0)
	{
		return 1;
	}

	if (src_x < 0)
	{
		dst_x -= src_x;
		src_x = 0;
	}
	if (src_y < 0)
	{
		dst_y -= src_y;
		src_y = 0;
	}
	if (dst_x < 0)
	{
		src_x -= dst_x;
		dst_x = 0;
	}
	if (dst_y < 0)
	{
		src_y -= dst_y;
		dst_y = 0;
	}


	if (src_y + cp_height > rsc_img->GetHeight())
	{
		//printf("AreaCopy src_y=%d cp_height=%d rsc_img->get_height()=%d\r\n", src_y, cp_height, rsc_img->get_height());
		cp_height = rsc_img->GetHeight() - src_y;
		if (cp_height <= 0)
		{
			return 1;
		}
	}
	if (src_x + cp_width > rsc_img->GetWidth())
	{
		//printf("AreaCopy src_x=%d cp_width=%d rsc_img->get_width()=%d\r\n", src_x, cp_width, rsc_img->get_width());
		cp_width = rsc_img->GetWidth() - src_x;
		if (cp_width <= 0)
		{
			return 1;
		}
	}
	if (dst_y + cp_height > dst_img->GetHeight())
	{
		cp_height = dst_img->GetHeight() - dst_y;
		if (cp_width <= 0)
		{
			return 1;
		}
	}
	if (dst_x + cp_width > dst_img->GetWidth())
	{
		cp_width = dst_img->GetWidth() - dst_x;
		if (cp_width <= 0)
		{
			return 1;
		}
	}
	return 0;
}
/************************************
 *
 */

//void AreaCopy(image * dst_img, image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
//{
//	int y;
//	if (dst_img->SrcGPUAddr() == 0 || src_img->SrcGPUAddr() == 0)
//		{
//			debug("warning::Image source point is NULL dst=%#x src=%#x\r\n", dst_img->SrcGPUAddr(), src_img->SrcGPUAddr());
//			return;
//		}
//	dst_img->lock();
//	src_img->lock();
//	int line_byte=cp_width<<2;
//	ProcArea(dst_img, src_img, src_x, src_y, cp_width, cp_height, dst_x, dst_y);
//	for (y = 0; y < cp_height; y++)
//	{
//		memcpy((unsigned int *) dst_img->pSrcBuffer + (y + dst_y) * dst_img->u32Width + dst_x,
//				(unsigned int *) src_img->pSrcBuffer + (y + src_y) * src_img->u32Width + src_x, line_byte);
//	}
//	dst_img->unlock();
//	src_img->unlock();
//}
void AreaCopy(image * dst_img, image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
{
	//int x;
	int y;

	if (dst_img->SrcGPUAddr() == 0 || src_img->SrcGPUAddr() == 0)
		{
		//	log_w("warning::Image source point is NULL dst=%#x src=%#x\r\n", dst_img->SrcGPUAddr(), src_img->SrcGPUAddr());
			return;
		}

	if(ProcArea(dst_img, src_img, src_x, src_y, cp_width, cp_height, dst_x, dst_y)){
		return ;
	}
	dst_img->lock();
	src_img->lock();
	int line_byte=cp_width<<2;
//	unsigned int dst_step= dst_img->u32Width;
//	unsigned int src_step= src_img->u32Width;
	unsigned int * dst_start=(unsigned int *)dst_img->pSrcBuffer +  dst_y * dst_img->u32Width + dst_x;
	unsigned int * src_start=(unsigned int *)src_img->pSrcBuffer +  src_y * src_img->u32Width + src_x;
	unsigned int dst_offset=0;
	unsigned int src_offset=0;
	for (y = 0; y < cp_height; y++)
	{
		memcpy( dst_start+dst_offset,
				 src_start+src_offset, line_byte);
		dst_offset+=dst_img->u32Width;
		src_offset+=src_img->u32Width;
	}

	dst_img->unlock();
	src_img->unlock();
}


void AreaCopy_no_ProcArea(image * dst_img, image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
{
	//int x;
	int y;

	if (dst_img->SrcGPUAddr() == 0 || src_img->SrcGPUAddr() == 0)
		{
		//	log_w("warning::Image source point is NULL dst=%#x src=%#x\r\n", dst_img->SrcGPUAddr(), src_img->SrcGPUAddr());
			return;
		}

//	if(ProcArea(dst_img, src_img, src_x, src_y, cp_width, cp_height, dst_x, dst_y)){
//		return ;
//	}
	dst_img->lock();
	src_img->lock();
	int line_byte=cp_width<<2;

//	unsigned int dst_step= dst_img->u32Width;
//	unsigned int src_step= src_img->u32Width;
	unsigned int * dst_start=(unsigned int *)dst_img->pSrcBuffer +  dst_y * dst_img->u32Width + dst_x;
	unsigned int * src_start=(unsigned int *)src_img->pSrcBuffer +  src_y * src_img->u32Width + src_x;
	unsigned int dst_offset=0;
	unsigned int src_offset=0;
	for (y = 0; y < cp_height; y++)
	{
		memcpy( dst_start+dst_offset,
				 src_start+src_offset, line_byte);
		dst_offset+=dst_img->u32Width;
		src_offset+=src_img->u32Width;
	}

	dst_img->unlock();
	src_img->unlock();
}


void BaseImage::fillColor(const Color32 color){
    Color32* pDstLine=(Color32*)pSrcBuffer;
    for (long y=0;y<u32Height;++y){
        for (long x=0;x<u32Width;++x){
            pDstLine[x]=color;
        }
        nextLine(pDstLine);
    }
}

void image::dump_from_buf(const void * buf, int width, int height)
{
	lock();
	SetBuffer(width, height);
	memcpy(pSrcBuffer, buf, SrcSize);
	unlock();
}
void image::dump_to_buf_part(void * buf,int src_x,int src_y,int src_w,int src_h, int dst_w,int dst_h,int dst_x, int dst_y)
	{
		int x;
		int y;
		//int s_x=src_x;
		//int s_y=src_y;
		int cp_w=src_w;
		int cp_h=src_h;


		if (cp_h <= 0 || cp_w <= 0)
		{
			return ;
		}


		if (src_x < 0)
		{
			dst_x -= src_x;
			src_x = 0;
		}
		if (src_y < 0)
		{
			dst_y -= src_y;
			src_y = 0;
		}
		if (dst_x < 0)
		{
			src_x -= dst_x;
			dst_x = 0;
		}
		if (dst_y < 0)
		{
			src_y -= dst_y;
			dst_y = 0;
		}



		if (src_y + cp_h > GetHeight())
		{
			//printf("AreaCopy src_y=%d cp_height=%d rsc_img->get_height()=%d\r\n", src_y, cp_height, rsc_img->get_height());
			cp_h = GetHeight() - src_y;
			if (cp_h <= 0)
			{
				return ;
			}
		}
		if (src_x + cp_w > GetWidth())
		{
			//printf("AreaCopy src_x=%d cp_width=%d rsc_img->get_width()=%d\r\n", src_x, cp_width, rsc_img->get_width());
			cp_w =GetWidth() - src_x;
			if (cp_w <= 0)
			{
				return ;
			}
		}
		if (dst_y + cp_h > dst_h)
		{
			cp_h = dst_h - dst_y;
			if (cp_h <= 0)
			{
				return ;
			}
		}
		if (dst_x + cp_w > dst_w)
		{
			cp_w = dst_w - dst_x;
			if (cp_w <= 0)
			{
				return ;
			}
		}

		lock();
		int line_byte=cp_w <<2;
		unsigned int dst_step= dst_w;
		unsigned int src_step= u32Width;
		unsigned int * dst_start;
		unsigned int * src_start=(unsigned int *)pSrcBuffer +src_y * src_step + src_x;
		unsigned int dst_offset=0;
		unsigned int src_offset=0;


#ifdef CONFIG_SCREEN_ROTATE_90
	int d_temp_y=dst_x;
	int d_temp_x=dst_h-dst_y-cp_h-1;
	 dst_step= dst_h;
	dst_start=(unsigned int *)buf +  d_temp_y * dst_step + d_temp_x;
	src_start+=cp_h*src_step;
    for(size_t i=0;i<cp_w;i++){
    	for(int j=0;j<cp_h;j++){
    		*(dst_start+dst_offset+j)=*(src_start+src_offset+i);
    		src_offset-=src_step;
    	}
    	src_offset=0;
    	dst_offset+=dst_step;
    }

#elif CONFIG_SCREEN_ROTATE_180
		dst_x=dst_w-dst_x-1;
		dst_y=dst_h-dst_y-1;
		dst_start=(unsigned int *)buf +  dst_y * dst_step + dst_x;

		for (y = 0; y<cp_h; y++)
		{
			//memcpy( dst_start+dst_offset,src_start+src_offset, line_byte);
			for(x=0;x<cp_w;x++){
				*(dst_start+dst_offset-x)=*(src_start+src_offset+x);
			}
			dst_offset-=dst_step;
			src_offset+=src_step;
		}
#else
		//log_i("===src_x=%d,src_y=%d,dst_x=%d,dst_y=%d,cp_w=%d,cp_h=%d====\n",src_x,src_y,dst_x,dst_y,cp_w,cp_h);
		dst_start=(unsigned int *)buf +  dst_y * dst_step + dst_x;
		for (y = 0; y < cp_h; y++)
		{
			memcpy( dst_start+dst_offset, src_start+src_offset, line_byte);
			dst_offset+=dst_step;
			src_offset+=src_step;
		}
	#endif
		unlock();
	}


int image::SetBuffer(int width, int height,unsigned int color)
{
	//path.format("SetBuffer-%dx%d",width,height);
	lock();
	static int dep = 4;
	int tmpsize = width * height * dep;
	if (tmpsize > SrcSize)
	{
		destroy();
	}
	if (pSrcBuffer == NULL)
	{
//		pSrcBuffer = malloc(tmpsize);
//		if (pSrcBuffer == NULL)
//		{
//			errexitf("image malloc failed: width=%d height=%d\n", width, height);
//		}

		int err = posix_memalign(&pSrcBuffer, 4, tmpsize);
		if (err) {
			log_e("posix_memalign failed: %s\n", strerror(err));
			return err;
		}

	}
	SrcSize = tmpsize;
	u32Width = width;
	u32Height = height;
	u32Stride = width * dep;
	cleanBuf(color);
	unlock();
	return 0;
}

int image::SetBuffer(int width, int height)
{

	//path.format("SetBuffer-%dx%d",width,height);
	lock();
	static int dep = 4;

	int tmpsize = width * height * dep;
	if (tmpsize > SrcSize)
	{
		destroy();
	}
	if (pSrcBuffer == NULL)
	{
		//pSrcBuffer = malloc(tmpsize);

		int err = posix_memalign(&pSrcBuffer, 4, tmpsize);
		if (err) {
			log_e("posix_memalign failed: %s\n", strerror(err));
			return false;
		}
//
//		if (pSrcBuffer == NULL)
//		{
//			errexitf("image malloc failed: width=%d height=%d\n", width, height);
//		}
	}
	SrcSize = tmpsize;

	u32Width = width;
	u32Height = height;
	u32Stride = width * dep;

	cleanBuf();
	unlock();
	return 0;
}

void image::AreaCmp(image * img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
{
	int x;
	int y;
	for (y = 0; y < cp_height; y++)
	{
		for (x = 0; x < cp_width; x++)
		{
			if (*((unsigned int *) pSrcBuffer + (y + dst_y) * u32Width + dst_x + x)
					== *((unsigned int *) img->pSrcBuffer + (y + src_y) * img->u32Width + src_x + x))
			{
				*((unsigned int *) pSrcBuffer + (y + dst_y) * u32Width + dst_x + x) = 0;
			}
		}
	}
}
