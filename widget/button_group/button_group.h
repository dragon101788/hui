#ifndef __BUTTON_GROUP_H__
#define __BUTTON_GROUP_H__

#include "XMLInstal.h"
#include "view.h"
#include "thread_msg.h"

class button_group: public BaseView
{
public:
	button_group()
	{
		mode=NULL;


	}
	~button_group()
	{

	}


	class node: public TouchView
	{
	public:
		void doTouchDown()
		{
			//debug("$$$HU$$$ %s %s tx=%d ty=%d t%d b%d l%d r%d\r\n",name,__FUNCTION__,tx,ty,top,bottom,left,right);
			if (isdn == 1)
				return;
			if(father->mode=="def")
				Flush();
		}

		void doTouchUp()
		{
			if (isdn == 0)
				return;
			if(father->mode=="def")
				Flush();
		}

		void doTouchActive()
		{

			if(father->mode=="radio")
			{
				father->cleanSelect(this);
				//select = 1;

				if(select_old)//双击取消
				select=0;
				else
				select=1;
				select_old=select;
				Flush();
			 }
			else if(father->mode=="multi")
			{
				if(select_old)//双击取消
				select=0;
				else
				select=1;
				select_old=select;
				Flush();
			}
		//	exec.start();
		 xml_mgr->AddExec(0, exec);//new added

		}
		node()
		{ 
			select = 0;
			select_old=0;
			alpha_mode=0;
		}
		void doDelete(){
			xml_mgr->DelTouchElement(this);
		}
		void doFlushConfig()
		{
			PraseElement();
			parent->tobeParent(name.c_str(),this);
			father=(button_group *)parent;
			exec.parse(m_mp);
			TouchParaseXml(m_mp);
			alpha_mode=m_mp["alpha_mode"]->getvalue_int();
			touch_init_area(abs_x, abs_y, width, height);
			SetRes(0, m_mp["up"]->getvalue());
			SetRes(1, m_mp["dn"]->getvalue());
			xml_mgr->AddEleArea(this);
			ParseModifRes();
			Flush();
		}
		void doRenderConfig()
		{
			if(this->father->mode=="def"){
				res[isdn].LoadResource();
				render_res[0].img=&res[isdn];
			}
			else{
           	       // image::Render(&this->parent->img[select], x-this->parent->x, y-this->parent->y,width,height,0,0);
				res[select].LoadResource();
				render_res[0].img=&res[select];
			}
		}

		int select;
		int select_old;
		int alpha_mode;
		ElementExec exec;
		button_group * father;
	};

	void doFlushConfig()
	{
		int j;
		hustr pic_sc;
		PraseElement();
		if (m_mp.exist("mode"))
                mode= m_mp["mode"]->getvalue();
		else
		mode="def";//没有指定模式则默认普通按钮
		int touch_lock = m_mp["lock"]->getvalue_int();
		node_num=m_mp.count("node");
		for (int i = 0; i <node_num; i++)
		{
			if (nodemp[i] == NULL)
			{
				nodemp[i] = new node;
				nodemp[i]->m_mp.fetch(m_mp["node"][i]);
				nodemp[i]->parent = this;
				nodemp[i]->xml_mgr = xml_mgr;
				nodemp[i]->mgr = mgr;
			}
			nodemp[i]->FlushConfig();
		//	nodemp[i]->touch_lock = touch_lock;

		}

	}

	void doRenderConfig()
	{


	}

	void cleanSelect(node * p)
	{
		map<int, node *>::iterator it;
		for (it = nodemp.begin(); it != nodemp.end(); it++)
		{
			if (it->second->select)
			{
				it->second->select = 0;
				if(p!=it->second)
					it->second->select_old=0;
				it->second->Flush();
			}
		}
	}

	void doDelete(){
	}
	map<int, node *> nodemp;
        int node_num;
        hustr mode; //"def":普通案件 "radio":单选  "multi":多选
 };







#endif //__STATIC_IMAGE_H__
