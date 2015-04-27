#include "framebuffer.h"

framebuffer fb;
void framebuffer::RenderImageToFrameBuffer(image * img)
{
	if (img == NULL ||img->isNULL())
	{
		huErrExit("RenderFromBuffer Image invalid\r\n");
	}

	ioctl(lcm_fd, IOCTL_LCD_DISABLE_INT);
	if (lcm_dpp == 16)
	{
		img->lock();
		//log_i("$$$luo$$$ bpp=16\r\n");
		for(int y=0;y<u32Height;y++)
		{
			for(int x=0;x<u32Width;x++)
			{
				Color32 * pix = &img->pixels(x,y);
				*(((unsigned short*)pSrcBuffer)+(y*u32Width+x)) = RGB565(pix->r,pix->g,pix->b);
			}
		}
		img->unlock();
	}
	else if (lcm_dpp == 32)
	{
		//log_i("$$$luo$$$ bpp=32\r\n");
		img->dump_to_buf(pSrcBuffer);
	}
	//memcpy(pSrcBuffer, img->pSrcBuffer, SrcSize);

	ioctl(lcm_fd, IOCTL_LCD_ENABLE_INT);

}


 void framebuffer::RenderImageToFrameBuffer_part(image * img,int src_x,int src_y,int src_w,int src_h,int dst_x,int dst_y)
{
	if (img == NULL ||img->isNULL())
	{
		huErrExit("RenderFromBuffer Image invalid\r\n");
	}

	ioctl(lcm_fd, IOCTL_LCD_DISABLE_INT);

	 if (lcm_dpp == 32)
	{
		img-> dump_to_buf_part(pSrcBuffer,src_x,src_y,src_w,src_h,u32Width,u32Height,dst_x, dst_y);
	}

	ioctl(lcm_fd, IOCTL_LCD_ENABLE_INT);

}
