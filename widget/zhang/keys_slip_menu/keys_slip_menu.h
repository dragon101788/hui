#ifndef __KEYS_SLIP_MENU_H__
#define __KEYS_SLIP_MENU_H__

#include "XMLInstal.h"
#include "view.h"
#include "thread_msg.h"


extern int msgSkip;//当滑动菜单滚动时应该忽略信息处理，此标志谨慎使用
class keys_slip_menu: public TimerView
{
public:

int hu_abs(int number) 
{               
        return (number >= 0 ? number : -number);                     
}   

	 class dm_image : public image
       	 {
       	 public:
                dm_image()
                {
                        dx = 0;
                        dy = 0;
                }
               // HuExec  exec;
                int dx;
                int dy;
        };

	class node: public element
	{
	public:
        int cx,cy;
        int id;
		node()
		{ 
			id=0;
			cx=0;
			cy=0;
		}
		~node()
		{

		}
		void doFlushConfig()
		{

			if(m_mp.exist("name"))
                name = m_mp["name"]->getvalue();
        		else{
        		//	log_i("need add a default name!!!!!!!\n");
        			name=hustr("%s-%d",parent->name.c_str(),id);
        		}

		//	log_i("%s,doFlushConfig\n",name.c_str());
                x = m_mp["x"]->getvalue_int();
                y = m_mp["y"]->getvalue_int();


                width = m_mp["width"]->getvalue_int();
                height = m_mp["height"]->getvalue_int();

                if (m_mp.exist("lay"))
                {
                        lay = m_mp["lay"]->getvalue_int();
                }
                else    
                {               
                        lay = 5;
                }       

                if (isNULL())
                {
                        //log_i("%s SetBuffer width=%d height=%d\r\n", name.c_str(), width, height);
                        SetBuffer(width, height);
                        path.format("ele-%s %dx%d", name.c_str(), width, height);
                }
				int temp_x=x;
				int temp_y=y;
				hide=parent->hide;
				x=x%parent->page_w; //取第一页位置
				y=y%parent->page_h; //取第一页位置
                x=x+parent->x;
                y=y+parent->y;
				initstack();
				x=temp_x;//还原x
				y=temp_y;//还原y
                x=x+parent->x;//x为在屏幕中的绝对位置，而非相对位置！！！
                y=y+parent->y;//y为在屏幕中的绝对位置，而非相对位置！！！

		}
		void doFlushConfigReduced()
		{
		//	log_i("%s,doFlushConfigReduced node\n",name.c_str());
			hide=parent->hide;
			x = m_mp["x"]->getvalue_int();
			y = m_mp["y"]->getvalue_int();

			x=x+parent->x;//x为在屏幕中的绝对位置，而非相对位置！！！
			y=y+parent->y;//y为在屏幕中的绝对位置，而非相对位置！！！
		}
		void doRender()
		{
              //  log_i("this->x=%d,this->cx=%d\n",x,cx);
               //
		//	printf("%s,doRender node\n",name.c_str());
			 if(x-parent->x<parent->width&&y-parent->y<parent->height&&x>=0&&y>=0){
		      //image::Render(&parent->img[1], x+parent->cx-parent->x,
		    	//	  	  	  	  	  	  	  	   y+parent->cy-parent->y,width,height,0,0);

			  prender_res[0]=&parent->img[1];
		      scroll_x=x+parent->cx-parent->x;
		      scroll_y= y+parent->cy-parent->y;
			 }

		}

		keys_slip_menu * parent;
	};

	int doTimer(int tm)
	{
	//	log_i("slip_menu::OnTimer %dms cx=%d dx=%d mx=%d \r\n", tm,cx,dx,mx);
		if(vertical_mode){
			for (int i = 0; i < rcn && cy > dy; i++)
				if (cy > dy){ //当前x大于目的x，则减小当前x(cx),包括翻页和回弹(自动向左运动)page-
					cy--;
				}
			for (int i = 0; i < rcn && cy < dy; i++)//(自动向右运动) page+
				if (cy < dy){
					cy++;
				}
			Flush();
			if (dy == cy)
			{
				msgSkip=0;
				TimerStop();
				if(isFlip)
				nodemp[select_id]->Flush();
			}
			else
			{
				TimerSet(tm + rsp);
			}
		}else{
			for (int i = 0; i < rcn && cx > dx; i++)
				if (cx > dx){ //当前x大于目的x，则减小当前x(cx),包括翻页和回弹(自动向左运动)page-

					// nodemp[select_id]->cx++;
					cx--;
				}
			for (int i = 0; i < rcn && cx < dx; i++)//(自动向右运动) page+
				if (cx < dx){

					cx++;
				}
			Flush();
			if (dx == cx)
			{
				msgSkip=0;
				TimerStop();

				if(isFlip)
				nodemp[select_id]->Flush();
			}
			else
			{
				TimerSet(tm + rsp);
			}
		}
	}


