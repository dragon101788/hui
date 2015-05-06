#ifndef __TOUCH_WHEEL_H__
#define __TOUCH_WHEEL_H__

#include "XMLInstal.h"
#include "view.h"
#include "ttf_font.h"


class touch_wheel:public TouchView{


public:



	class node: public BaseView
	{
	public:



		node()
		{
			color = 0;
			font = NULL;
			style = FONT_NORMAL;
			size = 16;
			hide = 0;
			lenth = 0;
			txt="no value";
			boss=NULL;
		}
		~node()
		{

			//log_i("~node()!!!!!!!!!!!!!!!!!!!\n");
			//xml_mgr->DelElement(name);
		}

		void doDelete()
		{

		}
		void doGetInfo(info & info)
		{
			GetElementInfo(info);
			info.AddInfo("txt", txt);
			info.AddInfo("size", size);
			info.AddInfo("font", font);
		}
		void doFlushConfig()
		{
			PraseElement();
			parent->tobeParent(name.c_str(),this);
			boss=(touch_wheel *)parent;
			render_cached=1;
			font = m_mp["font"]->getvalue();
			style = (unsigned char) m_mp["style"]->getvalue_int();
			size = m_mp["size"]->getvalue_int();
			ttf.fontHeight = size;
			ttf.style = style;
			ttf.m_font = &font_mp[font];

			doFlushConfigCom();
			if(!m_mp["cached"]->getvalue_int()){
				Flush();
			}

		}
		//仅将后期可能需要改变对的参数放在这里面

		void doFlushConfigReduced()
		{
			doFlushConfigCom();
			Flush();
		}
		void doFlushConfigCom()
		{
//			int red = m_mp["red"]->getvalue_int();
//			int green = m_mp["green"]->getvalue_int();
//			int blue = m_mp["blue"]->getvalue_int();
			color =m_mp["color"]->getvalue_hex();// (red & 0xff) << 16 | (green & 0xff) << 8 | blue & 0xff;
			ttf.color = color;
			ttf.SetBuffer(width, height);

			//int padding_left=width/2-((txt.length())*size)/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
			int padding_left=(width>>1)-(((txt.length())*size)>>2);

			padding_left>0?padding_left:0;
			int padding_top=(height-size)>>1;
			padding_top>0?padding_top:0;
			lock();
			if(boss->edge_fading){
				if(id==0){
					ttf.DrawText( (char *) txt.c_str(), txt.length(),padding_left,padding_top,(float)0,(float)1);
				}else if(id==boss->node_num-1){
					ttf.DrawText( (char *) txt.c_str(), txt.length(),padding_left,padding_top,(float)1,(float)0);
				}else
					ttf.DrawText( (char *) txt.c_str(), txt.length(),padding_left,padding_top);
			}
			else
			ttf.DrawText( (char *) txt.c_str(), txt.length(),padding_left,padding_top);
			unlock();

		}

		void doRenderConfig()
		{
			render_res[0].img=&ttf;
		}
		map<hustr, text>::iterator font_it;
		unsigned int color;
		hustr font;	//保存路径
		hustr txt;
		text ttf;
		unsigned char style;
		int size;
		int lenth;
		int buf_len;
		int id;
		touch_wheel * boss;
	};


