#ifndef __SIMPLE_FRAGMENT_H__
#define __SIMPLE_FRAGMENT_H__

#include "XMLInstal.h"
#include "view.h"



class simple_fragment: public BaseView
{

public:

	void doRenderConfig()
	{
		if(back_color)
			render_res[0].img=&bg;
	}
	void doFlushConfig()
	{
		PraseElement();
		if (m_mp.exist("back_color"))//在父元素的第几个页面里,0开始算起
		{
			back_color= m_mp["back_color"]->getvalue_hex();
			bg.SetBuffer(width,height,back_color);
		}

		Flush();
	}

//	void doFlushConfigReduced()
//	{
//		rePraseElement();
//		Flush();
//	}
	simple_fragment()
	{
		back_color=0;
	}
	~simple_fragment()
	{

	}
	void doDelete()
	{

	}
	int back_color;
	image bg;
};







#endif //__STATIC_IMAGE_H__
