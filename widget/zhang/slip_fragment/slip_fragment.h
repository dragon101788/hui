#ifndef __SLIP_FRAGMENT_H__
#define __SLIP_FRAGMENT_H__

#include "XMLInstal.h"
#include "layer.h"
#include "thread_msg.h"


class slip_fragment: public element, public touch_element, public timer_element
{

	int hu_abs(int number)
	{
	        return (number >= 0 ? number : -number);
	}
public:

	int doTimer(int tm)
	{
		for (int i = 0; i < rcn && cx > dx; i++)
			if (cx > dx) //当前x大于目的x，则减小当前x(cx),包括翻页和回弹(自动向左运动)
				cx--;
		for (int i = 0; i < rcn && cx < dx; i++)//(自动向右运动)
			if (cx < dx)
				cx++;
		for (int i = 0; i < rcn && cy > dy; i++)
			if (cy > dy) //当前x大于目的x，则减小当前x(cx),包括翻页和回弹(自动向左运动)
				cy--;
		for (int i = 0; i < rcn && cy < dy; i++)//(自动向右运动)
			if (cy < dy)
				cy++;
		Flush();
		if (dx == cx&&dy == cy)
		{
			if(child_lock)
				children_touch_lock=0;
			configChildAbsPos();
			TimerStop();
		}
		else
		{
			TimerSet(tm + rsp);
		}
	}
	void doTouchDown()
	{


//		if (dx != cx)
//		{
//			return;
//		}
//		dx=cx;
//		dy=cy;
		if (cx - move_x() < sum_w)
		{
			mx = move_x();

		}
		if (cy - move_y() < sum_h)
		{
			my = move_y();

		}
		 if ((hu_abs(move_x()) < remin)&&(hu_abs(move_y()) < remin))//在去抖范围内，响应为点击事件
		{
				if(child_lock){
					children_touch_lock=0;

				}
				configChildAbsPos();
		}
		else{ //步子大一点才刷新

			if(child_lock){
				children_touch_lock=1;
				//configChildAbsPos();
			}
			Flush();
		}


	}

	void doTouchUp()
	{
        cx-=mx;
        cy-=my;
		mx=0;
		my=0;
		if (hu_abs(move_x()) > remax)
		{ //移动页面
			log_i("move\r\n");
			if (move_x() > 0)//右移，页面减1
			{
				log_i("--\r\n");
				if (x_page  > 0)
				{
					x_page--;
					dx=width*x_page;
				}

			}
			else//左移，页面加一
			{
				if (x_page + 1 < x_page_num)
				{
					x_page++;
			        dx=width*x_page;
				}
				log_i("++OK\r\n", x_page);
			}
            TimerSet(0);
			xml_mgr->PostCS(hustr("x_page%d", x_page + 1));
			//Flush();
		}
		else if (hu_abs(move_x()) < remax && hu_abs(move_x()) > remin)
		{					//还原页面
			dx=width*x_page;
			TimerSet(0);
			//Flush();
		}
		else if (hu_abs(move_x()) < remin)//在去抖范围内，响应为点击事件
		{
			cx=dx;
			//Flush();
		}

/***********************************
 * y轴变化
 */

		if (hu_abs(move_y()) > remax)
		{ //移动页面
			log_i("move\r\n");
			if (move_y() > 0)//右移，页面减1
			{
				log_i("--\r\n");
				if (y_page > 0)
				{
					y_page--;
					dy=height*y_page;
				}

			}
			else//左移，页面加一
			{
				if (y_page + 1 < y_page_num)
				{
					y_page++;
			        dy=height*y_page;
				}
				log_i("++OK\r\n", y_page);
			}
            TimerSet(0);
			xml_mgr->PostCS(hustr("y_page%d", y_page + 1));
			//Flush();
		}
		else if (hu_abs(move_y()) < remax && hu_abs(move_y()) > remin)
		{					//还原页面
			dy=height*y_page;
			TimerSet(0);
			//Flush();
		}
		else if (hu_abs(move_y()) < remin)//在去抖范围内，响应为点击事件
		{
			cy=dy;
		//	Flush();
		}

	}

	void doTouchActive()
	{

	}
	void doRender()
	{
		//image::Render(&img[0], cx - mx, 0, (int)page_w, (int)height, 0, 0);
		scroll_x=cx - mx;
		scroll_y=cy - my;
	}
	void doFlushConfig()
	{

		PraseElement();
		x_lock = m_mp["x_lock"]->getvalue_int();
		y_lock = m_mp["y_lock"]->getvalue_int();
		rsp = m_mp["rsp"]->getvalue_int();
		rcn = m_mp["rcn"]->getvalue_int();
		remax = m_mp["remax"]->getvalue_int();
		remin = m_mp["remin"]->getvalue_int();
		x_page=m_mp["x_page"]->getvalue_int();
		y_page=m_mp["y_page"]->getvalue_int();
		child_lock=m_mp["child_lock"]->getvalue_int();
		sum_w=width*x_page_num;
		sum_h=height*y_page_num;
		int touch_lock = m_mp["lock"]->getvalue_int();
		touch_init_area(x, y, width, height);
		xml_mgr->AddEleArea( this);
		xml_mgr->AddTimerElement( this);
		Flush();

	}

	void doFlushConfigReduced()
	{
		rePraseElement();
		x_lock = m_mp["x_lock"]->getvalue_int();
		y_lock = m_mp["y_lock"]->getvalue_int();
		x_page=m_mp["x_page"]->getvalue_int();
		y_page=m_mp["y_page"]->getvalue_int();
		child_lock=m_mp["child_lock"]->getvalue_int();
		sum_w=width*x_page_num;
		sum_h=height*y_page_num;
		int touch_lock = m_mp["lock"]->getvalue_int();
		touch_init_area(x, y, width, height);
		Flush();
	}
	slip_fragment()
	{

		sum_w = 0;
		sum_h = 0;
		cx = 0; //当前x
		mx = 0; //拖动轴mx
		dx = 0; //时间轴x
		cy = 0; //当前y
		my = 0; //拖动轴my
		dy = 0; //时间轴y
		x_page = 0;
		y_page = 0;
		rsp=0;//返回定时器频率
		rcn=0;///返回速度
		child_lock=0;
		remax = 0;
		remin = 0;

	}
	~slip_fragment()
	{

	}
	void doDelete()
	{
		xml_mgr->DelTimerElement(this);
		xml_mgr->DelTouchElement(this);
	}


	int sum_w;//menu的宽度
	int sum_h; 
	int cx; //图像当前x，绝对值
	int mx; //拖动轴mx
	int dx; //时间轴x,目标x，跟x_page宽度对齐
	int cy; //图像当前x，绝对值
	int my; //拖动轴mx
	int dy; //时间轴x,目标x，跟x_page宽度对齐
	int x_page;
	int y_page;
	int remax;
	int remin;
	int rsp;//返回定时器频率
	int rcn;///返回速度
	int child_lock;

};







#endif //__STATIC_IMAGE_H__
