#ifndef __BUTTON_IMAGE_H__
#define __BUTTON_IMAGE_H__

#include "XMLInstal.h"
//#include "layer.h"
#include "view.h"
#include "thread_msg.h"

class button: public TouchView
{
public:
	void doTouchDown()
	{
		//printf("$$$HU$$$ %s %s tx=%d ty=%d t%d b%d l%d r%d\r\n",name,__FUNCTION__,tx,ty,top,bottom,left,right);
		if (isdn == 1)
			return;
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
	void doFlushConfig()
	{
		PraseElement();
		exec.parse(m_mp);
		TouchParaseXml(m_mp);
		alpha_mode=m_mp["alpha_mode"]->getvalue_int();
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
		Flush();
	}
	void doRenderConfig()
	{
		//image::Render(&res[isdn], 0, 0)

		if(alpha_mode){
			res[0].LoadResource();
			render_res[0].img=&res[0];
			if(isdn){
			res[1].LoadResource();
			render_res[1].img=&res[1];
			}
			else
			render_res.erase(1);
		}
		else{
		res[isdn].LoadResource();
		render_res[0].img=&res[isdn];
		}
	}
	void onAbsPosChanged(){
		touch_init_area(abs_x, abs_y, width, height);
	}
	button()
	{
		alpha_mode=0;
	}
	int alpha_mode;
	ElementExec exec;
	//hustr cmd;
};

#endif //__STATIC_IMAGE_H__
