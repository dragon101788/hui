


#include <stdio.h>
#include <stdlib.h>
#include"codec.h"
#include"rotate.h"
#include <math.h>


#ifdef _MSC_VER
    #define must_inline __forceinline
    #ifndef _WIN64
        //启用内联x86汇编  x64时vc编译器不支持内联汇编 并且没有MMX寄存器
        #define asm __asm
        //使用MMX
        #define MMX_ACTIVE
    #endif
    typedef    __int64             Int64;
    typedef    unsigned __int64    UInt64;
#else
    #ifdef __GNUC__
        #define must_inline __attribute__((always_inline))
    #else
        #define must_inline inline
    #endif
    typedef    long long            Int64;
    typedef    unsigned long long   UInt64;
#endif


const double PI=3.1415926535897932384626433832795;
#ifdef asm
    //SinCos:同时计算sin(Angle)和cos(Angle)的内嵌x86汇编函数
    void __declspec(naked) __stdcall SinCos(const double Angle,double& sina,double& cosa)
    {
        asm
        {
            fld  qword ptr [esp+4]//Angle
            mov  eax,[esp+12]//&sina
            mov  edx,[esp+16]//&cosa
            fsincos
            fstp qword ptr [edx]
            fstp qword ptr [eax]
            ret 16
        }
    }
#else
    void SinCos(const double Angle,double& sina,double& cosa)
    {
        sina=sin(Angle);
        cosa=cos(Angle);
    }
#endif


    void center_rotate( image & dstPic,image &  srcPic,int angle,double ZoomX,double ZoomY)
    {
    	static int src_width=0;
    	static int src_height=0;
    	static long dst_wh=0;
    	if(src_width!=srcPic.GetWidth()||src_height!=srcPic.GetHeight()){
    	//	debug("src_height!=srcPic.GetHeight()!!!!\n");
    		src_width=srcPic.GetWidth();
    		src_height=srcPic.GetHeight();
    		dst_wh=(long)( ::sqrt(1.0*srcPic.GetWidth()*srcPic.GetWidth()+srcPic.GetHeight()*srcPic.GetHeight()) +4 +0.5);
    	}
    	dstPic.SetBuffer(dst_wh,dst_wh);
        double rotaryAngle=(PI*2)*(angle*1.0/360);
        PicRotaryBilInear(dstPic,srcPic,rotaryAngle,ZoomX,ZoomY, (dstPic.GetWidth()-srcPic.GetWidth())*0.5,(dstPic.GetHeight()-srcPic.GetHeight())*0.5);
    }

