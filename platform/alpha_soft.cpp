#include "main/codec.h"


#define MAX_ALPHA 255
#define MIN_ALPHA 0




void Render_img_to_img(image * dst, image * src, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
{
//	ProcArea(dst, src, src_x, src_y, cp_width, cp_height, dst_x, dst_y);
	if (dst->SrcGPUAddr() == 0 || src->SrcGPUAddr() == 0)
	{
		errexitf("warning::Image source point is NULL dst=%#x src=%#x\r\n", dst->SrcGPUAddr(), src->SrcGPUAddr());
	}

	int x;
	int y;
	int alpha;
	Color32 * dst_pix ;
	Color32 * src_pix ;
	for (y = 0; y < cp_height; y++)
	{
		for (x = 0; x < cp_width; x++)
		{
			src_pix = ((Color32 *) src->pSrcBuffer + (y + src_y) * src->u32Width + x + src_x);
			if(src_pix->a<5){//透明度小于5基本看不清了，可以不复制

			}else if(src_pix->a>250){  //大于250时，底图片的像素基本看不清了，不如直接覆盖
			 *((unsigned int *) dst->pSrcBuffer + (y + dst_y) * dst->u32Width + x + dst_x)=*(unsigned int *)src_pix;
			}else{
				// alpha = src_pix->a*src->transp/100;
				 alpha = src_pix->a;  //图片的预设透明度失效
				 dst_pix = ((Color32 *) dst->pSrcBuffer + (y + dst_y) * dst->u32Width + x + dst_x);
	//			dst_pix->r =  (dst_pix->r * (MAX_ALPHA - alpha) + src_pix->r * alpha) / MAX_ALPHA;
	//			dst_pix->g = (dst_pix->g * (MAX_ALPHA - alpha) + src_pix->g * alpha) / MAX_ALPHA;
	//			dst_pix->b =  (dst_pix->b * (MAX_ALPHA - alpha) + src_pix->b * alpha) / MAX_ALPHA;
				dst_pix->r =  (dst_pix->r * (MAX_ALPHA - alpha) + src_pix->r * alpha) >>8;//除以255跟除以256差异不大，不如近似用位移
				dst_pix->g = (dst_pix->g * (MAX_ALPHA - alpha) + src_pix->g * alpha) >>8;
				dst_pix->b =  (dst_pix->b * (MAX_ALPHA - alpha) + src_pix->b * alpha) >>8;
				dst_pix->a = (dst_pix->a>src_pix->a?dst_pix->a:src_pix->a);
			}
			//((S_DRVBLT_ARGB8 *) dst->pSrcBuffer + (dst->y + dst_y) * dst->u32Width + dst->x + dst_x)->u8Alpha = 255;
		}
	}




}
