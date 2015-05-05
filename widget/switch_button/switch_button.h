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
		if(select_old)//双击取消
		select=0;
		else
		select=1;
		select_old=select;
		Flush();
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

		if (m_mp.exist("up"))
			SetRes(0, m_mp["up"]->getvalue());
		else if(m_mp.exist("up_color")){
			unsigned int up_color=m_mp["up_color"]->getvalue_hex();
			SetRes(0, up_color);
		}else{
			SetRes(0);
		}
		if (m_mp.exist("dn"))
			SetRes(1, m_mp["dn"]->getvalue());
		else if(m_mp.exist("dn_color")){
			unsigned int dn_color=m_mp["dn_color"]->getvalue_hex();
			SetRes(1, dn_color);
		}else{
			SetRes(1);
		}

		xml_mgr->AddEleArea(this);

		ParseModifRes();
		res[0].LoadResource();
		res[1].LoadResource();
		Flush();
	}
	void doRenderConfig()
	{

		render_res[0].img=&res[select];

	}
	void onAbsPosChanged(){
		touch_init_area(abs_x, abs_y, width, height);
	}
	switch_button()
	{
		select=0;
		select_old=0;
	}
	bool isSelect() const{
		return select;
	}
	void  setSelect(int sel) {
		if(select==sel){
			return ;
		}
		select=sel;
		select_old=sel;
		 Flush();
	}
	int select,select_old;
	ElementExec exec;
	//hustr cmd;
};

#endif //__STATIC_IMAGE_H__