void rotate( image & dstPic,image &  srcPic,int angle,double ZoomX,double ZoomY,double move_x,double move_y)
{
	static int src_width=0;
	static int src_height=0;
	static long dst_wh=0;
	if(src_width!=srcPic.GetWidth()||src_height!=srcPic.GetHeight()){
		debug("src_height!=srcPic.GetHeight()!!!!\n");
		src_width=srcPic.GetWidth();
		src_height=srcPic.GetHeight();
		dst_wh=(long)( ::sqrt(1.0*srcPic.GetWidth()*srcPic.GetWidth()+srcPic.GetHeight()*srcPic.GetHeight()) +4 +0.5);
	}
    dstPic.SetBuffer(dst_wh,dst_wh);
    double rotaryAngle=(PI*2)*(angle*1.0/360);
  //  PicRotary2(dstPic,srcPic,rotaryAngle,ZoomX,ZoomY, move_x, move_y);
    PicRotaryBilInear(dstPic,srcPic,rotaryAngle,ZoomX,ZoomY, move_x, move_y);

}
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //RotaryAngle为逆时针旋转的角度;
    //ZoomX,ZoomY为x轴y轴的缩放系数(支持负的系数,相当于图像翻转);
    //move_x,move_y为x轴y轴的平移量;


    void PicRotary2(const base_image& Dst,const base_image& Src,double RotaryAngle,double ZoomX,double ZoomY,double move_x,double move_y)
    {

      //  if ( (fabs(ZoomX*Src.u32Width)<1.0e-4) || (fabs(ZoomY*Src.u32Height)<1.0e-4) ) return; //太小的缩放比例认为已经不可见
        double tmprZoomXY=1.0/(ZoomX*ZoomY);
        double rZoomX=tmprZoomXY*ZoomY;
        double rZoomY=tmprZoomXY*ZoomX;
        double sinA,cosA;
        SinCos(RotaryAngle,sinA,cosA);
        long Ax_16=(long)(rZoomX*cosA*(1<<16));
        long Ay_16=(long)(rZoomX*sinA*(1<<16));
        long Bx_16=(long)(-rZoomY*sinA*(1<<16));
        long By_16=(long)(rZoomY*cosA*(1<<16));
        double rx0=Src.u32Width*0.5;  //(rx0,ry0)为旋转中心
        double ry0=Src.u32Height*0.5;
        long Cx_16=(long)((-(rx0+move_x)*rZoomX*cosA+(ry0+move_y)*rZoomY*sinA+rx0)*(1<<16));
        long Cy_16=(long)((-(rx0+move_x)*rZoomX*sinA-(ry0+move_y)*rZoomY*cosA+ry0)*(1<<16));

        Color32* pDstLine=(Color32*)Dst.pSrcBuffer;
        long srcx0_16=(Cx_16);
        long srcy0_16=(Cy_16);
        for (long y=0;y<Dst.u32Height;++y)
        {
            long srcx_16=srcx0_16;
            long srcy_16=srcy0_16;
            for (long x=0;x<Dst.u32Width;++x)
            {
                long srcx=(srcx_16>>16);
                long srcy=(srcy_16>>16);
                srcx_16+=Ax_16;
                srcy_16+=Ay_16;
                if (Src.getIsInPic(srcx,srcy))
                    pDstLine[x]=Src.pixels(srcx,srcy);
            }
            srcx0_16+=Bx_16;
            srcy0_16+=By_16;
            ((UInt8*&)pDstLine)+=Dst.u32Stride;
        }
    }







    struct TRotaryClipData{
    public:
        long src_width;
        long src_height;
        long dst_width;
        long dst_height;
        long Ax_16;
        long Ay_16;
        long Bx_16;
        long By_16;
        long Cx_16;
        long Cy_16;
        long border_width;//插值边界宽度
    private:
        long cur_dst_up_x0;
        long cur_dst_up_x1;
        long cur_dst_down_x0;
        long cur_dst_down_x1;
        must_inline bool is_border_src(long src_x_16,long src_y_16)
        {
             return ( ( (src_x_16>=(-(border_width<<16)))&&((src_x_16>>16)<(src_width +border_width)) )
                   && ( (src_y_16>=(-(border_width<<16)))&&((src_y_16>>16)<(src_height+border_width)) ) );
        }
        must_inline bool is_in_src(long src_x_16,long src_y_16)
        {
             return ( ( (src_x_16>=(border_width<<16))&&((src_x_16>>16)<(src_width-border_width) ) )
                   && ( (src_y_16>=(border_width<<16))&&((src_y_16>>16)<(src_height-border_width)) ) );
        }
        void find_begin_in(long dst_y,long& out_dst_x,long& src_x_16,long& src_y_16)
        {
            src_x_16-=Ax_16;
            src_y_16-=Ay_16;
            while (is_border_src(src_x_16,src_y_16))
            {
                --out_dst_x;
                src_x_16-=Ax_16;
                src_y_16-=Ay_16;
            }
            src_x_16+=Ax_16;
            src_y_16+=Ay_16;
        }
        bool find_begin(long dst_y,long& out_dst_x0,long dst_x1)
        {
            long test_dst_x0=out_dst_x0-1;
            long src_x_16=Ax_16*test_dst_x0 + Bx_16*dst_y + Cx_16;
            long src_y_16=Ay_16*test_dst_x0 + By_16*dst_y + Cy_16;
            for (long i=test_dst_x0;i<=dst_x1;++i)
            {
                if (is_border_src(src_x_16,src_y_16))
                {
                    out_dst_x0=i;
                    if (i==test_dst_x0)
                        find_begin_in(dst_y,out_dst_x0,src_x_16,src_y_16);
                    if (out_dst_x0<0)
                    {
                        src_x_16-=(Ax_16*out_dst_x0);
                        src_y_16-=(Ay_16*out_dst_x0);
                    }
                    out_src_x0_16=src_x_16;
                    out_src_y0_16=src_y_16;
                    return true;
                }
                else
                {
                    src_x_16+=Ax_16;
                    src_y_16+=Ay_16;
                }
            }
            return false;
        }
        void find_end(long dst_y,long dst_x0,long& out_dst_x1)
        {
            long test_dst_x1=out_dst_x1;
            if (test_dst_x1<dst_x0) test_dst_x1=dst_x0;

            long src_x_16=Ax_16*test_dst_x1 + Bx_16*dst_y + Cx_16;
            long src_y_16=Ay_16*test_dst_x1 + By_16*dst_y + Cy_16;
            if (is_border_src(src_x_16,src_y_16))
            {
                ++test_dst_x1;
                src_x_16+=Ax_16;
                src_y_16+=Ay_16;
                while (is_border_src(src_x_16,src_y_16))
                {
                    ++test_dst_x1;
                    src_x_16+=Ax_16;
                    src_y_16+=Ay_16;
                }
                out_dst_x1=test_dst_x1;
            }
            else
            {
                src_x_16-=Ax_16;
                src_y_16-=Ay_16;
                while (!is_border_src(src_x_16,src_y_16))
                {
                    --test_dst_x1;
                    src_x_16-=Ax_16;
                    src_y_16-=Ay_16;
                }
                out_dst_x1=test_dst_x1;
            }
        }

        must_inline void update_out_dst_x_in()
        {
            if ((0==border_width)||(out_dst_x0_boder>=out_dst_x1_boder) )
            {
                out_dst_x0_in=out_dst_x0_boder;
                out_dst_x1_in=out_dst_x1_boder;
            }
            else
            {
                long src_x_16=out_src_x0_16;
                long src_y_16=out_src_y0_16;
                long i=out_dst_x0_boder;
                while (i<out_dst_x1_boder)
                {
                    if (is_in_src(src_x_16,src_y_16)) break;
                    src_x_16+=Ax_16;
                    src_y_16+=Ay_16;
                    ++i;
                }
                out_dst_x0_in=i;

                src_x_16=out_src_x0_16+(out_dst_x1_boder-out_dst_x0_boder)*Ax_16;
                src_y_16=out_src_y0_16+(out_dst_x1_boder-out_dst_x0_boder)*Ay_16;
                i=out_dst_x1_boder;
                while (i>out_dst_x0_in)
                {
                    src_x_16-=Ax_16;
                    src_y_16-=Ay_16;
                    if (is_in_src(src_x_16,src_y_16)) break;
                    --i;
                }
                out_dst_x1_in=i;
            }
        }
        must_inline void update_out_dst_up_x()
        {
            if (cur_dst_up_x0<0)
                out_dst_x0_boder=0;
            else
                out_dst_x0_boder=cur_dst_up_x0;
            if (cur_dst_up_x1>=dst_width)
                out_dst_x1_boder=dst_width;
            else
                out_dst_x1_boder=cur_dst_up_x1;
            update_out_dst_x_in();
        }
        must_inline void update_out_dst_down_x()
        {
            if (cur_dst_down_x0<0)
                out_dst_x0_boder=0;
            else
                out_dst_x0_boder=cur_dst_down_x0;
            if (cur_dst_down_x1>=dst_width)
                out_dst_x1_boder=dst_width;
            else
                out_dst_x1_boder=cur_dst_down_x1;
            update_out_dst_x_in();
        }

    public:
        long out_src_x0_16;
        long out_src_y0_16;

        long out_dst_up_y;
        long out_dst_down_y;

        long out_dst_x0_boder;
        long out_dst_x0_in;
        long out_dst_x1_in;
        long out_dst_x1_boder;

    public:
        bool inti_clip(double move_x,double move_y,unsigned long aborder_width)
        {
            border_width=aborder_width;

            //计算src中心点映射到dst后的坐标
            out_dst_down_y=(long)(src_height*0.5+move_y);
            cur_dst_down_x0=(long)(src_width*0.5+move_x);
            cur_dst_down_x1=cur_dst_down_x0;
            //得到初始扫描线
            if (find_begin(out_dst_down_y,cur_dst_down_x0,cur_dst_down_x1))
                find_end(out_dst_down_y,cur_dst_down_x0,cur_dst_down_x1);
            out_dst_up_y=out_dst_down_y;
            cur_dst_up_x0=cur_dst_down_x0;
            cur_dst_up_x1=cur_dst_down_x1;
            update_out_dst_up_x();
            return (cur_dst_down_x0<cur_dst_down_x1);
        }
        bool next_clip_line_down()
        {
            ++out_dst_down_y;
            if (!find_begin(out_dst_down_y,cur_dst_down_x0,cur_dst_down_x1)) return false;
            find_end(out_dst_down_y,cur_dst_down_x0,cur_dst_down_x1);
            update_out_dst_down_x();
            return (cur_dst_down_x0<cur_dst_down_x1);
        }
        bool next_clip_line_up()
        {
            --out_dst_up_y;
            if (!find_begin(out_dst_up_y,cur_dst_up_x0,cur_dst_up_x1)) return false;
            find_end(out_dst_up_y,cur_dst_up_x0,cur_dst_up_x1);
            update_out_dst_up_x();
            return (cur_dst_up_x0<cur_dst_up_x1);
        }
    };

