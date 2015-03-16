#ifndef __STATIC_IMAGE_H__
#define __STATIC_IMAGE_H__

#include "XMLInstal.h"
#include "layer.h"

class static_image: public element
{
public:
	static_image()
	{
		xpos = 0;
		ypos = 0;
		id = 0;
	}
	~static_image()
	{

	}
	void doDelete()
	{

	}
	void doGetInfo(info & info)
	{
		GetElementInfo(info);
		info.AddInfo("id", id);
		info.AddInfo("xpos", xpos);
		info.AddInfo("ypos", ypos);
	}
	void doFlushConfig()
	{
		PraseElement();
		id = m_mp["id"]->getvalue_int();
		xpos = m_mp["xpos"]->getvalue_int();
		ypos = m_mp["ypos"]->getvalue_int();

		for (int i = 0; i < m_mp.count("node"); i++)
		{
			printf("doFlushConfig %d %s\r\n",i, m_mp["node"][i]->getvalue());
			SetRes(i, m_mp["node"][i]->getvalue());
		}
		ParseModifRes();
		if(!m_mp["cached"]->getvalue_int()){//如果cached=1标志就不绘制，等待下一次动态参数调整后统一绘制
			Flush();
		}
	}
	void doFlushConfigReduced()
	{
		hide = m_mp["hide"]->getvalue_int();
		xpos = m_mp["xpos"]->getvalue_int();
		ypos = m_mp["ypos"]->getvalue_int();
		id = m_mp["id"]->getvalue_int();

		Flush();
	}


	void doRender()
	{
		//image::Render(&res[id], xpos, ypos, width, height, 0, 0);
		res[id].LoadResource();
		cur_res=&res[id];
	}
	int xpos;
	int ypos;

	int id;
};

#endif //__STATIC_IMAGE_H__
