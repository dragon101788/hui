#ifndef __SCREEN_LOCK__
#define __SCREEN_LOCK__
#include "XMLInstal.h"
#include "view.h"
#include"widget/static_image/static_image.h"

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
			if(mx>0&&slider_x+mx+slider_width<dst_right){
				offset_x=(slider_x+mx);
				slider_abs_x=offset_x+abs_x;
				if(slider_x+mx+slider_width>dst_left){//已经达到解锁区域
					for (int i = 0; i <node_num; i++)
					{
						if(nodemp[i]->id!=1)
						nodemp[i]->setImageID(1);
					}
				}else{
					for (int i = 0; i <node_num; i++)
					{
						if(nodemp[i]->id!=0)
						nodemp[i]->setImageID(0);
					}
				}
				Flush();
			}
		}

	}

	void doTouchUp()
	{
//判断是否达到目的地，否则滑块返回
		if(offset_x+slider_width<dst_left){
			checked=0;
			offset_x=slider_x;
			slider_abs_x=slider_x+abs_x;
			Flush();
		}else{
			checked=1;
			if(listener!=NULL){
				listener->onUnlock();
			}
			//复原参数防止重复触发
			slider_abs_x=slider_x+abs_x;
			offset_x=slider_x;
			for (int i = 0; i <node_num; i++)
			{
				if(nodemp[i]->id!=0)
				nodemp[i]->setImageID(0);
			}
			log_i("arrived!!!!!!\n");
		}
	}

	void doTouchActive()
	{
	//	log_i("doTouchActive!!!!!!\n");
	//	xml_mgr->AddExec(0, exec);
	}
	void doDelete()
	{
		xml_mgr->DelTouchElement(this);
	}
	void doFlushConfig()
	{
		PraseElement();
		exec.parse(m_mp);
		slider_x= m_mp["slider_x"]->getvalue_int();  //滑块相对本元素的偏移
		slider_y= m_mp["slider_y"]->getvalue_int();
		dst_left=m_mp["dst_left"]->getvalue_int();  //触发解锁区域
		dst_right=m_mp["dst_right"]->getvalue_int();
		slider_abs_x=slider_x+abs_x;    //滑块绝对位置
		slider_abs_y=slider_y+abs_y;
		img.SetResource(m_mp["node"]->getvalue());  //滑块资源
		img.LoadResource();
		slider_width=img.GetWidth();
		slider_height=img.GetHeight();

		node_num=m_mp.count("state_image");
		for (int i = 0; i <node_num; i++)   //解锁状态图片
		{
			if (nodemp[i] == NULL)
			{
				nodemp[i] = new static_image;
				nodemp[i]->m_mp.fetch(m_mp["state_image"][i]);
				nodemp[i]->parent = this;
				nodemp[i]->xml_mgr = xml_mgr;
				nodemp[i]->mgr = mgr;
			}
			nodemp[i]->FlushConfig();
		}


		TouchParaseXml(m_mp);
		touch_init_area(abs_x,abs_y,width,height);

		xml_mgr->AddEleArea(this);
		Flush();

	}
	void doRender()
	{
		//image::Render(&img, move_x(), move_y());
		render_res[0].dst_x=offset_x;
		render_res[0].img=&img;

	}

	bool isSelect(){

	}
	void reset(){
		slider_abs_x=slider_x+abs_x;
	//	slider_abs_y=slider_y+abs_y;
		offset_x=slider_x;
		for (int i = 0; i <node_num; i++)
		{
			if(nodemp[i]->id!=0)
			nodemp[i]->setImageID(0);
		}
		Flush();
	}
	bool isChecked(){
		return checked;
	}

	 class UnlockedListener{
	 public:
		 virtual void onUnlock(){}
	 };

	 void setUnlockedListener(UnlockedListener *L){
		 listener=L;
	 }


		screen_lock(){
			mx=0;
			checked=0;
			listener=NULL;
		}
protected:
	UnlockedListener *listener;
	image img;
	ElementExec exec;
	map<int, static_image *> nodemp;
	int offset_x;
	int node_num;
	int slider_x;
	int slider_y;
	int slider_abs_x;
	int slider_abs_y;
	int slider_width;
	int slider_height;
	bool checked;
	int mx;
	int dst_left,dst_right;
};

#endif