must_inline void BilInear_Fast(const base_image& pic,const long x_16,const long y_16,Color32* result)
{
    Color32* PColor0=&pic.pixels(x_16>>16,y_16>>16);
    Color32* PColor1=(Color32*)((UInt8*)PColor0+pic.u32Stride);
    unsigned long u_8=(unsigned char)(x_16>>8);
    unsigned long v_8=(unsigned char)(y_16>>8);
    unsigned long pm3_8=(u_8*v_8)>>8;
    unsigned long pm2_8=u_8-pm3_8;
    unsigned long pm1_8=v_8-pm3_8;
    unsigned long pm0_8=256-pm1_8-pm2_8-pm3_8;

    unsigned long Color=*(unsigned long*)(PColor0);
    unsigned long BR=(Color & 0x00FF00FF)*pm0_8;
    unsigned long GA=((Color & 0xFF00FF00)>>8)*pm0_8;
                  Color=((unsigned long*)(PColor0))[1];
                  GA+=((Color & 0xFF00FF00)>>8)*pm2_8;
                  BR+=(Color & 0x00FF00FF)*pm2_8;
                  Color=*(unsigned long*)(PColor1);
                  GA+=((Color & 0xFF00FF00)>>8)*pm1_8;
                  BR+=(Color & 0x00FF00FF)*pm1_8;
                  Color=((unsigned long*)(PColor1))[1];
                  GA+=((Color & 0xFF00FF00)>>8)*pm3_8;
                  BR+=(Color & 0x00FF00FF)*pm3_8;

    *(unsigned long*)result=(GA & 0xFF00FF00)|((BR & 0xFF00FF00)>>8);
}