	void changePage(){
		//log_i("page_node_num=%d\r\n", page_node_num);
		//log_i("page=%d\r\n", page);
		if (select_id>=page_node_num*(page+1))//右移，页面+1
		{
			int cnt =select_id/(page_node_num*(page+1));
						if (page +cnt <= const_page)
						{
							isFlip=true;
							page+=cnt;
									if(vertical_mode){
										dy=page_h*page;
										for (int i = 0; i < node_num; i++)
										 {
										  nodemp[i]->cy-=page_h*cnt;
										 }
									}else{
										dx=page_w*page;

										for (int i = 0; i < node_num; i++)
										 {
										  nodemp[i]->cx-=page_w*cnt;
										 }
									}
								printf("++OK\r\n", page);
								if(noAnimation){//首次载入直接进入当前位置，跳过动画
									//noAnimation=false;
									isFlip=false;//isFlip最好理解为是否需要动画
									cy=dy;
									cx=dx;

								}else{
								TimerSet(0);
								msgSkip=1;
								}
								xml_mgr->PostCS(hustr("page%d", page + 1));
						}else{
							isFlip=false;
						}

					}

		else if(select_id<page_node_num*(page))//左移，页面-1
			{
			// int cnt =(page_node_num*page)/(select_id+6);
			// int cnt =(page_node_num*page)/(select_id+6);
			 int cnt = (page_node_num*(page+1)-(select_id+1))/page_node_num;
			if(page-cnt>=0){
			    isFlip=true;
				printf("--\r\n");

				page-=cnt;
				if(vertical_mode){
					dy=page_h*page;
					 for (int i = 0; i < node_num; i++)
					 	 {
							 nodemp[i]->cy+=page_h*cnt;
					 	 }
				}else{
					dx=page_w*page;
					 for (int i = 0; i < node_num; i++)
					 	 {
							 nodemp[i]->cx+=page_w*cnt;
					 	 }
				}
					if(noAnimation){//首次载入直接进入当前位置，跳过动画
						//noAnimation=false;
						isFlip=false;//isFlip最好理解为是否需要动画

						cy=dy;
						cx=dx;

					}else{
					 TimerSet(0);
					 msgSkip=1;
					}
					xml_mgr->PostCS(hustr("page%d", page + 1));
			}
		 else{
			 isFlip=false;
		 }
		}
		else{
			isFlip=false;
		}


	}



	void doGetInfo(info & info)
		{
			GetElementInfo(info);

			info.AddInfo("select", select_id);
			printf("in doGetInfo() select_id=%d\r\n", select_id);
		//	changePage();

		}

