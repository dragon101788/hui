

#ifndef _ZOOM_SOFT_H_
#define _ZOOM_SOFT_H_



extern void PicZoom_no_Bilinear(const BaseImage& Dst,const BaseImage& Src);
extern void PicZoom_Bilinear(const BaseImage& Dst,const BaseImage& Src);
extern void PicZoom_ftBilinear(const BaseImage& Dst,const BaseImage& Src);
#endif
