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

extern int go;
#define RGB565(r,g,b)  ( ((r>>3)&(1<<5)-1)<<11 | ((g>>2)&(1<<6)-1)<<5 | ((b>>3)&(1<<5)-1)<<0 )
class framebuffer: public Mutex//, public thread,public Sem
{

public:
	int lcm_fd;
	void * pSrcBuffer;
	unsigned long SrcSize;
	unsigned int u32Width;	// crop width
	unsigned int u32Height;	// crop height
	int lcm_dpp;
	hustr snap;

//	image *from_img;

//
//	int run()
//	{
//
//		while(go){
//			waitSem();
//			lock();
//			if(from_img!=NULL){
//				RenderImageToFrameBuffer(from_img);
//			}
//			unlock();
//
//		}
//
//	}

	framebuffer()
	{
		Accept();
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
#ifdef CONFIG_SCREEN_ROTATE_90
		u32Width = var.yres;
		u32Height = var.xres;
#else
		u32Width = var.xres;
		u32Height = var.yres;
#endif
		lcm_dpp = var.bits_per_pixel;
		log_i("FrameBuffer Accept %d %d %d %d %d %d %d\r\n",u32Width,u32Height,lcm_dpp,var.xres_virtual,var.yres_virtual,var.xoffset,var.yoffset);
		SrcSize = u32Width * u32Height * (lcm_dpp / 8);
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

//	void NotifyRenderFrameBuffer(image * img){
//		//log_i("RenderImageToFrameBuffer in!!!\n");
//		lock();
//    	from_img=img;
//    	unlock();
//    	postSem();
//    //	log_i("RenderImageToFrameBuffer out!!!!\n");
//    }
	void RenderImageToFrameBuffer(image * img);
	void RenderImageToFrameBuffer_part(image * img,int src_x,int src_y,int src_w,int src_h,int dst_x,int dst_y);

};

extern framebuffer fb;

#endif //__FRAMEBUFFER_H__
