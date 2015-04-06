#ifndef  __TTF_FONT_H__
#define  __TTF_FONT_H__

#include "codec.h"
//#include "trueType/ft2build.h"
//#include "trueType/freetype.h"
//#include "trueType/ftoutln.h"
//#include "trueType/ftsynth.h"

#include "ft2build.h"
#include FT_FREETYPE_H
 #include FT_OUTLINE_H
 #include FT_SYNTHESIS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iconv.h>
#include <map>
#include "hulib.h"
using namespace std;



#define   FONT_NORMAL    0
#define   FONT_ITALIC    1
#define   FONT_BOLD      2
typedef unsigned short U16;

class text;


class FontDev: public Mutex
{ //FontDev字体绘制器 ,阻塞绘制元素，同BLT功能一般
public:
	FontDev()
	{

		face = NULL;
		ft_Lib = NULL;

	}
	~FontDev()
	{
		if (face != NULL)
			FT_Done_Face(face);
		if (ft_Lib != NULL)
			FT_Done_FreeType(ft_Lib);

	}
	int TTF_Init(const char * path, unsigned char bitmap)
	{
		FT_UInt glyph_index;
		int i;
		ft_error = FT_Init_FreeType(&ft_Lib);
		if (ft_error)
		{
			log_e("Error at init ft_Lib!\n");
			FT_Done_FreeType(ft_Lib);
			return -1;
		}

		ft_error = FT_New_Face(ft_Lib, path, 0, &face);
		if (ft_error)
		{
			log_e("New face error!\n");
			FT_Done_FreeType(ft_Lib);
			return -1;
		}

		TTF_bitmap_type = bitmap;

		matrix.xx = 1 << 16;
		matrix.xy = 0x5800;
		matrix.yx = 0;
		matrix.yy = 1 << 16;
		log_i("TTF_Init succssed!!!!!!!!!!!!!!!!!\n");
		// logMsg("init ok.\n");

		return 0;
	}
	int setPixelSize(int fontWidth, int fontHeight)
	{
		Font_H = fontHeight;
		ft_error = FT_Set_Pixel_Sizes(face, fontWidth, fontHeight);
		if (ft_error)
		{
			errexitf("Error at set fontSize %dx%d \n", fontWidth, fontHeight);
		}

	}
	int setStyle(unsigned char bitmap)
	{
		TTF_bitmap_type = bitmap;
		return 0;
	}
         ssize_t convert(const char *tocode, const char *fromcode, char *inbufp, size_t inbytesleft, char *outbufp,
                size_t outbytesleft);

	void DrawText(text * ptext, const char *encode, char * showtxt,int buff_width,int buff_height,
			unsigned int txt_len);
	void DrawText(text * ptext, const char *encode, char * showtxt,int buff_width,int buff_height,
				unsigned int txt_len,int padding_left,int padding_top);
	void DrawText(text * ptext, const char *encode, char * showtxt,int buff_width,
			int buff_height, unsigned int txt_len,int padding_left,int padding_top,float alphaStart,float alphaEnd);
	int TTF_DisplayAscii(text * ptext, const unsigned char *text, int num,
			unsigned int color, unsigned char style);
	int TTF_DisplayUnicode(text * ptext, const wchar_t *text, int num,
			unsigned int color, unsigned char style,int buff_width,int buff_height);
	int TTF_DisplayUnicode(text * ptext, const wchar_t *text, int num, unsigned int color, unsigned char style,
			int buff_width,int buff_height ,int padding_left,int padding_top);
	int TTF_DisplayUnicode(text * ptext, const wchar_t *text, int num, unsigned int color, unsigned char style,
			int buff_width,int buff_height ,int padding_left,int padding_top,float alphaStart,float alphaEnd);
	FT_Library ft_Lib;
	FT_Face face;
	FT_UInt glyph_index;
	FT_Vector pen;
	FT_Matrix matrix;
	FT_Error ft_error;
	unsigned char TTF_bitmap_type;
	unsigned int Font_H;
};
class text: public image
{
private:
	//void LCD_PutPixel(unsigned int x, unsigned int y, unsigned int color, unsigned char gray);
	//void ft_draw_bitmap(FT_Bitmap *bitmap, int x, int y, unsigned int color);

public:
	//以下几个变量需要外部赋值
	FontDev * m_font;
	
