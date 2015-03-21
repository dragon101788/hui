#ifndef __CODEC_H__
#define __CODEC_H__

#include <string>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "include/autoconf.h"

using namespace std;

#include "hui.h"
//#include "jpeg.h"
//#include "jpegcodec.h"
//#include "w55fa92_blt.h"
//#include "blt.h"
#include "hulib.h"
#include "alpha.h"

extern int snapcache;


void MyMemMove(void *dst,const void *src,size_t count);
//int bmpCodec_to_image(image * sobj, const char *filename);
//int jpegCodec_to_image(image * sobj, const char *filename);
int pngCodec_to_image(image * sobj, const char *filepath);
int pngEndec_to_image(const char *file_name, image * graph);
int codec_to_Image(image * enode, const char * filename);


void Render_img_to_buf(void * buf, image * img, int width, int height);
void ProcArea(image * dst_img, image * rsc_img, int & src_x, int & src_y, int & cp_width, int & cp_height, int & dst_x, int & dst_y);
void AreaCopy(image * dst_img, image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y);

int fill_image(image * img, int a = 255, int r = 128, int g = 128, int b = 128);
int access_Image(const char * filename);

//typedef struct
//{
//	unsigned char u8Blue;
//	unsigned char u8Green;
//	unsigned char u8Red;
//	unsigned char u8Alpha;
//} IMG_PIX;

//base type
typedef char            Int8;
typedef unsigned char   UInt8;
typedef short           Int16;
typedef unsigned short  UInt16;
typedef long            Int32;
typedef unsigned long   UInt32;

//32bitARGB颜色类型
struct Color32 {

    union {
        UInt32   argb;
        struct
        {
            UInt8  b;
            UInt8  g;
            UInt8  r;
            UInt8  a;
        };
    };
// inline Color32(){}
//inline static UInt32 packColor32Data(unsigned long r8,unsigned long g8,unsigned long b8,unsigned long a8=255) { return b8|(g8<<8)|(r8<<16)|(a8<<24);  }
//    inline Color32(const Color32& color32):argb(color32.argb){}
//    inline explicit Color32(const UInt32 color32):argb(color32){}
//    inline explicit Color32(unsigned long r8,unsigned long g8,unsigned long b8,unsigned long a8=255):argb(packColor32Data(r8,g8,b8,a8)){}
//    inline bool operator ==(const Color32& color32) const { return argb==color32.argb; }
//    inline bool operator !=(const Color32& color32) const{ return !((*this)==color32); }
};


//图像数据区的描述信息
class base_image{
public:
	void * pSrcBuffer;
	unsigned long SrcSize;
	unsigned int u32Width;	// crop width
	unsigned int u32Height;	// crop height
	unsigned int u32Stride;
	~base_image(){};
	//////////////////////////////////////////////////////////////////////////
    inline base_image() :pSrcBuffer(0),u32Stride(0),u32Width(0),u32Height(0),SrcSize(0){}
    inline base_image(const base_image& ref) :pSrcBuffer(ref.pSrcBuffer),u32Stride(ref.u32Stride),u32Width(ref.u32Width),u32Height(ref.u32Height),SrcSize(ref.SrcSize){}
//
    //访问(x,y)坐标处的颜色
    inline Color32& pixels(const long x,const long y) const { return getLinePixels(y)[x]; }
    //得到y行的颜色首地址
    inline Color32* getLinePixels(const long y) const { return (Color32*) ( ((UInt8*)pSrcBuffer) + u32Stride*y ); }

    //是否是空图像区
    inline bool getIsEmpty()const { return ((u32Width<=0)||(u32Height<=0)); }
    //将pline指向下一行颜色
    inline void nextLine(Color32*& pline)const {  ((UInt8*&)pline)+=u32Stride;   }

    //坐标边界饱和  如果(x,y)坐标在图片数据区外,(x,y)值会被设置到图片最近的边界内,并返回false(否则什么也不做,返回true) //警告! 图片区域不能为空
    inline bool clipToBorder(long& x, long& y)const{
        bool isIn = true;
        if (x < 0) {
            isIn = false; x = 0;
        } else if (x >= u32Width) {
            isIn = false; x = u32Width - 1;
        }
        if (y < 0) {
            isIn = false; y = 0;
        } else if (y >= u32Height) {
            isIn = false; y = u32Height - 1;
        }
        return isIn;
    }
    //获取一个点的颜色,默认执行边界饱和测试  当坐标超出区域的时候返回的颜色为最近的边界上的颜色值并且其alpha通道置零  //警告! 图片区域不能为空 速度很慢
    inline Color32 getPixelsBorder(long x, long y) const {
        bool isInPic = clipToBorder(x,y);
        Color32 result = pixels(x,y);
        if (!isInPic)
            result.a=0;
        return result;
    }
    inline bool getIsInPic(long x, long y)const{
        //return (x>=0)&&(y>=0)&&(x<width)&&(y<height);
        //利用负数的编码优化为:
        return ((unsigned long)x<(unsigned long)u32Width)&&((unsigned long)y<(unsigned long)u32Height);
    }