	void doRenderConfig()
			{

			}
	void doDelete()
	{

	}
	void doFlushConfig()
		{
			PraseElement();
			num_max = m_mp["max"]->getvalue_int();
			num_min = m_mp["min"]->getvalue_int();
			step = m_mp["step"]->getvalue_int();
//			direct =m_mp["direct"]->getvalue_int();
			edge_fading=m_mp["edge_fading"]->getvalue_int();
			node_num=m_mp.count("node");
			for (int i = 0; i < node_num; i++)
			{
				if (nodemp[i] == NULL)
				{
					nodemp[i] = new node;
					nodemp[i]->m_mp.fetch(m_mp["node"][i]);
					nodemp[i]->id=i;
					nodemp[i]->xml_mgr = xml_mgr;
					nodemp[i]->mgr = mgr;
					nodemp[i]->parent = this;
				}

	    	}
			step_h=m_mp["remin"]->getvalue_int();
			set_num= m_mp["set_num"]->getvalue_int();
			effect_num=set_num;
			FlushConfigCom();
			TouchParaseXml(m_mp);
			touch_init_area(abs_x, abs_y, width, height);
			xml_mgr->AddEleArea(this);
			xml_mgr->UnDoneProc();//统一刷新
			for (int i = 0; i < node_num; i++)
				{
				nodemp[i]->FlushConfig();
				}
			xml_mgr->DoneProc();

		}
//	//此函数仅用于刷新外部需要刷新的情况
//	void doFlushConfigReduced()
//			{
//				hide = m_mp["hide"]->getvalue_int();
//				//set_num= m_mp["set_num"]->getvalue_int();
//				step_h=m_mp["remin"]->getvalue_int();
//				set_num= m_mp["set_num"]->getvalue_int();
//				FlushConfigCom();
//				xml_mgr->UnDoneProc();//统一刷新
//				for (int i = 0; i < node_num; i++)
//					{
//					nodemp[i]->FlushConfigReduced();
//					}
//				xml_mgr->DoneProc();
//			}
	void FlushConfigCom()
				{
					if(node_num==3){
						if(set_num-step>=num_min&&(set_num+step<=num_max)){
							nodemp[0]->txt=hustr("%d",set_num+step);
							nodemp[1]->txt=hustr("%d",set_num);
							nodemp[2]->txt=hustr("%d",set_num-step);
						}
						else if(set_num==num_min){
							nodemp[2]->txt=hustr("%d",num_max);
							nodemp[1]->txt=hustr("%d",set_num);
							nodemp[0]->txt=hustr("%d",set_num+step);

						}
						else if(set_num==num_max){
							nodemp[2]->txt=hustr("%d",set_num-step);
							nodemp[1]->txt=hustr("%d",set_num);
							nodemp[0]->txt=hustr("%d",num_min);
						}
					}else if(node_num==5){
						if(set_num-step*2>=num_min&&(set_num+step*2<=num_max)){
							nodemp[0]->txt=hustr("%d",set_num+step*2);
							nodemp[1]->txt=hustr("%d",set_num+step);
							nodemp[2]->txt=hustr("%d",set_num);
							nodemp[3]->txt=hustr("%d",set_num-step);
							nodemp[4]->txt=hustr("%d",set_num-step*2);
						}
						else if(set_num==num_max){
							nodemp[0]->txt=hustr("%d",num_min+step);
							nodemp[1]->txt=hustr("%d",num_min);
							nodemp[2]->txt=hustr("%d",set_num);
							nodemp[3]->txt=hustr("%d",set_num-step);
							nodemp[4]->txt=hustr("%d",set_num-step*2);
						}
						else if(set_num==num_max-1){
							nodemp[0]->txt=hustr("%d",num_min);
							nodemp[1]->txt=hustr("%d",set_num+step);
							nodemp[2]->txt=hustr("%d",set_num);
							nodemp[3]->txt=hustr("%d",set_num-step);
							nodemp[4]->txt=hustr("%d",set_num-step*2);
						}
						else if(set_num==num_min){
							nodemp[0]->txt=hustr("%d",set_num+step*2);
							nodemp[1]->txt=hustr("%d",set_num+step);
							nodemp[2]->txt=hustr("%d",set_num);
							nodemp[3]->txt=hustr("%d",num_max);
							nodemp[4]->txt=hustr("%d",num_max-step);
						}
						else if(set_num==num_min+1){
							nodemp[0]->txt=hustr("%d",set_num+step*2);
							nodemp[1]->txt=hustr("%d",set_num+step);
							nodemp[2]->txt=hustr("%d",set_num);
							nodemp[3]->txt=hustr("%d",num_min);
							nodemp[4]->txt=hustr("%d",num_max);
						}
					}

				}

int abs(int a){
	return a>=0?a:-a;
}
	void doTouchDown()
	{
		static int old_num=0;
//		if(direct)
//			setDirect(1);
		my = move_y();
		step_my=my/step_h;

		if(abs(step_my)>0){
			set_num=effect_num+step_my*step;
			if(set_num>num_max){
				set_num%=num_max;
			}
			else if(set_num<num_min){
				set_num+=num_max;
			}

			if(old_num!=set_num){
				old_num=set_num;
			FlushConfigCom();
			xml_mgr->UnDoneProc();//统一刷新
			for (int i = 0; i < node_num; i++)
				{
				nodemp[i]->doFlushConfigReduced();
				}
			xml_mgr->DoneProc();
			if(listener!=NULL){
				listener->onNumChanged(set_num);
			}
		}
		}

/*

		if ((abs(step_my) < step_cnt))//在去抖范围内，响应为点击事件
		{

		}
		else{ //步子大一点才刷新
			step_cnt++;
			//int lenth_step=my/step_h;
			//log_i("refresh lenth_step=%d!!\n",lenth_step);
			set_num+=step_my>0?step:-step;
			if(set_num>num_max){
				set_num%=num_max;
			}
			else if(set_num<num_min){
				set_num+=num_max;
			}

			FlushConfigCom();
			xml_mgr->UnDoneProc();//统一刷新
			for (int i = 0; i < node_num; i++)
				{
				nodemp[i]->FlushConfigReduced();
				}
			xml_mgr->DoneProc();
			if(listener!=NULL){
				listener->onNumChanged(set_num);
			}

		}
		*/
	}

	void doTouchUp()
	{
		effect_num=set_num;
		step_cnt=1;
//		if(direct){ //设置直接输出
//			setDirect(0);
//			if(hasParent())
//				parent->Flush(); //刷新父亲以保证其他元素也能输出到xmlproc
//		}
	}

	void doTouchActive()
	{

	}
	 void setHide(int hide){
		this->hide=hide;

	}

//void setDirect(int d){
//	xml_mgr->directDraw=d;
//}
	 void setNum(int num){
		 set_num=num;
		 if(set_num>num_max){
			set_num%=num_max;
		}
		else if(set_num<num_min){
			set_num+=num_max;
		}

		FlushConfigCom();
		xml_mgr->UnDoneProc();//统一刷新
		for (int i = 0; i < node_num; i++)
			{
			nodemp[i]->doFlushConfigReduced();
			}
		xml_mgr->DoneProc();
	 }

	 bool getNum(){
		 return set_num;
	 }

	 class NumChangedListener{
	 public:
		 virtual void onNumChanged(int num){}
	 };

	 void setOnNumChangedListener(NumChangedListener *L){
		 listener=L;
	 }



		touch_wheel(){
			node_num=0;
			step=1;
			num_min=0;
			num_max=1;
			set_num=1;
			step_cnt=1;
			edge_fading=0;
			listener=NULL;
		}
		~touch_wheel(){
			nodemp.clear();
		}

	protected:
	int node_num;
	int num_min,num_max;
	int step;
	int set_num;
	int my,step_my;
	int step_h;
	int step_cnt;
	int edge_fading;
//	int direct;
	int effect_num;
	NumChangedListener *listener;
	map<int, node *> nodemp;

};







#endif //__STATIC_IMAGE_H__
