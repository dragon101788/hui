#ifndef __ZOOM_IMAGE_H__
#define __ZOOM_IMAGE_H__

#include "XMLInstal.h"
#include "view.h"

class zoom_image: public View
{
public:
	zoom_image()
	{

		id = 0;
		zoom_x=10;
		zoom_y=10;
		center=0;
		fitWindow=0;
		fitWindow=0;
		smooth=0;
	}
	~zoom_image()
	{

	}

	void doDelete()
	{

	}


	void doGetInfo(info & info)
	{
		GetElementInfo(info);
		info.AddInfo("id", id);
		info.AddInfo("scroll_x", scroll_x);
		info.AddInfo("scroll_y", scroll_y);
	}
	void doFlushConfig()
	{
		PraseElement();
		id = m_mp["id"]->getvalue_int();
		scroll_x = m_mp["scroll_x"]->getvalue_int();
		scroll_y = m_mp["scroll_y"]->getvalue_int();
		smooth=m_mp["smooth"]->getvalue_int();
		fitWindow= m_mp["fitWindow"]->getvalue_int();
		if(fitWindow){
			zoom_x=width;
			zoom_y=height;
		}else{
			zoom_x = m_mp["zoom_x"]->getvalue_int();
			zoom_y = m_mp["zoom_y"]->getvalue_int();
		}
		center=m_mp["center"]->getvalue_int();
		if(center){
			scroll_x = (zoom_x-width)>>1;
			scroll_y = (zoom_y-height)>>1;
		}else{
			scroll_x = m_mp["scroll_x"]->getvalue_int();
			scroll_y = m_mp["scroll_y"]->getvalue_int();
		}
		for (int i = 0; i < m_mp.count("node"); i++)
		{
			log_i("doFlushConfig %d %s\r\n",i, m_mp["node"][i]->getvalue());
			SetRes(i, m_mp["node"][i]->getvalue());
		}
		ParseModifRes();
		res[id].LoadResource();
		zoom_img.SetBuffer(zoom_x,zoom_y);
		 if(smooth)
			 PicZoom_ftBilinear(zoom_img,res[id]);
		 else
			 PicZoom_no_Bilinear(zoom_img,res[id]);
		if(!m_mp["cached"]->getvalue_int()){//如果cached=1标志就不绘制，等待下一次动态参数调整后统一绘制
			Flush();
		}
	}
	void doFlushConfigReduced()
	{
		hide = m_mp["hide"]->getvalue_int();

		id = m_mp["id"]->getvalue_int();


		smooth=m_mp["smooth"]->getvalue_int();
		fitWindow= m_mp["fitWindow"]->getvalue_int();
		res[id].LoadResource();
		if(fitWindow){
			zoom_x=width;
			zoom_y=height;
		}else{
			zoom_x = m_mp["zoom_x"]->getvalue_int();
			zoom_y = m_mp["zoom_y"]->getvalue_int();
		}
		center=m_mp["center"]->getvalue_int();
		if(center){
			scroll_x = (zoom_x-width)>>1;
			scroll_y = (zoom_y-height)>>1;
		}else{
			scroll_x = m_mp["scroll_x"]->getvalue_int();
			scroll_y = m_mp["scroll_y"]->getvalue_int();
		}
		zoom_img.SetBuffer(zoom_x,zoom_y);
		 if(smooth)
			 PicZoom_ftBilinear(zoom_img,res[id]);
		 else
			 PicZoom_no_Bilinear(zoom_img,res[id]);
		Flush();
	}


	void doRender()
	{
		prender_res[0]=&zoom_img;

	}

	image zoom_img;
	int zoom_x,zoom_y;
	int id;
	int center;
	int fitWindow;
	int smooth;

};

#endif //__STATIC_IMAGE_H__
