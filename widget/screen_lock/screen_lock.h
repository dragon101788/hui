#ifndef __SCREEN_LOCK__
#define __SCREEN_LOCK__
#include "XMLInstal.h"
#include "view.h"


class screen_lock: public TouchView
{
public:
	class drag_touch_rect
	{
	public:
		drag_touch_rect()
		{
			active = 0;
		}
		int touch_area(int tx, int ty)
		{
			//printf("drag_touch_rect touch\r\n");

			if (active == 0)
			{
				//exec.start();
				boss->xml_mgr->AddExec(0, exec);
				active = 1;

				return 1;
			}

		}
		void free_area(int tx, int ty)
		{
			//printf("drag_touch_rect free\r\n");
			active = 0;
		}

		void touch_activate(int tx, int ty)
		{
		}

		int top;
		int bottom;
		int left;
		int right;
		//hustr cmd;
		HuExec exec;
		lj_lock * boss;
		int active;
		//list<set_config *> scfg;
	};

	screen_lock()
	{
		current_rect = NULL;
	}
	void doTouchDown()
	{
		//printf("tx=%d ty=%d\r\n",tx,ty);

		drag_touch_rect * toe = *it;
		if (GetTouchX() > toe->left && GetTouchX() < toe->right && GetTouchY() > toe->top && GetTouchY() < toe->bottom)
		{
			//printf("%d %d down\r\n",samp.x,samp.y);
			if (current_rect != NULL)
				current_rect->free_area(x, y);

			current_rect = toe;
			int ret = toe->touch_area(x, y);

			if (it != rects.end())
				++it;

		}
		Flush();
		//y=ty;

		//printf("top=%d bottom=%d left=%d right=%d x=%d ox=%d\r\n",top,bottom,left,right,x,ox);

	}

	void doTouchUp()
	{
		it = rects.begin();
		//Flush();
		//UpdateRect();
	}

	void doTouchActive()
	{
		//UpdateRect();
		if (current_rect != NULL)
		{
			current_rect->touch_activate(GetTouchX(), GetTouchY());
		}
	}
	void doDelete()
	{
		xml_mgr->DelTouchElement(this);
	}
	void doFlushConfig()
	{
		PraseElement();

		for (int i = 0; i < m_mp.count("rect"); i++)
		{
			drag_touch_rect * te = new drag_touch_rect;
			te->boss = this;
			te->top = m_mp["rect"][i]["top"]->getvalue_int();
			te->bottom = m_mp["rect"][i]["bottom"]->getvalue_int();
			te->left = m_mp["rect"][i]["left"]->getvalue_int();
			te->right = m_mp["rect"][i]["right"]->getvalue_int();
			te->exec.parse(m_mp["rect"][i]);
			rects.push_back(te);
		}
		it = rects.begin();
		img.SetResource(m_mp["node"]->getvalue());

		img.LoadResource();

		TouchParaseXml(m_mp);
		touch_init_area(x, y, width, height);
		xml_mgr->AddEleArea(this);
		Flush();

	}
	void doRender()
	{
		//image::Render(&img, move_x(), move_y());
		scroll_x=move_x();
		scroll_y=move_y();
		prender_res[0]=&img;

	}

	drag_touch_rect * current_rect;
	list<drag_touch_rect *>::iterator it;
	list<drag_touch_rect *> rects;
	image img;
	HuExec exec;

};

#endif

