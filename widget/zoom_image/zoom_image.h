#ifndef __ZOOM_IMAGE_H__
#define __ZOOM_IMAGE_H__

#include "XMLInstal.h"
#include "view.h"

class zoom_image: public BaseView
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

	}
	void doFlushConfig()
	{
		PraseElement();
		id = m_mp["id"]->getvalue_int();
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
			render_res[0].dst_x = (width-zoom_x)>>1;
			render_res[0].dst_y = (height-zoom_y)>>1;
		}else{
			render_res[0].dst_x = m_mp["offset_x"]->getvalue_int();
			render_res[0].dst_y = m_mp["offset_y"]->getvalue_int();
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
			 ImageTransform::zoom_bilinear(zoom_img,res[id]);
		 else
			 ImageTransform::zoom_no_bilinear(zoom_img,res[id]);
		if(!m_mp["cached"]->getvalue_int()){//如果cached=1标志就不绘制，等待下一次动态参数调整后统一绘制
			Flush();
		}
	}


	void doRenderConfig()
	{
		render_res[0].img=&zoom_img;

	}

	image zoom_img;
	int zoom_x,zoom_y;
	int id;
	int center;
	int fitWindow;
	int smooth;

};

#endif //__STATIC_IMAGE_H__
