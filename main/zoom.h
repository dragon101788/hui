

#ifndef _ZOOM_H_
#define _ZOOM_H_



extern void PicZoom_no_Bilinear(const base_image& Dst,const base_image& Src);
extern void PicZoom_Bilinear(const base_image& Dst,const base_image& Src);
extern void PicZoom_ftBilinear(const base_image& Dst,const base_image& Src);
#endif
