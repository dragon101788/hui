#ifndef __VIEW_H__
#define __VIEW_H__

#include"layer.h"
#include"screenHandler.h"




class BaseView:public element{
	public:
	BaseView(){

	}
	~BaseView(){

	}

	virtual void setHide(int hide){
		this->hide=hide;
		log_i("%s set hide=%d!!\n",name.c_str(),hide);
		if(isParent()){
			iterator it;
			BaseView *ele;
			for (it = elem.begin(); it != elem.end(); ++it)
			{
				 ele=(BaseView *)it->second;
				if(ele!=NULL)
					ele->onParentHideChanged(hide);
			}
		}
		Flush();
	}
	virtual void onParentHideChanged(int hide){
	}


	void setXY(int x,int y){
		if(this->x!=x||this->y!=y){
			cleanLastPos();
		}
		this->x=x;
		this->y=y;
		Flush();
	}

};
class TimerView:public BaseView, public timer_element{
	public:
	TimerView(){
		listener=NULL;
	}
	~TimerView(){

	}
	void setTouchEventListener(EventListener *L){
		listener=L;
	}

	 int onTimer(int tm){
		 doTimer(tm);
		if(listener!=NULL)
			listener->onTimer(name.c_str(),tm);
	}
	virtual int doTimer(int tm)=0;
	EventListener *listener;
};

class TouchView:public BaseView,public touch_element{
	public:
	TouchView(){
		listener=NULL;
	}
	~TouchView(){

	}

	void setTouchEventListener(EventListener *L){
		listener=L;
	}

	 void onTouchDown(){
		//log_s("%s doTouchDown!!!!!!!!!!!!!!!!!! \n",name.c_str());
		doTouchDown();
		if(listener!=NULL)
			listener->onTouchDown(name.c_str());
	}
	 void onTouchUp(){
		 doTouchUp();
		if(listener!=NULL)
			listener->onTouchUp(name.c_str());
	}
	 void onTouchActive(){
		 doTouchActive();
		if(listener!=NULL)
			listener->onTouchActive(name.c_str());
	}
	 virtual void doTouchDown()=0;
	 virtual void doTouchUp()=0;
	 virtual void  doTouchActive()=0;


	EventListener *listener;
};


class View:public BaseView,public touch_element, public timer_element{
	public:
	View(){
		listener=NULL;
	}
	~View(){

	}

	void setTouchEventListener(EventListener *L){
		listener=L;
	}

	 void onTouchDown(){
		 doTouchDown();
		if(listener!=NULL)
			listener->onTouchDown(name.c_str());

	}
	 void onTouchUp(){
		 doTouchUp();
		if(listener!=NULL)
			listener->onTouchUp(name.c_str());
	}
	 void onTouchActive(){
		 doTouchActive();
		if(listener!=NULL)
			listener->onTouchActive(name.c_str());

	}
	 int onTimer(int tm){
		 doTimer(tm);
		if(listener!=NULL)
			listener->onTimer(name.c_str(),tm);

	}

	 virtual void doTouchDown()=0;
	 virtual void doTouchUp()=0;
	 virtual void  doTouchActive()=0;

	virtual int doTimer(int tm)=0;


	EventListener *listener;
};









#endif //__LAYER_H__