#ifndef __STATIC_TEXT_H__
#define __STATIC_TEXT_H__

#include "XMLInstal.h"
//#include "layer.h"
#include "view.h"
#include "ttf_font.h"
//#include "alpha.h"

class static_text: public BaseView
{
public:


	static_text()
	{
		color = 0;
//		bkcolor=0;
		font = NULL;
		style = FONT_NORMAL;
		size = 16;
		hide = 0;
		lenth = 0;
		align_center=0;
		padding_left=0;
		padding_top=0;

	}
	~static_text()
	{
	}

	void test(const char * abc)
	{
	}
	void doDelete()
	{

	}
	void doGetInfo(info & info)
	{
		GetElementInfo(info);
		info.AddInfo("txt", txt);
		info.AddInfo("size", size);
		info.AddInfo("font", font);
	}
	void doFlushConfig()
	{
		PraseElement();

		if(hide==0)
		{

			if (m_mp.exist("color"))  //新接口
			 color=m_mp["color"]->getvalue_hex();
			else{  //兼容老方式
				int red = m_mp["red"]->getvalue_int();
				int green = m_mp["green"]->getvalue_int();
				int blue = m_mp["blue"]->getvalue_int();
				color = (red & 0xff) << 16 | (green & 0xff) << 8 | blue & 0xff;
			}
			if(m_mp.exist("bg_color")){
				unsigned int bg_color=m_mp["bg_color"]->getvalue_hex();
				ttf.SetBuffer(width, height,bg_color);
			}else{
				ttf.SetBuffer(width, height);
			}
			int padding_left=m_mp["padding_left"]->getvalue_int();
			int padding_top=m_mp["padding_top"]->getvalue_int();
			if (m_mp.exist("align_center"))
			align_center=m_mp["align_center"]->getvalue_int();

			font = m_mp["font"]->getvalue();

			txt = m_mp["txt"]->getvalue();
/*********************
 * style:
*#define   FONT_NORMAL    0
*#define   FONT_ITALIC    1
*#define   FONT_BOLD      2
 */
			style = (unsigned char) m_mp["style"]->getvalue_int();
			size = m_mp["size"]->getvalue_int();

			ttf.m_font = &font_mp[font];
			//log_i("get font_mp %x %x\r\n", font_mp[font].face,font_mp[font].ft_Lib);
			ttf.fontHeight = size;
			ttf.color = color;
			ttf.style = style;
			log_i("bkcolor=%d!!!!!!!!!!!!\n",bk_color);
			/*
			if(align_center)
			x-=(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
			ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length());
			*/
			if(align_center){
				padding_left+=width/2-(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
				padding_left>0?padding_left:0;
				padding_top+=(height-size)/2;
				padding_top>0?padding_top:0;
				}

			ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),padding_left,padding_top);
	}
		if(!m_mp["cached"]->getvalue_int()){//如果存在cached标志就不绘制，等待下一次动态参数
			Flush();
		}
	}

//	void doFlushConfigReduced()
//	{
//		hide = m_mp["hide"]->getvalue_int();
//		ttf.SetBuffer(width, height);
//		if(hide==0)
//		{
//			//id = m_mp["id"]->getvalue_int();
//			int red = m_mp["red"]->getvalue_int();
//			int green = m_mp["green"]->getvalue_int();
//			int blue = m_mp["blue"]->getvalue_int();
//			 padding_left=m_mp["padding_left"]->getvalue_int();
//			 padding_top=m_mp["padding_top"]->getvalue_int();
//			if (m_mp.exist("align_center"))
//			align_center=m_mp["align_center"]->getvalue_int();
//			color = (red & 0xff) << 16 | (green & 0xff) << 8 | blue & 0xff;
//			txt = m_mp["txt"]->getvalue();
//			ttf.color = color;
//			int pl=padding_left;
//			int pt=padding_top;
//			if(align_center){
//				pl+=width/2-(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
//				pl>0?pl:0;
//				pt+=(height-size)/2;
//				pt>0?pt:0;
//				}
//			ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),pl,pt);
//	}
//		Flush();
//	}
	void doRenderConfig()
	{
		//image::Render(&ttf, 0, 0);
		render_res[0].img=&ttf;
		//	image::Render(&img, 0, 0, width, height, 0, 0);
	}
	void setText(const char * t){
		int pl=padding_left;
		int pt=padding_top;
		txt =t;
		ttf.cleanBuf();
		if(align_center){
			pl+=width/2-(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
			pl>0?pl:0;
			pt+=(height-size)/2;
			pt>0?pt:0;
			}
		ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),pl,pt);
		Flush();
	}
	void setText(const char * t,unsigned int color){
		int pl=padding_left;
		int pt=padding_top;
		txt =t;
		ttf.color = color;
		ttf.cleanBuf();
		if(align_center){
			pl+=width/2-(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
			pl>0?pl:0;
			pt+=(height-size)/2;
			pt>0?pt:0;
			}
		ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),pl,pt);
		Flush();
	}
	void setColor(unsigned int color){
		ttf.color = color;
		ttf.cleanBuf();
		int pl=padding_left;
		int pt=padding_top;
		if(align_center){
			pl+=width/2-(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
			pl>0?pl:0;
			pt+=(height-size)/2;
			pt>0?pt:0;
			}
		ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),pl,pt);
		Flush();
	}
	void setSize( int size){
		int pl=padding_left;
		int pt=padding_top;
		ttf.fontHeight = size;
		if(align_center){
			pl+=width/2-(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
			pl>0?pl:0;
			pt+=(height-size)/2;
			pt>0?pt:0;
			}
		ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),pl,pt);
		Flush();
	}
	text ttf;
	map<hustr, text>::iterator font_it;
	unsigned int color;
	unsigned int bk_color;
	hustr font;	//保存路径
	hustr txt;	//

	unsigned char style;
	int size;
	int lenth;
	int buf_len;
	int align_center;
	int padding_left;
	int padding_top;
};

#endif //__STATIC_IMAGE_H__