inline void BilInear_Border(const base_image& pic,const long x_16,const long y_16,Color32* result)
{
    unsigned long x0=(x_16>>16);
    unsigned long y0=(y_16>>16);

    Color32 pixel[4];
    pixel[0]=pic.getPixelsBorder(x0,y0);
    pixel[2]=pic.getPixelsBorder(x0,y0+1);
    pixel[1]=pic.getPixelsBorder(x0+1,y0);
    pixel[3]=pic.getPixelsBorder(x0+1,y0+1);

    base_image npic;
    npic.pSrcBuffer     =&pixel[0];
    npic.u32Width=2*sizeof(Color32);
    //npic.u32Width     =2;
    //npic.u32Height    =2;
    BilInear_Fast(npic,(unsigned short)x_16,(unsigned short)y_16,result);
}

must_inline Color32 AlphaBlend(Color32 dst,Color32 src)
{
    //AlphaBlend颜色混合公式(对其中的每个颜色分量分别处理)：
    // new_color=(dst_color*(255-src_color.alpha)+src_color*src_color.alpha)/255

    /*版本一：
    //提示：除法指令是很慢的操作，但vc2005可以把x/255编译为完全等价的"(x*M)>>N"类似的快速计算代码；
    unsigned long a=src.a;
    //if (a==0) return dst;
    //else if (a==255) return src;
    unsigned long ra=255-a;
    unsigned long result_b=(dst.b*ra+src.b*a)/255;
    unsigned long result_g=(dst.g*ra+src.g*a)/255;
    unsigned long result_r=(dst.r*ra+src.r*a)/255;
    unsigned long result_a=(dst.a*ra+a*a)/255;
    unsigned long result=(result_b) | (result_g<<8) | (result_r<<16) | (result_a<<24);
    return *(Color32*)&result;
    //*/

    /*版本二：
    //颜色处理中，也可以这样近似计算: x/255  =>  x>>8
    unsigned long a=src.a;
    unsigned long ra=255-a;
    unsigned long result_b=(dst.b*ra+src.b*a)>>8;
    unsigned long result_g=(dst.g*ra+src.g*a)>>8;
    unsigned long result_r=(dst.r*ra+src.r*a)>>8;
    unsigned long result_a=(dst.a*ra+a*a)>>8;
    unsigned long result=(result_b) | (result_g<<8) | (result_r<<16) | (result_a<<24);
    return *(Color32*)&result;
    //*/

    /*版本三：
    // (dst*(255-alpha)+src*alpha)>>8 近似为:(dst*(256-alpha)+src*alpha)>>8
    //   即 ((dst<<8)+(src-dst)*alpha)>>8  从而用一个移位代替一个乘法 (*256会被优化为移位)
    long a=src.a;
    unsigned long result_b=((unsigned long)(((long)dst.b)*256+((long)src.b-(long)dst.b)*a))>>8;
    unsigned long result_g=((unsigned long)(((long)dst.g)*256+((long)src.g-(long)dst.g)*a))>>8;
    unsigned long result_r=((unsigned long)(((long)dst.r)*256+((long)src.r-(long)dst.r)*a))>>8;
    unsigned long result_a=((unsigned long)(((long)dst.a)*256+((long)a-(long)dst.a)*a))>>8;
    unsigned long result=(result_b) | (result_g<<8) | (result_r<<16) | (result_a<<24);
    return *(Color32*)&result;
    //*/

    //*版本四：
    //同时运行两路颜色分量
    unsigned long Src_ARGB=*(unsigned long*)&src;
    unsigned long Dst_ARGB=*(unsigned long*)&dst;
    unsigned long a=Src_ARGB>>24;
    unsigned long ra=255-a;
    unsigned long result_RB=((Dst_ARGB & 0x00FF00FF)*ra + (Src_ARGB & 0x00FF00FF)*a);
    unsigned long result_AG=(((Dst_ARGB & 0xFF00FF00)>>8)*ra + ((Src_ARGB & 0xFF00FF00)>>8)*a);
    unsigned long result=((result_RB & 0xFF00FF00)>>8) | (result_AG & 0xFF00FF00);
    return *(Color32*)&result;
    //*/
}




