

#ifndef _ROTATE_H_
#define _ROTATE_H_






extern void PicRotary2(const image& Dst,const image& Src,double RotaryAngle,double ZoomX,double ZoomY,double move_x,double move_y);
extern void center_rotate( image & dstPic,image &  srcPic,int angle,double ZoomX,double ZoomY);
extern void  PicRotaryBilInear(const base_image & Dst,const base_image& Src,double RotaryAngle,double ZoomX,double ZoomY,double move_x,double move_y);
//    TFileInputStream bmpInputStream("test1.bmp");
//#endif
//    TBmpFile::load(&bmpInputStream,&srcPic);//加载源图片


extern  void rotate( image & dstPic,image &  srcPic,int angle,double ZoomX,double ZoomY,double move_x,double move_y);


#endif