    //填充颜色
    void fillColor(const Color32 color)const ;

	int GetWidth()
	{
		return u32Width;
	}
	int GetHeight()
	{
		return u32Height;
	}
};



class image:public base_image, virtual public Mutex
{
	/*
protected:

//	Color32 * pSrcBuffer;


public:
	void * pSrcBuffer;
	unsigned long SrcSize;
	unsigned int u32Width;	// crop width
	unsigned int u32Height;	// crop height
	unsigned int u32Stride;



	//////////////////////////////////////////////////////////////////////////
//    inline TPixels32Ref() :pdata(0),byte_width(0),width(0),height(0){}
//    inline TPixels32Ref(const TPixels32Ref& ref) :pdata(ref.pdata),byte_width(ref.byte_width),width(ref.width),height(ref.height){}
//
    //访问(x,y)坐标处的颜色
    inline Color32& pixels(const long x,const long y) const { return getLinePixels(y)[x]; }
    //得到y行的颜色首地址
    inline Color32* getLinePixels(const long y) const { return (Color32*) ( ((UInt8*)pSrcBuffer) + u32Stride*y ); }

    //是否是空图像区
    inline bool getIsEmpty()const { return ((u32Width<=0)||(u32Height<=0)); }
    //将pline指向下一行颜色
    inline void nextLine(Color32*& pline)const {  ((UInt8*&)pline)+=u32Stride;   }

    //坐标边界饱和  如果(x,y)坐标在图片数据区外,(x,y)值会被设置到图片最近的边界内,并返回false(否则什么也不做,返回true) //警告! 图片区域不能为空
    inline bool clipToBorder(long& x, long& y)const{
        bool isIn = true;
        if (x < 0) {
            isIn = false; x = 0;
        } else if (x >= u32Width) {
            isIn = false; x = u32Width - 1;
        }
        if (y < 0) {
            isIn = false; y = 0;
        } else if (y >= u32Height) {
            isIn = false; y = u32Height - 1;
        }
        return isIn;
    }
    //获取一个点的颜色,默认执行边界饱和测试  当坐标超出区域的时候返回的颜色为最近的边界上的颜色值并且其alpha通道置零  //警告! 图片区域不能为空 速度很慢
    inline Color32 getPixelsBorder(long x, long y) const {
        bool isInPic = clipToBorder(x,y);
        Color32 result = pixels(x,y);
        if (!isInPic)
            result.a=0;
        return result;
    }
    inline bool getIsInPic(long x, long y)const{
        //return (x>=0)&&(y>=0)&&(x<width)&&(y<height);
        //利用负数的编码优化为:
        return ((unsigned long)x<(unsigned long)u32Width)&&((unsigned long)y<(unsigned long)u32Height);
    }

    //填充颜色
    void fillColor(const Color32 color)const{
        Color32* pDstLine=(Color32*)pSrcBuffer;
        for (long y=0;y<u32Height;++y){
            for (long x=0;x<u32Width;++x){
                pDstLine[x]=color;
            }
            nextLine(pDstLine);
        }
    }

    ////////////////////////////////////////////////////

*/
public:
	int transp;             //透明度
	hustr path;

	friend void Render_img_to_img(image * dst, image * src, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y);
	friend int image_write_to_snap(image * img, const char * rawpath);
	friend int image_read_from_snap(image * img, const char * rawpath);
	friend void AreaCopy(image * dst_img, image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y);
	int getSize()
	{
		return SrcSize;
	}
	/*
	int GetWidth()
	{
		return u32Width;
	}
	int GetHeight()
	{
		return u32Height;
	}
*/
	void setTransp(int n)
	{
		lock();
		transp = n;
		unlock();
	}
	void dump_from_buf(const void * buf, int width, int height);

	void dump_to_buf(void * buf)
	{
		lock();
		//memcpy(buf, pSrcBuffer, SrcSize);
#ifdef CONFIG_REVERSE_SCREEN
		MyMemMove(buf, pSrcBuffer, SrcSize);
#else
		memcpy(buf, pSrcBuffer, SrcSize);
#endif
		unlock();
	}
	int isNULL()
	{
		return (pSrcBuffer == NULL);
	}

