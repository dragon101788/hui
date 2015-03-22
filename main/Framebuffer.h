#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include <string>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "hulib.h"
#include "codec.h"
#include "autoconf.h"
#include <semaphore.h>

using namespace std;

#define DEVMEM_GET_STATUS	_IOR('m', 4, unsigned int)
#define DEVMEM_SET_STATUS	_IOW('m', 5, unsigned int)
#define DEVMEM_GET_PHYSADDR	_IOR('m', 6, unsigned int)
#define DEVMEM_GET_VIRADDR	_IOR('m', 7, unsigned int)

#define IOCTL_LCD_ENABLE_INT	_IO('v', 28)
#define IOCTL_LCD_DISABLE_INT	_IO('v', 29)

//fbflush fbf;
extern int go;

//static pthread_mutex_t mutex;

#define RGB565(r,g,b)  ( ((r>>3)&(1<<5)-1)<<11 | ((g>>2)&(1<<6)-1)<<5 | ((b>>3)&(1<<5)-1)<<0 )
class framebuffer: public Mutex, public thread,public Sem
{

public:
	int lcm_fd;
	void * pSrcBuffer;
	unsigned long SrcSize;
	unsigned int u32Width;	// crop width
	unsigned int u32Height;	// crop height
	int lcm_dpp;
//	image put;
//	image in_img;
//	image out_img;
	hustr snap;

	image *from_img;
	//sem_t sem;//信号量
	/*
    pthread_cond_t cond;
	int flag;

	void pthread_init()
	{
	    pthread_cond_init(&cond,NULL);
	}

	void pthread_suspend()
	{
	    lock();
	    flag--;
	    unlock();
	}


	void pthread_resume()
	{
		lock();
	    flag++;
	    pthread_cond_signal(&cond);
	    unlock();
	}

*/


	int run()
	{

		while(go){
			//printf("run  IN!!!\n");
			//sem_wait(&sem);
			waitSem();
			lock();
			if(from_img!=NULL){
		//	from_img->lock();
		//	printf("run  RenderImageToFrameBuffer!!!\n");
			RenderImageToFrameBuffer(from_img);
		//	from_img->unlock();
			}
			unlock();
		//	printf("run  out!!!\n");

		}
		/*
		//return 0;
		put.dump_from_buf(pSrcBuffer,u32Width,u32Height);
		//memcpy(put.pSrcBuffer, pSrcBuffer, SrcSize);
		if (!snap.empty())
		{
			printf("fbf snap = %s\r\n", snap.c_str());
			out_img.ReSetResource(snap);
			snap.clear();
		}
		//put.cleanBuf();
		for (int i = 1; i <= 5; i++)
		{
			printf("fbflush run %d\r\n", i);
//			in_img.stransformation.colorMultiplier.i16Alpha = 255 - 32 * i;
//			put.Render(&in_img, 0, 0);
			out_img.setTransp(20 * i);
			put.Render(&out_img, 0, 0);
			RenderImageToFrameBuffer(&put);
			usleep(1000*30);

			}*/
	}

//	void DumpToXml(image & out)
//	{
//		//cancel();
//		wait();
//		out_img = out;
//		create();
//	}
//	void DumpToSnap(const char * path)
//	{
//		//cancel();
//		printf("DumpToSnap %s\r\n", path);
//		wait();
//		snap = path;
//		create();
//	}
	framebuffer()
	{
		Accept();
		/*
		put.path = "framebuffer change put buf";
		in_img.path = "framebuffer change in buf";
		out_img.path = "framebuffer change out buf";

		put.SetBuffer(u32Width, u32Height);
		in_img.SetBuffer(u32Width, u32Height);
		out_img.SetBuffer(u32Width, u32Height);
		*/
		//flag=0;
		//pthread_init();


	}
	~framebuffer()
	{
		Destroy();
	}
	int Accept()
	{
		lcm_fd = open("/dev/fb0", O_RDWR);
		if (lcm_fd == -1)
		{
			errexitf("### Error: cannot open LCM device, returns %d\n", lcm_fd);
		}

		struct fb_var_screeninfo var;
		if (ioctl(lcm_fd, FBIOGET_VSCREENINFO, &var) == -1)
		{
			errexitf("### Error: ioctl FBIOGET_VSCREENINFO: %s\n", strerror(errno));
		}

		u32Width = var.xres;
		u32Height = var.yres;

		lcm_dpp = var.bits_per_pixel;
		log_i("FrameBuffer Accept %d %d %d %d %d %d %d\r\n",u32Width,u32Height,lcm_dpp,var.xres_virtual,var.yres_virtual,var.xoffset,var.yoffset);
		SrcSize = u32Width * u32Height * (lcm_dpp / 8);
		//log_i("$$$luo$$$ SrcSize is %d \r\n",SrcSize);

		pSrcBuffer = mmap(NULL, SrcSize, PROT_READ | PROT_WRITE, MAP_SHARED, lcm_fd, 0);
		if (pSrcBuffer == MAP_FAILED)
		{
			errexitf("### Error: failed to map LCM device to memory: %s\n", strerror(errno));
		}

		return 0;
	}

	void Destroy()
	{
		log_i("$$$HU$$$ framebuffer Destroy\r\n");
		if (pSrcBuffer != NULL && pSrcBuffer != MAP_FAILED)
		{
			munmap(pSrcBuffer, SrcSize);
			pSrcBuffer = NULL;
		}
		pSrcBuffer = NULL;
		close(lcm_fd);
		lcm_fd = -1;
	}

	void NotifyRenderFrameBuffer(image * img){
		//log_i("RenderImageToFrameBuffer in!!!\n");
		lock();
    	from_img=img;
    	unlock();
    	//sem_post(&sem);
    	postSem();
    //	log_i("RenderImageToFrameBuffer out!!!!\n");
    }
	void RenderImageToFrameBuffer(image * img);
	void RenderImageToFrameBuffer_part(image * img,int src_x,int src_y,int src_w,int src_h,int dst_x,int dst_y);

};

extern framebuffer fb;

#endif //__FRAMEBUFFER_H__