void PicRotary_BilInear_CopyLine(Color32* pDstLine,long dst_border_x0,long dst_in_x0,long dst_in_x1,long dst_border_x1,
                        const base_image& SrcPic,long srcx0_16,long srcy0_16,long Ax_16,long Ay_16)
{
    long x;
    for (x=dst_border_x0;x<dst_in_x0;++x)
    {
        Color32 src_color;
        BilInear_Border(SrcPic,srcx0_16,srcy0_16,&src_color);
        pDstLine[x]=AlphaBlend(pDstLine[x],src_color);
        srcx0_16+=Ax_16;
        srcy0_16+=Ay_16;
    }
    for (x=dst_in_x0;x<dst_in_x1;++x)
    {
        BilInear_Fast(SrcPic,srcx0_16,srcy0_16,&pDstLine[x]);
        srcx0_16+=Ax_16;
        srcy0_16+=Ay_16;
    }
    for (x=dst_in_x1;x<dst_border_x1;++x)
    {
        Color32 src_color;
        BilInear_Border(SrcPic,srcx0_16,srcy0_16,&src_color);
        pDstLine[x]=AlphaBlend(pDstLine[x],src_color);
        srcx0_16+=Ax_16;
        srcy0_16+=Ay_16;
    }
}


    void PicRotaryBilInear(const base_image & Dst,const base_image& Src,double RotaryAngle,double ZoomX,double ZoomY,double move_x,double move_y)
    {
    	static double sZoomX=0;
    	static double sZoomY=0;
    	static int src_width=0;
    	static int src_height=0;
    	static double tmprZoomXY,rZoomX,rZoomY,sinA,cosA,rx0,ry0;
    	long Ax_16,Ay_16,Bx_16,By_16,Cx_16,Cy_16;
    	//	使用静态变量规避很多重复的计算
    	if(sZoomX!=ZoomX||sZoomY!=ZoomY||src_width!=Src.u32Width||src_height!=Src.u32Height){
    		sZoomX=ZoomX;
    		sZoomY=ZoomY;
    		src_width=Src.u32Width;
    		src_height=Src.u32Height;
			//if ( (fabs(ZoomX*Src.u32Width)<1.0e-4) || (fabs(ZoomY*Src.u32Height)<1.0e-4) ) return; //太小的缩放比例认为已经不可见
			 tmprZoomXY=1.0/(ZoomX*ZoomY);
			 rZoomX=tmprZoomXY*ZoomY;
			 rZoomY=tmprZoomXY*ZoomX;
			rx0=Src.u32Width*0.5;  //(rx0,ry0)为旋转中心
			ry0=Src.u32Height*0.5;
    	}
		SinCos(RotaryAngle,sinA,cosA);
		Ax_16=(long)(rZoomX*cosA*(1<<16));
		Ay_16=(long)(rZoomX*sinA*(1<<16));
		Bx_16=(long)(-rZoomY*sinA*(1<<16));
		By_16=(long)(rZoomY*cosA*(1<<16));
		Cx_16=(long)((-(rx0+move_x)*rZoomX*cosA+(ry0+move_y)*rZoomY*sinA+rx0)*(1<<16));
		Cy_16=(long)((-(rx0+move_x)*rZoomX*sinA-(ry0+move_y)*rZoomY*cosA+ry0)*(1<<16));

        TRotaryClipData rcData;
        rcData.Ax_16=Ax_16;
        rcData.Bx_16=Bx_16;
        rcData.Cx_16=Cx_16;
        rcData.Ay_16=Ay_16;
        rcData.By_16=By_16;
        rcData.Cy_16=Cy_16;
        rcData.dst_width=Dst.u32Width;
        rcData.dst_height=Dst.u32Height;
        rcData.src_width=Src.u32Width;
        rcData.src_height=Src.u32Height;
        if (!rcData.inti_clip(move_x,move_y,1)) return;

        Color32* pDstLine=(Color32*)Dst.pSrcBuffer;
        ((UInt8*&)pDstLine)+=(Dst.u32Stride*rcData.out_dst_down_y);
        while (true) //to down
        {
            long y=rcData.out_dst_down_y;
            if (y>=Dst.u32Height) break;
            if (y>=0)
            {
                PicRotary_BilInear_CopyLine(pDstLine,rcData.out_dst_x0_boder,rcData.out_dst_x0_in,
                    rcData.out_dst_x1_in,rcData.out_dst_x1_boder,Src,rcData.out_src_x0_16,rcData.out_src_y0_16,Ax_16,Ay_16);
            }
            if (!rcData.next_clip_line_down()) break;
            ((UInt8*&)pDstLine)+=Dst.u32Stride;
        }

        pDstLine=(Color32*)Dst.pSrcBuffer;
        ((UInt8*&)pDstLine)+=(Dst.u32Stride*rcData.out_dst_up_y);
        while (rcData.next_clip_line_up()) //to up
        {
            long y=rcData.out_dst_up_y;
            if (y<0) break;
            ((UInt8*&)pDstLine)-=Dst.u32Stride;
            if (y<Dst.u32Height)
            {
                PicRotary_BilInear_CopyLine(pDstLine,rcData.out_dst_x0_boder,rcData.out_dst_x0_in,
                    rcData.out_dst_x1_in,rcData.out_dst_x1_boder,Src,rcData.out_src_x0_16,rcData.out_src_y0_16,Ax_16,Ay_16);
            }
        }
    }