	image& operator=(const image &img)
	{
		if (SrcSize < img.SrcSize)
		{
			destroy();
		}
		SetBuffer(img.u32Width, img.u32Height);
		memcpy(pSrcBuffer, img.pSrcBuffer, SrcSize);
		path = img.path;
	}


	 void dump_to_buf_part(void * buf,int src_x,int src_y,int src_w,int src_h, int dst_w,int dst_h,int dst_x, int dst_y);


//	inline void SetPix(int pos, int r, int g, int b, int a)
//	{
//		IMG_PIX * tmp = GetPix(pos);
//		tmp->u8Red = r;
//		tmp->u8Green = g;
//		tmp->u8Blue = b;
//		tmp->u8Alpha = a;
//	}
//	inline void SetPix(int x, int y, int r, int g, int b, int a)
//	{
//		IMG_PIX * tmp = GetPix(x,y);
//		tmp->u8Red = r;
//		tmp->u8Green = g;
//		tmp->u8Blue = b;
//		tmp->u8Alpha = a;
//	}
//	inline Color32 * GetPix(int x, int y)
//	{
//		if (x > u32Width || y > u32Height)
//		{
//			huErrExitf("Get pixels beyond the range %s x=%d y=%d width=%d height=%d\r\n", path.c_str(), x, y, u32Width, u32Height);
//		}
//		return GetPix(y * u32Width + x);
//	}
	inline Color32 * GetPix(int pos)
	{
		if ((pos * 4) > SrcSize)
		{
			huErrExitf("Get pixels beyond the range %s pos=%d width=%d height=%d\r\n", path.c_str(), pos, u32Width, u32Height);
		}
		return ((Color32 *) pSrcBuffer + pos);
	}

	unsigned long SrcGPUAddr()
	{
		return (unsigned long) pSrcBuffer;
	}

	void SetResource(const char * filepath)
	{
		//printf("SetResource %s\r\n",filepath);

		if (path != filepath)
		{
			destroy();
			path = filepath;
		}
	}
	int LoadResource()
	{
		if (pSrcBuffer != NULL)
		{
			return 0;
		}
		if (path.empty())
		{
			log_w("warning LoadResource empty path\r\n");
			return -1;
		}
		return codec_to_Image(this, path.nstr());//装载图片
	}
	void ReSetResource(const char * filepath)
	{
		if (path != filepath)
		{
			path = filepath;
			codec_to_Image(this, path.c_str());
		}
	}
	void SaveResource(const char * file)
	{
		path = file;
		pngEndec_to_image(file, this);
	}