	unsigned int fontWidth;
	unsigned int fontHeight;
	unsigned int color;
	unsigned char style;
	//void *text_buffer;
	text()
	{
		fontWidth = 0;
		fontHeight = 16;
		color = 0;
		style = 0;
		m_font = NULL;
		log_i("truetype\n");
	}
	~text()
	{
		destroy();
	}
	void destroy()
	{
		//printf("destroy ttf\n");

	}
	void LCD_PutPixel(unsigned int x, unsigned int y, unsigned int color,
			unsigned char gray)
	{

		((Color32 *) pSrcBuffer + y * u32Width + x)->r = color >> 16
				& 0xff;
		((Color32 *) pSrcBuffer + y * u32Width + x)->g = color >> 8
				& 0xff;
		((Color32 *) pSrcBuffer + y * u32Width + x)->b = color
				& 0xff;
		((Color32 *) pSrcBuffer + y * u32Width + x)->a = gray;
	}
	void ft_draw_bitmap(FT_Bitmap *bitmap, int dst_x, int dst_y,unsigned int color);
	//字体渐渐隐藏显示
	void ft_draw_bitmap_fade(FT_Bitmap *bitmap, int dst_x, int dst_y,
				unsigned int color,float startAlpha,float endAlpha);
	//int added_txt(unsigned int w, unsigned int h, unsigned int color);
	//void free_text();
	//void Disp_txt(unsigned int * pSrcBuffer, unsigned int SrcBuffer_W, unsigned int x, unsigned int y, unsigned int w,unsigned int h);
	//int TTF_Init(const char* path, int fontWidth, int fontHeight, unsigned char bitmap);
	//int ft_set_style(int fontWidth, int fontHeight, unsigned char bitmap);
	//int TTF_DisplayAscii(int x, int y, const unsigned char *text, int num, unsigned int color, unsigned char style);
	//int TTF_DisplayUnicode(int x, int y, const wchar_t *text, int num, unsigned int color, unsigned char style);
	void DrawText(const char *encode, char * showtxt, unsigned int txt_len)
	{
		if (m_font != NULL)
		{
//			if (fontHeight > u32Height)
//			{
//				errexitf("DrawText fontHeight[%d] > u32Height[%d]\r\n",
//						fontHeight, u32Height);
//			}
			m_font->DrawText(this, encode, showtxt, u32Width,u32Height,txt_len);

		}
	}

	void DrawText(const char *encode, char * showtxt, unsigned int txt_len,int padding_left,int padding_top)
		{
			if (m_font != NULL)
			{
	//			if (fontHeight > u32Height)
	//			{
	//				errexitf("DrawText fontHeight[%d] > u32Height[%d]\r\n",
	//						fontHeight, u32Height);
	//			}
				m_font->DrawText(this, encode, showtxt, u32Width,u32Height,txt_len,padding_left,padding_top);

			}
		}
	void DrawText(const char *encode, char * showtxt, unsigned int txt_len,int padding_left,int padding_top,float alphaStart,float alphaEnd)
		{
			if (m_font != NULL)
			{
	//			if (fontHeight > u32Height)
	//			{
	//				errexitf("DrawText fontHeight[%d] > u32Height[%d]\r\n",
	//						fontHeight, u32Height);
	//			}
				m_font->DrawText(this, encode, showtxt, u32Width,u32Height,txt_len,padding_left,padding_top,alphaStart, alphaEnd);

			}
		}
	int  DrawText_unicode( wchar_t * showtxt, unsigned int txt_len)
	{
		if (m_font != NULL)
		{
//			if (fontHeight > u32Height)
//			{
//				errexitf("DrawText fontHeight[%d] > u32Height[%d]\r\n",
//						fontHeight, u32Height);
//			}
			return m_font->TTF_DisplayUnicode(this,showtxt,txt_len, this->color, this->style,u32Width,u32Height);

		}
	}
	int  DrawText_unicode( wchar_t * showtxt, unsigned int txt_len,int padding_left,int padding_top)
	{
		if (m_font != NULL)
		{
			return m_font->TTF_DisplayUnicode(this,showtxt,txt_len, this->color, this->style,u32Width,u32Height, padding_left,padding_top);
		}
	}
//format convert 
//eg:convert("UTF-8", "wchar_t", (char *)out_buf1, rc, out_buf2, out_len);

};

extern map<hustr, FontDev> font_mp;

#endif    /* __TTF_FUNC_H__ */