	void doRender()
	{
	//	printf("%s,doRender\n",name.c_str());
		//image::Render(&img[0], cx , cy, (int)width, (int)height, 0, 0);
		cur_res=&img[0];
		scroll_x=cx;
		scroll_y=cy;
		if(!isFlip)
		nodemp[select_id]->Flush();//都使用第一页的节点显示


	}
	void doFlushConfig()
	{
		int i,j; 
		PraseElement();
		//cached与hide唯一的区别在于，cache仅在初始化时隐藏，通过sdcfg送任何参数都会将其显示出来
		if(m_mp["cached"]->getvalue_int())
		 hide=1;
		noAnimation=true;
	//	printf("%s,doFlushConfig\n",name.c_str());
		if (m_mp.exist("page"))
		const_page = m_mp["page"]->getvalue_int()-1;//0代表1页，2代表3页,在xml中1代表1页
                rsp = m_mp["rsp"]->getvalue_int();
                rcn = m_mp["rcn"]->getvalue_int();


		select_id=m_mp["select"]->getvalue_int();
		if (m_mp.exist("vertical_mode")){
			vertical_mode=m_mp["vertical_mode"]->getvalue_int();
		}
		//noAnimation=m_mp["noAnim"]->getvalue_int();
		page_w=width;
		page_h=height;
		if(vertical_mode){
			sum_h=height*(const_page+1);
			sum_w=width;
		}else{
			sum_w=width*(const_page+1);
			sum_h=height;
		}
		/*
		sum_w=width;
		width/=const_page+1;//触摸及可视宽度等于图片宽度除以页数
		page_w=width;
*/
		//node_name="pic_node";
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

		   if(use_small==0)//使用大图
		   {
			img[j].SetBuffer(sum_w, sum_h);
                            
			img[j].SetResource(m_mp[pic_sc.c_str()]->getvalue());//设置全局图片
			img[j].LoadResource();//加载按下前的图片，此图片可以替代背景图片
		   }
	       	  else //使用小图片
	      	   {
			printf("use small pic!!!!!!!!!!!!!!!!!!!\n");
			hustr filename("%s_%d.png", name.c_str(),j);
			if (access_Image(filename))
			{
				img[j].SetResource(filename);
				img[j].LoadResource();
			}
			else
			{

				map<int, dm_image> tmp;
				for (int i = 0; i < m_mp.count("node"); i++)
				{
					HUMap & mp = m_mp["node"][i];
					tmp[i].dx=mp["x"]->getvalue_int();	
					tmp[i].dy=mp["y"]->getvalue_int();
					tmp[i].SetResource(mp[pic_sc.c_str()]->getvalue());
					tmp[i].LoadResource();
				}
				img[j].path.format("slip_menu-%s output", name.c_str());
				img[j].SetBuffer(sum_w, sum_h);
				int size=  tmp.size();
				for (int i = 0; i < size; i++)
				{
					img[j].Render(&tmp[i], tmp[i].dx,tmp[i].dy);
					tmp.erase(i);
					//tmp[i].destroy();
				}
				img[j].SaveResource(filename);



			}
		   }
		  }
	     }

		node_num=m_mp.count("node");
		if (m_mp.exist("page_node_num"))
				page_node_num=m_mp["page_node_num"]->getvalue_int();
		else
			page_node_num=node_num/(const_page+1);
		for (int i = 0; i < node_num; i++)
		{
			if (nodemp[i] == NULL)
			{
				nodemp[i] = new node;
				nodemp[i]->id=i;
				nodemp[i]->m_mp.fetch(m_mp["node"][i]);
				nodemp[i]->parent = this;
				nodemp[i]->xml_mgr = xml_mgr;
				nodemp[i]->mgr = mgr;
			}


			nodemp[i]->FlushConfig();

		}


		xml_mgr->AddTimerElement( this);
		changePage();
		if(vertical_mode)
			nodemp[select_id]->y+=nodemp[select_id]->cy;
		else
		nodemp[select_id]->x+=nodemp[select_id]->cx;

		Flush();


	}
	void doFlushConfigReduced()
	{
	//	printf("%s,doFlushConfigReduced\n",name.c_str());
		hide = m_mp["hide"]->getvalue_int();
		select_id=m_mp["select"]->getvalue_int();
		//noAnimation=m_mp["noAnim"]->getvalue_int();
		changePage();
		for (int i = 0; i < node_num; i++)
		{
			nodemp[i]->FlushConfigReduced();

		}
		if(vertical_mode)
			nodemp[select_id]->y+=nodemp[select_id]->cy;
		else
		nodemp[select_id]->x+=nodemp[select_id]->cx;
		Flush();
		if(noAnimation)
			noAnimation=false;
	}


	keys_slip_menu()
	{
		pic_sc=NULL;
		isFlip=false;
		//noAnimation=true;
		node_name=NULL;
		sum_w = 0;
		sum_h = 0;
		page_w = 0;
		page_h = 0;

		vertical_mode=0; //默认是水平翻页
		cx = 0; //当前x
		cy = 0; //当前y
		//mx = 0; //拖动轴mx
		dx = 0; //时间轴x
		dy = 0; //时间轴x
		page = 0;
		 rsp=0;//返回定时器频率
		rcn=0;///返回速度
        const_page=0;
		page_max = 0;
		node_num=0;
		use_small=0;
	    select_id=0;
	    page_node_num=0;
	//	timer_element::mutex.SetMutex(this);
	}

	void doDelete()
	{

		for (int i = 0; i < node_num; i++)
		{
			nodemp[i]->xml_mgr->element_manager::DelElement(nodemp[i]->name);
    	}
		xml_mgr->DelTimerElement(this);
	}
	~keys_slip_menu()
	{

	}

	int sum_w;//menu的宽度
	int sum_h; 
	int page_w;//一页的宽度
	int page_h;//一页的高度
	bool noAnimation;
	//int op;
	int select_id;  //哪张图片选中
	bool isFlip;
	int use_small;
	hustr node_name;
	hustr pic_sc;
	int cx; //图像当前x，绝对值
	int cy; //图像当前y，绝对值
	//int mx; //拖动轴mx
	int dx; //时间轴x,目标x，跟page宽度对齐
	int dy; //时间轴x,目标x，跟page宽度对齐
	int page;
	int const_page;
	int page_max;
	int vertical_mode;//翻页模式，0-水平，1竖直
	int rsp;//返回定时器频率
	int rcn;///返回速度
	int page_node_num;
	map<int, node *> nodemp;
	image img[2];
        int node_num;


};







#endif //__STATIC_IMAGE_H__
