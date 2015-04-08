

#ifndef _IMAGE_STRANSFORM_H_
#define _IMAGE_STRANSFORM_H_
#include "rotate.h"
#include "zoom.h"

/***************
 * 此类用于图像变化，如旋转，缩放
 *
 */
class ImageTransform{
public:
	static void rotete_center( image & dstPic,image &  srcPic,int angle,double ZoomX,double ZoomY){
		::center_rotate( dstPic,srcPic, angle, ZoomX, ZoomY);
	}
	static void rotate( image & dstPic,image &  srcPic,int angle,double ZoomX,double ZoomY,double move_x,double move_y){
		::rotate(dstPic,srcPic, angle,ZoomX,ZoomY, move_x, move_y);
	}
	static void zoom_no_bilinear(const BaseImage& Dst,const BaseImage& Src){
		::PicZoom_no_Bilinear(Dst,Src);
	}
	static void zoom_bilinear(const BaseImage& Dst,const BaseImage& Src){
		::PicZoom_ftBilinear(Dst,Src);
	}
};
#endif
