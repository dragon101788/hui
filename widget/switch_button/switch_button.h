#ifndef __SWITCH_BUTTON_H__
#define __SWITCH_BUTTON_H__

#include "XMLInstal.h"
#include "view.h"

class switch_button: public TouchView
{
public:
	void doTouchDown()
	{
		//printf("$$$HU$$$ %s %s tx=%d ty=%d t%d b%d l%d r%d\r\n",name,__FUNCTION__,tx,ty,top,bottom,left,right);
		if (isdn == 1)
			return;
		//printf("%s touch\r\n", name.c_str());
		if(select_old)//双击取消
		select=0;
		else
		select=1;
		select_old=select;
		Flush();
	}

	void doTouchUp()
	{
		if (isdn == 0)
			return;
		//printf("%s free\r\n", name.c_str());


		//Flush();
	}

	void doTouchActive()
	{
		xml_mgr->AddExec(0, exec);
	}

	void doDelete()
	{
		xml_mgr->DelTouchElement(this);
	}
	void doFlushConfig()
	{
		PraseElement();
		exec.parse(m_mp);
		TouchParaseXml(m_mp);
		touch_init_area(abs_x, abs_y, width, height);

		SetRes(0, m_mp["up"]->getvalue());
		SetRes(1, m_mp["dn"]->getvalue());

		xml_mgr->AddEleArea(this);

		ParseModifRes();
		res[0].LoadResource();
		res[1].LoadResource();
		Flush();
	}
	void doRender()
	{

		prender_res[0]=&res[select];

	}
	void onAbsPosChanged(){
		touch_init_area(abs_x, abs_y, width, height);
	}
	switch_button()
	{
		select,select_old;
	}
	int getSelect() const{
		return select;
	}

	int select,select_old;
	HuExec exec;
	//hustr cmd;
};

#endif //__STATIC_IMAGE_H__
