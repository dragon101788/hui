#ifndef __TEXT_BUTTON_IMAGE_H__
#define __TEXT_BUTTON_IMAGE_H__

#include "XMLInstal.h"
#include "layer.h"
#include "thread_msg.h"
#include "alpha.h"
#include "ttf_font.h"
class text_button: public element, public touch_element
{
public:
	void doTouchDown()
	{
		//printf("$$$HU$$$ %s %s tx=%d ty=%d t%d b%d l%d r%d\r\n",name,__FUNCTION__,tx,ty,top,bottom,left,right);
		if (isdn == 1)
			return;

		//printf("%s touch\r\n", name.c_str());

		Flush();
	}

	void doTouchUp()
	{
		if (isdn == 0)
			return;

		//printf("%s free\r\n", name.c_str());

		Flush();
	}

	void doTouchActive()
	{
		xml_mgr->AddExec(0, exec);
	}

	void doDelete()
	{
		xml_mgr->DelTouchElement(this);
	}

	void doRender()
		{
			image::Render(&res[isdn], 0, 0);
			Render_img_to_img(this, &ttf, 0, 0, width, height,0,0);//将文字复制到图片上
		}



	void doFlushConfig()
	{
		PraseElement();
		exec.parse(m_mp);
		TouchParaseXml(m_mp);
		touch_init_area(x, y, width, height);

		SetRes(0, m_mp["up"]->getvalue());
		SetRes(1, m_mp["dn"]->getvalue());




		xml_mgr->AddEleArea(this);
		setTextRes();//设置文字相关属性
		ParseModifRes();
		//printf("Render_img_to_img!!!!\n");
		//Render_img_to_img(&res[0], &ttf, 0, 0, width, height,0,0);//将文字复制到图片上
		//Render_img_to_img(&res[1], &ttf, 0, 0, width, height,0,0);//将文字复制到图片上

		Flush();
	}

	void doFlushConfigReduced()
	{

		exec.parse(m_mp);
		SetRes(0, m_mp["up"]->getvalue());
		SetRes(1, m_mp["dn"]->getvalue());

		setTextRes();//设置文字相关属性


		Flush();
	}
	void setTextRes(){


			unsigned int color;
			hustr font;	//保存路径
			hustr txt;	//保存路径
			unsigned char style;
			int size;
			int lenth;
			int buf_len;
			int align_center=0;
			int red = m_mp["red"]->getvalue_int();
			int green = m_mp["green"]->getvalue_int();
			int blue = m_mp["blue"]->getvalue_int();
			int padding_left=m_mp["padding_left"]->getvalue_int();
			int padding_top=m_mp["padding_top"]->getvalue_int();
			if (m_mp.exist("align_center"))
			 align_center=m_mp["align_center"]->getvalue_int();
			color = (red & 0xff) << 16 | (green & 0xff) << 8 | blue & 0xff;
			font = m_mp["font"]->getvalue();
			txt = m_mp["txt"]->getvalue();
			style = (unsigned char) m_mp["style"]->getvalue_int();
			size = m_mp["size"]->getvalue_int();
			ttf.m_font = &font_mp[font];
			ttf.fontHeight = size;
			ttf.color = color;
			ttf.style = style;
			ttf.SetBuffer(width, height);
			if(align_center){
				padding_left+=width/2-(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
				padding_top+=(height-size)/2;
			}
				ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),padding_left,padding_top);


	}



	text_button()
		{


		}


	map<hustr, text>::iterator font_it;
	HuExec exec;
	//hustr cmd;
	text ttf;





};

#endif //__STATIC_IMAGE_H__
