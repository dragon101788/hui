
#include <math.h>

#include "codec.h"



#ifdef _MSC_VER     
    #define must_inline __forceinline 
    #ifndef _WIN64
        //��������x86���  x64ʱvc��������֧��������� ����û��MMX�Ĵ���
        #define asm __asm
        //ʹ��MMX
        #define MMX_ACTIVE
    #endif
    typedef    __int64                Int64;
    typedef    unsigned __int64    UInt64;
#else
    #ifdef __GNUC__
        #define must_inline __attribute__((always_inline)) 
    #else
        #define must_inline inline 
    #endif
    typedef    long long            Int64;
    typedef    unsigned long long    UInt64;
#endif 




//////////////////////////////////////////////////////////////////////////////////////////////////////



void PicZoom_no_Bilinear(const BaseImage& Dst,const BaseImage& Src)
{
    if (  (0==Dst.u32Width)||(0==Dst.u32Height)
        ||(0==Src.u32Width)||(0==Src.u32Height)) return;

    long xrIntFloat_16=(Src.u32Width<<16)/Dst.u32Width+1;
    long yrIntFloat_16=(Src.u32Height<<16)/Dst.u32Height+1;

    long dst_width=Dst.u32Width;
    Color32* pDstLine=(Color32*)Dst.pSrcBuffer;
    long srcy_16=0;
    for (long y=0;y<Dst.u32Height;++y)
    {
        Color32* pSrcLine=Src.getLinePixels(srcy_16>>16);
        long srcx_16=0;
        for (long x=0;x<dst_width;++x)
        {
            pDstLine[x]=pSrcLine[srcx_16>>16];
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((UInt8*&)pDstLine)+=Dst.u32Stride;
    }
}



void CreateZoomTable_OpMul(const long SrcLength,const long DstLength,long* ZoomTable)
{
    long delta_src=SrcLength;
    long delta_dst=DstLength;

    long inc_src,inc_dst;
    if (delta_src>0) 
        inc_src=1; 
    else if (delta_src<0) {
        delta_src=-delta_src; 
        inc_src=-1; 
    } else  
        inc_src=0;
    if (delta_dst>0) 
        inc_dst=1; 
    else if (delta_dst<0) {
        delta_dst=-delta_dst; 
        inc_dst=-1; 
    }else
        inc_dst=0;

    long MaxLength;
    if (delta_src>=delta_dst)
        MaxLength=delta_src; 
    else 
        MaxLength=delta_dst;

    long SrcPos=0;
    long DstPos=0;
    long mSrcPos=0;
    long mDstPos=0;
    for (long i=0;i<MaxLength;++i) {
        ZoomTable[SrcPos]=DstPos;
        mSrcPos+=delta_src;
        if (mSrcPos>=MaxLength) { 
            mSrcPos-=MaxLength; 
            DstPos+=inc_dst; 
        }
        mDstPos+=delta_dst;
        if (mDstPos>=MaxLength) { 
            mDstPos-=MaxLength; 
            SrcPos+=inc_src;
        }
    }
}

//����汾�ڳ����ͳ˷��Ƚϰ����CPU�Ͽ���Ч������ ����ʵ���ԼӸĶ��������������κγ˳���ָ��
void PicZoom3_Table_OpMul(const BaseImage& Dst,const BaseImage& Src)
{
    if (  (0==Dst.u32Width)||(0==Dst.u32Height)
        ||(0==Src.u32Width)||(0==Src.u32Height)) return;

    long dst_width=Dst.u32Width;

    long* SrcX_Table = new long[dst_width];
    CreateZoomTable_OpMul(Src.u32Width,Dst.u32Width,SrcX_Table);//���ɱ� SrcX_Table
    long* SrcY_Table = new long[Dst.u32Height];
    CreateZoomTable_OpMul(Src.u32Height,Dst.u32Height,SrcY_Table);//���ɱ� SrcX_Table

    Color32* pDstLine=(Color32*)Dst.pSrcBuffer;
    for (long y=0;y<Dst.u32Height;++y)
    {
        long srcy=SrcY_Table[y];
        Color32* pSrcLine=Src.getLinePixels(srcy);//��Щ�Ķ�Ҳ����ȥ������������һ���˷�
        for (long x=0;x<dst_width;++x)
            pDstLine[x]=pSrcLine[SrcX_Table[x]];
        ((UInt8*&)pDstLine)+=Dst.u32Stride;
    }

    delete [] SrcY_Table;
    delete [] SrcX_Table;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




    must_inline void Bilinear_Fast_Common(Color32* PColor0,Color32* PColor1,unsigned long u_8,unsigned long v_8,Color32* result)
    {
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

        *(unsigned long*)(result)=(GA & 0xFF00FF00)|((BR & 0xFF00FF00)>>8);
    }

    inline void Bilinear_Border_Common(const BaseImage& pic,const long x_16,const long y_16,Color32* result)
    {
        long x=(x_16>>16);
        long y=(y_16>>16);
        unsigned long u_16=((unsigned short)(x_16));
        unsigned long v_16=((unsigned short)(y_16));

        Color32 pixel[4];
        pixel[0]=pic.getPixelsBorder(x,y);
        pixel[1]=pic.getPixelsBorder(x+1,y);
        pixel[2]=pic.getPixelsBorder(x,y+1);
        pixel[3]=pic.getPixelsBorder(x+1,y+1);
        
        Bilinear_Fast_Common(&pixel[0],&pixel[2],u_16>>8,v_16>>8,result);
    }

void PicZoom_Bilinear(const BaseImage& Dst,const BaseImage& Src)
{
    if (  (0==Dst.u32Width)||(0==Dst.u32Height)
        ||(0==Src.u32Width)||(0==Src.u32Height)) return;

    long xrIntFloat_16=((Src.u32Width)<<16)/Dst.u32Width+1;
    long yrIntFloat_16=((Src.u32Height)<<16)/Dst.u32Height+1;
    const long csDErrorX=-(1<<15)+(xrIntFloat_16>>1);
    const long csDErrorY=-(1<<15)+(yrIntFloat_16>>1);

    long dst_width=Dst.u32Width;

    //�������Ҫ���⴦��ı߽�
    long border_y0=-csDErrorY/yrIntFloat_16+1;              //y0+y*yr>=0; y0=csDErrorY => y>=-csDErrorY/yr
    if (border_y0>=Dst.u32Height) border_y0=Dst.u32Height;
    long border_x0=-csDErrorX/xrIntFloat_16+1;     
    if (border_x0>=Dst.u32Width ) border_x0=Dst.u32Width;
    long border_y1=(((Src.u32Height-2)<<16)-csDErrorY)/yrIntFloat_16+1; //y0+y*yr<=(height-2) => y<=(height-2-csDErrorY)/yr
    if (border_y1<border_y0) border_y1=border_y0;
    long border_x1=(((Src.u32Width-2)<<16)-csDErrorX)/xrIntFloat_16+1;
    if (border_x1<border_x0) border_x1=border_x0;

    Color32* pDstLine=(Color32*)Dst.pSrcBuffer;
    long Src_byte_width=Src.u32Stride;
    long srcy_16=csDErrorY;
    long y;
    for (y=0;y<border_y0;++y)
    {
        long srcx_16=csDErrorX;
        for (long x=0;x<dst_width;++x)
        {
            Bilinear_Border_Common(Src,srcx_16,srcy_16,&pDstLine[x]); //border
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((UInt8*&)pDstLine)+=Dst.u32Stride;
    }
    for (y=border_y0;y<border_y1;++y)
    {
        long srcx_16=csDErrorX;
        long x;
        for (x=0;x<border_x0;++x)
        {
            Bilinear_Border_Common(Src,srcx_16,srcy_16,&pDstLine[x]);//border
            srcx_16+=xrIntFloat_16;
        }

        {
            unsigned long v_8=(srcy_16 & 0xFFFF)>>8;
            Color32* PSrcLineColor= (Color32*)((UInt8*)(Src.pSrcBuffer)+Src_byte_width*(srcy_16>>16)) ;
            for (long x=border_x0;x<border_x1;++x)
            {
                Color32* PColor0=&PSrcLineColor[srcx_16>>16];
                Color32* PColor1=(Color32*)((UInt8*)(PColor0)+Src_byte_width);
                Bilinear_Fast_Common(PColor0,PColor1,(srcx_16 & 0xFFFF)>>8,v_8,&pDstLine[x]);
                srcx_16+=xrIntFloat_16;
            }
        }

        for (x=border_x1;x<dst_width;++x)
        {
            Bilinear_Border_Common(Src,srcx_16,srcy_16,&pDstLine[x]);//border
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((UInt8*&)pDstLine)+=Dst.u32Stride;
    }
    for (y=border_y1;y<Dst.u32Height;++y)
    {
        long srcx_16=csDErrorX;
        for (long x=0;x<dst_width;++x)
        {
            Bilinear_Border_Common(Src,srcx_16,srcy_16,&pDstLine[x]); //border
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((UInt8*&)pDstLine)+=Dst.u32Stride;
    }
}


//===============================================================================================================================

void PicZoom_ftBilinear(const BaseImage& Dst,const BaseImage& Src)
{
    if (  (0==Dst.u32Width)||(0==Dst.u32Height)
        ||(2>Src.u32Width)||(2>Src.u32Height)) return;

    long xrIntFloat_16=((Src.u32Width-1)<<16)/Dst.u32Width;
    long yrIntFloat_16=((Src.u32Height-1)<<16)/Dst.u32Height;

    long dst_width=Dst.u32Width;
    long Src_byte_width=Src.u32Stride;
    Color32* pDstLine=(Color32*)Dst.pSrcBuffer;
    long srcy_16=0;
    for (long y=0;y<Dst.u32Height;++y)
    {
        unsigned long v_8=(srcy_16 & 0xFFFF)>>8;
        Color32* PSrcLineColor= (Color32*)((UInt8*)(Src.pSrcBuffer)+Src_byte_width*(srcy_16>>16)) ;
        long srcx_16=0;
        for (long x=0;x<dst_width;++x)
        {
            Color32* PColor0=&PSrcLineColor[srcx_16>>16];
            Bilinear_Fast_Common(PColor0,(Color32*)((UInt8*)(PColor0)+Src_byte_width),(srcx_16 & 0xFFFF)>>8,v_8,&pDstLine[x]);
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
        ((UInt8*&)pDstLine)+=Dst.u32Stride;
    }
}

