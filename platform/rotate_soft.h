

#ifndef _ROTATE_SOFT_H_
#define _ROTATE_SOFT_H_





extern void PicRotary2(const image& Dst,const image& Src,double RotaryAngle,double ZoomX,double ZoomY,double move_x,double move_y);
extern void center_rotate( image & dstPic,image &  srcPic,int angle,double ZoomX,double ZoomY);
extern void  PicRotaryBilInear(const base_image & Dst,const base_image& Src,double RotaryAngle,double ZoomX,double ZoomY,double move_x,double move_y);

extern  void rotate( image & dstPic,image &  srcPic,int angle,double ZoomX,double ZoomY,double move_x,double move_y);


#endif