	int SetBuffer(int width, int height);

//	void ProcArea(image * rsc_img, int & src_x, int & src_y, int & cp_width, int & cp_height, int & dst_x, int & dst_y)
//	{
//		if (cp_width == 0 || cp_height == 0)
//		{
//			return;
//		}
//		if (cp_width < 0 || cp_height < 0)
//		{
//			errexit("AreaCopy cp_windth or cp_height <0\r\n");
//		}
//		//printf("$$$HU$$$ src_x=%d\r\n", src_x);
//
//		if (src_x < 0)
//		{
//			dst_x -= src_x;
//			src_x = 0;
//		}
//		if (src_y < 0)
//		{
//			dst_y -= src_y;
//			src_y = 0;
//		}
////		if (dst_x < 0)
////		{
////			dst_x = 0;
////		}
////		if (dst_y < 0)
////		{
////			dst_y = 0;
////		}
//		if (src_y + cp_height > rsc_img->u32Height)
//		{
//			//printf("AreaCopy src_y=%d cp_height=%d rsc_img->u32Height=%d\r\n", src_y, cp_height, rsc_img->u32Height);
//			cp_height = rsc_img->u32Height - src_y;
//			if (cp_height <= 0)
//			{
//				return;
//			}
//		}
//		if (src_x + cp_width > rsc_img->u32Width)
//		{
//			//printf("AreaCopy src_x=%d cp_width=%d rsc_img->u32Width=%d\r\n", src_x, cp_width, rsc_img->u32Width);
//			cp_width = rsc_img->u32Width - src_x;
//			if (cp_width <= 0)
//			{
//				return;
//			}
//		}
//		if (dst_y + cp_height > u32Height)
//		{
//			cp_height = u32Height - dst_y;
//		}
//		if (dst_x + cp_width > u32Width)
//		{
//			cp_width = u32Width - dst_x;
//		}
//	}
	void AreaCopy(image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
	{

		::AreaCopy(this, src_img, src_x, src_y, cp_width, cp_height, dst_x, dst_y);
	}

//	void AreaCopy_transparent(image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
//	{
//		ProcArea(src_img, src_x, src_y, cp_width, cp_height, dst_x, dst_y);
//
//		unsigned int * src_buf = (unsigned int*) src_img->pSrcBuffer;
//		unsigned int src_w = src_img->u32Width;
//		unsigned int * dst_buf = (unsigned int*) pSrcBuffer;
//		unsigned int dst_w = u32Width;
//		int dx = dst_x;
//		int dy = dst_y;
//		for (int sy = 0; sy < src_img->u32Height; sy++)
//		{
//			for (int sx = 0; sx < src_img->u32Width; sx++)
//			{
//				IMG_PIX * sp = (IMG_PIX *) (src_buf + sx + sy * src_w);
//				if ((*sp).u8Alpha != 0)
//				{
//					IMG_PIX * dp = (IMG_PIX *) (dst_buf + dx + sx + (dy + sy) * dst_w);
//
//					//				S_DRVBLT_ARGB8 * up = (S_DRVBLT_ARGB8 *) ((unsigned int*) g_lay_mgr[0]->pSrcBuffer + dx + sx
//					//						+ (dy + sy) * g_lay_mgr[0]->u32Width);
//
//					*dp = *sp;
//
//				}
//			}
//		}
//	}

	void AreaCopy(image * img, int x, int y)
	{
		AreaCopy(img, 0, 0, img->u32Width, img->u32Height, x, y);
	}

	void DrawPoint(int x, int y, int color)
	{
		*((unsigned int *) pSrcBuffer + y * u32Width + x) = color;
	}
	void DrawBrush(int x, int y, int color)
	{
		DrawPoint(x, y, color);
		DrawPoint(x, y + 1, color);
		DrawPoint(x, y + 2, color);
		DrawPoint(x, y - 1, color);
		DrawPoint(x, y - 2, color);

		DrawPoint(x + 1, y, color);
		DrawPoint(x + 2, y, color);

		DrawPoint(x - 1, y, color);
		DrawPoint(x - 2, y, color);
	}

	void AreaCmp(image * img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y);
	void cleanBuf()
	{
		if (pSrcBuffer)
			memset(pSrcBuffer, 0, SrcSize);
	}

	void debug_info()
	{
		printf("u32Width=%d\r\n", u32Width);
		printf("u32Height=%d\r\n", u32Height);
		printf("u32Stride=%d\r\n", u32Stride);
		printf("SrcGPUAddr=0x%x\r\n", SrcGPUAddr());

	}

	void Render(image * img, int x, int y)
	{
		img->LoadResource();
		Render_img_to_img(this, img, 0, 0, img->u32Width, img->u32Height, x, y);
	}
	void Render(image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
	{
		src_img->LoadResource();
		ProcArea(this, src_img, src_x, src_y, cp_width, cp_height, dst_x, dst_y);
		Render_img_to_img(this, src_img, src_x, src_y, cp_width, cp_height, dst_x, dst_y);
	}

//	int SetConfig(const char * key,const char * value)
//	{
//		if(strcasecmp(key,"alpha")==0)
//		{
//			stransformation.colorMultiplier.i16Alpha = strtoul(value,NULL,10);
//		}
//		else
//		{
//			return -1;
//		}
//		return 0;
//	}

//	
inline 	image()
	{

//		pSrcBuffer = NULL;
//		SrcSize = 0;
//		u32Width = 0;       // crop width
//		u32Height = 0;      // crop height
//		u32Stride = 0;
		transp = 100;		//控件透明度-默认不透明

		//s_sblitop.transformation = &stransformation;

	}

	virtual ~image()
	{
		destroy();
	}

	//释放空间并不改变其他内容
	void Free()
	{
		if (pSrcBuffer != NULL)
		{
			log_i("destroy image pSrcBuffer [%s] %dx%d\r\n", path.c_str(), u32Width, u32Height);
			free(pSrcBuffer);
			pSrcBuffer = NULL;
		}
	}
	void destroy()
	{

		if (pSrcBuffer != NULL)
		{
			log_i("destroy image pSrcBuffer [%s] %dx%d\r\n", path.c_str(), u32Width, u32Height);
			free(pSrcBuffer);
			pSrcBuffer = NULL;
			SrcSize = 0;
			u32Width = 0;       // crop width
			u32Height = 0;      // crop height
			u32Stride = 0;
			path.clear();
		}
	}

};




#endif //__CODEC_H__
