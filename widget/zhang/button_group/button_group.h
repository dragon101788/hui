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
        use_small=0;
        pic_sc=NULL;

	}
	~button_group()
	{

	}
	 class dm_image : public image
       	 {
       	 public:
                dm_image()
                {
                        dx = 0;
                        dy = 0;
                }
                HuExec exec;
                int dx;
                int dy;
        };

	class node: public TouchView
	{
	public:
		void doTouchDown()
		{
			//debug("$$$HU$$$ %s %s tx=%d ty=%d t%d b%d l%d r%d\r\n",name,__FUNCTION__,tx,ty,top,bottom,left,right);
			if (isdn == 1)
				return;
			Flush();
		}

		void doTouchUp()
		{
			if (isdn == 0)
				return;
			Flush();
		}

		void doTouchActive()
		{
			if(father->mode=="radio")
			{
				father->cleanSelect();
				select = 1;
		        }
			else if(father->mode=="multi")
			{
				if(select_old)//双击取消
				select=0;
				else
				select=1;	
				select_old=select;
			}
			
		//	exec.start();
		 xml_mgr->AddExec(0, exec);//new added

		}
		node()
		{ 
			select = 0;
			select_old=0;
		}
		void doDelete(){
			xml_mgr->DelTouchElement(this);
		}
		void doFlushConfig()
		{
			name = m_mp["name"]->getvalue();
			int tmpX = father->x+m_mp["x"]->getvalue_int();
			int tmpY = father->y+ m_mp["y"]->getvalue_int();

			width = m_mp["width"]->getvalue_int();
			height = m_mp["height"]->getvalue_int();
			render_width=width;
			render_height=height;
			hide = m_mp["hide"]->getvalue_int();

			if (m_mp.exist("hide_lay"))
			{
				hide_lay= m_mp["hide_lay"]->getvalue_int();
			}
			//控件被移动
			if (tmpX != x || tmpY != y)
			{
				cleanLastPos();
				x = tmpX;
				y = tmpY;
			}
			abs_x=x;
			abs_y=y;

			if (m_mp.exist("lay"))
			{
				lay = m_mp["lay"]->getvalue_int();
			}
			else
			{
				lay = 5;
			}
			if (pSrcBuffer == NULL)
			{
				//log_i("%s SetBuffer width=%d height=%d\r\n", name.c_str(), width, height);
				SetBuffer(width, height);
				path.format("ele-%s %dx%d", name.c_str(), width, height);
			}
			initstack();

			exec.parse(m_mp);
			touch_init_area(abs_x, abs_y, width, height);
			xml_mgr->AddEleArea( this);
			//Flush();
		}
//		void doFlushConfigReduced()
//		{
//			exec.parse(m_mp);
//			//Flush();
//		}
		void doRender()
		{
			if(this->father->mode=="def"){
		       // image::Render(&this->father->img[isdn], x-this->father->x, y-this->father->y,width,height,0,0);
				prender_res[0]=&father->img[isdn];
		        scroll_x=x-father->x;
		        scroll_y=y-father->y;
			}
			else{
           	       // image::Render(&this->father->img[select], x-this->father->x, y-this->father->y,width,height,0,0);
				prender_res[0]=&father->img[select];
		        scroll_x=x-father->x;
		        scroll_y=y-father->y;
			}
		}

		int select;
		int select_old;
		HuExec exec;
		button_group * father;
	};
	void doFlushConfig()
	{
		int j;
		hustr pic_sc;
		PraseElement();
		name = m_mp["name"]->getvalue();
		if (m_mp.exist("mode"))
                mode= m_mp["mode"]->getvalue();
		else
		mode="def";//没有指定模式则默认普通按钮

		if (m_mp.exist("small_pic"))
		{
			use_small=m_mp["small_pic"]->getvalue_int();
		}
		for(j=0;j<2;j++)
       		 {
				if(j==0)
				{
				pic_sc="up";
				}
				else
				{
				pic_sc="dn";
				}
				if (img[j].isNULL())
				  {
					node_num=m_mp.count("node");
				   if(use_small==0)//使用大图
				   {
					img[j].SetResource(m_mp[pic_sc.c_str()]->getvalue());//设置全局图片
					img[j].LoadResource();
				   }
				  else //使用小图片
				   {
					log_i("use small pic!!!!!!!!!!!!!!!!!!!\n");
					hustr filename("%s_%d.png", name.c_str(),j);
					if (access_Image(filename))
					{
						img[j].SetResource(filename);
						img[j].LoadResource();
					}
					else
					{

						map<int, dm_image> tmp;

						for (int i = 0; i < node_num; i++)
						{
							HUMap & mp = m_mp["node"][i];
							tmp[i].dx=mp["x"]->getvalue_int();
							tmp[i].dy=mp["y"]->getvalue_int();
							tmp[i].SetResource(mp[pic_sc.c_str()]->getvalue());
							tmp[i].LoadResource();
						}
						img[j].path.format("slip_menu-%s output", name.c_str());
						img[j].SetBuffer(width, height);
						for (int i = 0; i < tmp.size(); i++)
						{
							img[j].Render(&tmp[i], tmp[i].dx,tmp[i].dy);
						}
						img[j].SaveResource(filename);
					}
				   }
				  }
	       	 }

		int touch_lock = m_mp["lock"]->getvalue_int();
		for (int i = 0; i <node_num; i++)
		{
			if (nodemp[i] == NULL)
			{
				nodemp[i] = new node;
				nodemp[i]->m_mp.fetch(m_mp["node"][i]);
				nodemp[i]->father = this;
				nodemp[i]->xml_mgr = xml_mgr;
				nodemp[i]->mgr = mgr;
			}

			nodemp[i]->touch_lock = touch_lock;
				nodemp[i]->FlushConfig();

		}
		Flush();

	}
	void doFlushConfigReduced()
	{
		if (m_mp.exist("mode"))
                mode= m_mp["mode"]->getvalue();
		else
		mode="def";//没有指定模式则默认普通按钮

		int touch_lock = m_mp["lock"]->getvalue_int();

		for (int i = 0; i <node_num; i++)
		{

			nodemp[i]->FlushConfig();
			nodemp[i]->touch_lock = touch_lock;
		}
		Flush();

	}


	void doRender()
	{
		//img[0].LoadResource();
		prender_res[0]=&img[0];

	}

	void cleanSelect()
	{
		map<int, node *>::iterator it;
		for (it = nodemp.begin(); it != nodemp.end(); it++)
		{
			if (it->second->select)
			{
				it->second->select = 0;
				it->second->Flush();
			}
		}
	}

	void doDelete(){
		for (int i = 0; i < node_num; i++)
		{
			nodemp[i]->xml_mgr->element_manager::DelElement(nodemp[i]->name);
    	}
	}
	map<int, node *> nodemp;
	image img[2];
        int use_small;
        hustr pic_sc;
        int node_num;
	hustr mode; //"def":普通案件 "radio":单选  "multi":多选
 };







#endif //__STATIC_IMAGE_H__
