#ifndef __TOUCH_MGR_H__
#define __TOUCH_MGR_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include <list>
#include <set>
using namespace std;
#include "ParaseXML.h"
#include "hulib.h"
#include "layer.h"
class touch_manager;


//class origin
//{
//public:
//
//	origin()
//	{
//		ox = 0;
//		oy = 0;
//		mx = 0;
//		my = 0;
//		x_lock = 0;
//		y_lock = 0;
//	}
//
//	int move_x()
//	{
//		if (x_lock)
//			return 0;
//		else
//			return mx;
//	}
//	int move_y()
//	{
//		if (y_lock)
//			return 0;
//		else
//			return my;
//	}
//	int x_lock;
//	int y_lock;
//	int tx, ty;
//private:
//	int mx, my;
//
//};

class touch_element :virtual public ele_nest_extend,virtual public Mutex
{
public:
//	virtual void doFlushConfig()
//	{
//		errexit("$$$HU$$$ can't virtual doFlushConfig\r\n");
//	}
	touch_element()
	{
		top = 0;
		bottom = 0;
		left = 0;
		right = 0;
		isdn = 0;
		touch_lock = 0;
		touch_mgr = NULL;
		x_lock = 0;
		y_lock = 0;

		ox = 0;
		oy = 0;
		//RegistdoFlushConfig (touch_ele);
	}
	~touch_element()
	{
	}
	int AreaWidth()
	{
		return right - left;
	}
	int AreaHeight()
	{
		return bottom - top;
	}

	virtual void onTouchDown()=0;
	virtual void onTouchUp()=0;
	virtual void onTouchActive() = 0;

	void touch_init_area(int x, int y, int width, int height)
	{
		lock();
		top = y;
		left = x;
		bottom = y + height;
		right = x + width;
		unlock();
	}
	void origin_in()
	{
		if (ox == 0 && oy == 0)
		{
			ox = GetTouchX();
			oy = GetTouchY();
		}
	}

	void origin_out()
	{
		ox = 0;
		oy = 0;
	}
	int move_x()
	{
//		int lock=x_lock;
//		if(hasParent()){
//			lock|=parent->children_x_lock;
//		}
		if (x_lock)
			return 0;
		if (isdn == 0)
			return 0;

		return GetTouchX() - ox;
	}
	int move_y()
	{
//		int lock=y_lock;
//		if(hasParent()){
//			lock|=parent->children_y_lock;
//		}
		if (y_lock)
			return 0;
		if (isdn == 0)
			return 0;

		return GetTouchY() - oy;
	}
	void touch_area()
	{

		lock();
		origin_in();
		onTouchDown();
		isdn = 1;
		unlock();
	}
	void free_area()
	{
		lock();
		onTouchUp();
		isdn = 0;
		origin_out();
		if (GetTouchP() == 0 && isArea(GetTouchX(), GetTouchY()))
		{
			touch_activate();
		}
		unlock();
	}
	void touch_activate()
	{
		lock();
		onTouchActive();
		unlock();
	}
	void TouchParaseXml(HUMap & mp)
	{
		lock();
		touch_lock = mp["lock"]->getvalue_int();
		x_lock = mp["x_lock"]->getvalue_int();
		y_lock = mp["y_lock"]->getvalue_int();
		//log_i("TouchParaseXml %d %d %d\r\n", touch_lock, x_lock, y_lock);
		unlock();
	}

	int isArea(int x, int y)
	{
		return (x > left && x < right && y > top && y < bottom);
	}

	 int GetTouchX();
	 int GetTouchY();
	 int GetTouchP();

	int top;
	int bottom;
	int left;
	int right;
	int isdn;
	int touch_lock;
	int ox, oy;
	int x_lock;
	int y_lock;
	touch_manager * touch_mgr;
};

struct touch_sample
{
	int x;
	int y;
	int pressure;
};
int GetTouchSample(touch_sample * samp);
int TouchInit();


class touch_manager :virtual public Mutex
{
public:

	void AddEleArea(touch_element * ele)
	{
		lock();
		ele->touch_mgr = this;
		mp.insert(ele);
		unlock();
	}

	void init_touch()
	{

	}
	int touch_proc_event(touch_sample * samp);
//	void StopElement(touch_element * ele)
//	{
//		mp[name]->touch_lock = 1;
//	}
	void DelTouchElement(touch_element * ele)
	{
		lock();
		mp.erase(ele);
		unlock();
	}
	void ClearElement()
	{
		lock();
		mp.clear();
		unlock();
	}
	void LockAll()
	{
		lock();
		iterator it;
		for (it = mp.begin(); it != mp.end(); ++it)
		{
			(*it)->touch_lock = 1;
		}
		unlock();
	}
	void UnLockAll()
	{
		lock();
		iterator it;
		for (it = mp.begin(); it != mp.end(); ++it)
		{
			(*it)->touch_lock = 0;
		}
		unlock();
	}
	touch_manager()
	{
		in_touch=0;
	}
	virtual ~touch_manager()
	{
	}
	//ott2001a_sample samp;
	//int ox, oy;
	//touch_element * ooe;
	touch_sample cur_samp;
	typedef set<touch_element *>::iterator iterator;
	set<touch_element *> mp;

	int  inTouch(){
		//log_s("in_touch= %d!!!!!!!!!!!\n",in_touch);
		return in_touch;
	}
	virtual void onGlobalTouchPressed(){};
	virtual void onGlobalTouchReleased(){};
private :
	int in_touch; //触摸处于处理状态，添加此标识用于通知耗时的控件触摸时是否让出系统资源
};

#endif //__TOUCH_MGR_H__