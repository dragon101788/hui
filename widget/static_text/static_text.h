#ifndef __STATIC_IMAGE_H__
#define __STATIC_IMAGE_H__

#include "XMLInstal.h"
#include "layer.h"
#include "ttf_font.h"
#include "alpha.h"

class static_text: public element
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
		oldtxt=NULL;
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
			//id = m_mp["id"]->getvalue_int();
			int red = m_mp["red"]->getvalue_int();
			int green = m_mp["green"]->getvalue_int();
			int blue = m_mp["blue"]->getvalue_int();
			int padding_left=m_mp["padding_left"]->getvalue_int();
			int padding_top=m_mp["padding_top"]->getvalue_int();
			if (m_mp.exist("align_center"))
			align_center=m_mp["align_center"]->getvalue_int();
			color = (red & 0xff) << 16 | (green & 0xff) << 8 | blue & 0xff;
			//bkcolor = m_mp["bkcolor"]->getvalue_int();
			font = m_mp["font"]->getvalue();
			oldtxt=txt;
			txt = m_mp["txt"]->getvalue();
			style = (unsigned char) m_mp["style"]->getvalue_int();
			size = m_mp["size"]->getvalue_int();

			ttf.m_font = &font_mp[font];
			//printf("get font_mp %x %x\r\n", font_mp[font].face,font_mp[font].ft_Lib);
			ttf.fontHeight = size;
			ttf.color = color;
			ttf.style = style;
			ttf.SetBuffer(width, height);
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

		Flush();
	}
	void doRender()
	{
		image::Render(&ttf, 0, 0);
		//	image::Render(&img, 0, 0, width, height, 0, 0);
	}

	text ttf;
	map<hustr, text>::iterator font_it;
	unsigned int color;
	hustr font;	//保存路径
	hustr txt;	//
	hustr oldtxt;	//
	unsigned char style;
	int size;
	int lenth;
	int buf_len;
	int align_center;
};

#endif //__STATIC_IMAGE_H__
