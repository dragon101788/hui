#ifndef __SCREEN_LOCK__
#define __SCREEN_LOCK__
#include "XMLInstal.h"
#include "view.h"


class screen_lock: public TouchView
{
public:


	void doTouchDown()
	{
		//printf("tx=%d ty=%d\r\n",tx,ty);
		//在滑块区域内
		if (GetTouchX() > slider_abs_x && GetTouchX() < slider_abs_x+slider_width&& GetTouchY() > slider_abs_y&& GetTouchY() < slider_abs_y+slider_height)
		{


			mx=move_x();
			//if(mx<0){

			if(mx>0&&slider_x+mx<dst_x+slider_width){
			scroll_x=-(slider_x+mx);
			slider_abs_x=-scroll_x+abs_x;
			Flush();
			}
		}

	}

	void doTouchUp()
	{
//判断是否达到目的地，否则滑块返回
		if(scroll_x>-dst_x){
			scroll_x=-slider_x;
			slider_abs_x=slider_x+abs_x;
			Flush();
		}else{
			onTouchActive();
			//复原参数防止重复触发
			slider_abs_x=slider_x+abs_x;
			scroll_x=-slider_x;
			log_i("arrived!!!!!!\n");
		}
	}

	void doTouchActive()
	{
		log_i("doTouchActive!!!!!!\n");
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
		slider_x= m_mp["slider_x"]->getvalue_int();
		slider_y= m_mp["slider_y"]->getvalue_int();
		dst_x=m_mp["dst_x"]->getvalue_int();
		slider_abs_x=slider_x+abs_x;
		slider_abs_y=slider_y+abs_y;
		img.SetResource(m_mp["node"]->getvalue());
		img.LoadResource();
		slider_width=img.GetWidth();
		slider_height=img.GetHeight();
		TouchParaseXml(m_mp);
		touch_init_area(abs_x,abs_y,width,height);

		xml_mgr->AddEleArea(this);
		Flush();

	}
	void doRender()
	{
		//image::Render(&img, move_x(), move_y());
		prender_res[0]=&img;

	}
	screen_lock(){
		select=0;
		mx=0;
	}
	bool isSelect(){

	}
	void reset(){
		slider_abs_x=slider_x+abs_x;
	//	slider_abs_y=slider_y+abs_y;
		scroll_x=-slider_x;
		Flush();
	}
	image img;
	HuExec exec;
	int select; //滑块是否选中
	int slider_x;
	int slider_abs_x;
	int slider_abs_y;
	int slider_y;
	int slider_width;
	int slider_height;
	int mx;
	int dst_x;
};

#endif

