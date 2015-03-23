#ifndef __SIMPLE_FRAGMENT_H__
#define __SIMPLE_FRAGMENT_H__

#include "XMLInstal.h"
#include "view.h"



class simple_fragment: public BaseView
{

public:

	void doRender()
	{

	}
	void doFlushConfig()
	{
		PraseElement();
		Flush();
	}

	void doFlushConfigReduced()
	{
		rePraseElement();
		Flush();
	}
	simple_fragment()
	{
	}
	~simple_fragment()
	{

	}
	void doDelete()
	{

	}

};







#endif //__STATIC_IMAGE_H__
