#ifndef __BUTTON_GROUP_H__
#define __BUTTON_GROUP_H__

#include "XMLInstal.h"
#include "layer.h"
#include "thread_msg.h"

class button_group: public element
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

	class node: public element, public touch_element
	{
	public:
		void doTouchDown()
		{
			//debug("$$$HU$$$ %s %s tx=%d ty=%d t%d b%d l%d r%d\r\n",name,__FUNCTION__,tx,ty,top,bottom,left,right);
			if (this->isdn == 1)
				return;

			Flush();
		}

		void doTouchUp()
		{
			if (this->isdn == 0)
				return;

			Flush();
		}

		void doTouchActive()
		{
			if(this->parent->mode=="radio")
			{
				parent->cleanSelect();
				select = 1;
		        }
			else if(this->parent->mode=="multi")
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
			PraseElement();
			exec.parse(m_mp);
		//	exec.name = parent->name;
			touch_init_area(abs_x, abs_y, width, height);

//			int def_lay_num = 1;
//			if (mp.find("lay"))
//			{
//				def_lay_num = mp["lay"].getvalue_int();
//			}
//			g_lay_mgr[def_lay_num]->AddElement(name, this);
			xml_mgr->AddEleArea( this);
			Flush();
			//debug("init end\r\n");
		}
		void doFlushConfigReduced()
		{
			exec.parse(m_mp);
			Flush();
		}
		void doRender()
		{
			if(this->parent->mode=="def"){
		       // image::Render(&this->parent->img[isdn], x-this->parent->x, y-this->parent->y,width,height,0,0);
		        cur_res=&this->parent->img[isdn];
		        scroll_x=x-this->parent->x;
		        scroll_y=y-this->parent->y;
			}
			else{
           	       // image::Render(&this->parent->img[select], x-this->parent->x, y-this->parent->y,width,height,0,0);
				cur_res=&this->parent->img[select];
				scroll_x=x-this->parent->x;
				scroll_y=y-this->parent->y;
			}
		}

		int select;
		int select_old;
	//	image img[2];
		HuExec exec;
		button_group * parent;
	};
	void doFlushConfig()
	{
		int j;
		hustr pic_sc;
		//m_mp.display();
		PraseElement();
		name = m_mp["name"]->getvalue();
	//	x = m_mp["x"].getvalue_int();
	//	y = m_mp["y"].getvalue_int();
		
		if (m_mp.exist("mode"))
                mode= m_mp["mode"]->getvalue();
		else
		mode="def";//没有指定模式则默认普通按钮
#if 0
		img[0].SetResource(m_mp["up"].getvalue());//设置全局图片
		img[1].SetResource(m_mp["dn"].getvalue());
		img[0].LoadResource();//加载按下前的图片，此图片可以替代背景图片
		img[1].LoadResource();//加载按下后的图
#endif

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
	//	if (img[j].pSrcBuffer == NULL)
		if (img[j].isNULL())
	   	  {
			node_num=m_mp.count("node");
		   if(use_small==0)//使用大图
		   {
		//	img[j].SetBuffer(width, height, eDRVBLT_DEST_ARGB8888, eDRVBLT_SRC_ARGB8888);
			img[j].SetResource(m_mp[pic_sc.c_str()]->getvalue());//设置全局图片
			img[j].LoadResource();
		   }
	       	  else //使用小图片
	      	   {
			debug("use small pic!!!!!!!!!!!!!!!!!!!\n");
			hustr filename("%s_%d.png", name.c_str(),j);



			if (access_Image(filename))
			{
				img[j].SetResource(filename);
				img[j].LoadResource();
			}


			else
			{

				map<int, dm_image> tmp;

//				for (int i = 0; i < m_mp["node"].BrotherCount(); i++)
				for (int i = 0; i < node_num; i++)
				{
					HUMap & mp = m_mp["node"][i];
					tmp[i].dx=mp["x"]->getvalue_int();
					tmp[i].dy=mp["y"]->getvalue_int();
					tmp[i].SetResource(mp[pic_sc.c_str()]->getvalue());
					tmp[i].LoadResource();
				}
				img[j].path.format("slip_menu-%s output", name.c_str());
			//	img[j].SetBuffer(width, height, eDRVBLT_DEST_ARGB8888, eDRVBLT_SRC_ARGB8888);
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
				nodemp[i]->parent = this;
				nodemp[i]->xml_mgr = xml_mgr;
				nodemp[i]->mgr = mgr;
			}

			nodemp[i]->touch_lock = touch_lock;
		//	if(!m_mp["noDraw"]->getvalue_int())//如果存在noDraw标志就不绘制，等待下一次动态参数
				nodemp[i]->FlushConfig();

		}
		//if(!m_mp["noDraw"]->getvalue_int())//如果存在noDraw标志就不绘制，等待下一次动态参数
			Flush();

	//	debug("Radio_buttons OK\r\n");
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
		//	img[select].LoadResource();
		//	image::Render(&img[0], 0, 0);//显示按下前大图
		cur_res=&img[0];

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
