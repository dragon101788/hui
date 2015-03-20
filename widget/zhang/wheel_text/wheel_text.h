#ifndef __WHEEL_IMAGE_H__
#define __WHEEL_IMAGE_H__

#include "XMLInstal.h"
#include "layer.h"
#include "ttf_font.h"


class wheel_text:public element{
private:
	int num_min,num_max;
	int step;
	int set_num;

public:
	int node_num;

	wheel_text(){
		node_num=0;
		step=1;
		num_min=0;
		num_max=1;
		set_num=1;
	}
	~wheel_text(){

//		log_i("~wheel_text()!!!!!!!!!!!!!!!!!!!\n");

		nodemp.clear();
	}


	class node: public element
	{
	public:



		node()
		{
			color = 0;
	//		bkcolor=0;
			font = NULL;
			style = FONT_NORMAL;
			size = 16;
			hide = 0;
			lenth = 0;
			txt="no value";
		}
		~node()
		{

			//log_i("~node()!!!!!!!!!!!!!!!!!!!\n");
			//xml_mgr->DelElement(name);
		}

		void test(const char * abc)
		{
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
			/*
			if(m_mp.exist("name"))
                name = m_mp["name"]->getvalue();
        		else{
        			name=hustr("%s-%d",parent->name.c_str(),id);
        		}
			int tmpX = m_mp["x"]->getvalue_int();
			int tmpY = m_mp["y"]->getvalue_int();
			width = m_mp["width"]->getvalue_int();
			height = m_mp["height"]->getvalue_int();
			//控件被移动
			if (tmpX != x || tmpY != y)
			{
				Back();
				x = tmpX;
				y = tmpY;
			}
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
			font = m_mp["font"]->getvalue();
			style = (unsigned char) m_mp["style"]->getvalue_int();
			size = m_mp["size"]->getvalue_int();

			ttf.m_font = &font_mp[font];
			log_i("get font_mp %x %x\r\n", font_mp[font].face,
					font_mp[font].ft_Lib);
			ttf.fontHeight = size;
			ttf.color = color;
			ttf.style = style;
			//log_i("width=%d, height= %d\r\n", width,height);
*/
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

			hide=parent->hide;
			int red = m_mp["red"]->getvalue_int();
			int green = m_mp["green"]->getvalue_int();
			int blue = m_mp["blue"]->getvalue_int();
			color = (red & 0xff) << 16 | (green & 0xff) << 8 | blue & 0xff;
			ttf.color = color;
			ttf.SetBuffer(width, height);
			//ttf.cleanBuf();

			int padding_left=width/2-(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
			padding_left>0?padding_left:0;
			int padding_top=(height-size)/2;
			padding_top>0?padding_top:0;
			if(id==0){
				ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),padding_left,padding_top,(float)0,(float)1);
			}else if(id==parent->node_num-1){
				ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),padding_left,padding_top,(float)1,(float)0);
			}
			else
			ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length(),padding_left,padding_top);


		}

		void doRender()
		{
			//log_i("in mode doRender()!!!!!\n");
			//image::Render(&ttf, 0, 0);
			cur_res=&ttf;
			//	image::Render(&img, 0, 0, width, height, 0, 0);
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
		wheel_text * parent;
	};


	map<int, node *> nodemp;


	void doRender()
			{
				//image::Render(&ttf, 0, 0);
				//	image::Render(&img, 0, 0, width, height, 0, 0);
			}
	void doDelete()
	{

		for (int i = 0; i < node_num; i++)
		{
			nodemp[i]->xml_mgr->element_manager::DelElement(nodemp[i]->name);
    	}

	}
	void doFlushConfig()
		{
			PraseElement();
			num_max = m_mp["max"]->getvalue_int();
			num_min = m_mp["min"]->getvalue_int();
			step = m_mp["step"]->getvalue_int();

			node_num=m_mp.count("node");
			for (int i = 0; i < node_num; i++)
			{
				if (nodemp[i] == NULL)
				{
					nodemp[i] = new node;

					//nodemp[i]->name=hustr("%s-%d",name.c_str(),i);
					nodemp[i]->m_mp.fetch(m_mp["node"][i]);
					nodemp[i]->id=i;
					nodemp[i]->parent = this;
					nodemp[i]->xml_mgr = xml_mgr;
					nodemp[i]->mgr = mgr;
				}

	    	}
			FlushConfigCom();
			xml_mgr->UnDoneProc();//统一刷新
			for (int i = 0; i < node_num; i++)
				{
				nodemp[i]->FlushConfig();
				}
			xml_mgr->DoneProc();

		}
	//此函数仅用于刷新外部需要刷新的情况
	void doFlushConfigReduced()
			{
				hide = m_mp["hide"]->getvalue_int();
				//set_num= m_mp["set_num"]->getvalue_int();
				FlushConfigCom();
				xml_mgr->UnDoneProc();//统一刷新
				for (int i = 0; i < node_num; i++)
					{
					nodemp[i]->FlushConfigReduced();
					}
				xml_mgr->DoneProc();
			}
	void FlushConfigCom()
				{

					log_i("doFlushConfigCom()!!!!!!!  \n");
					set_num= m_mp["set_num"]->getvalue_int();
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




};







#endif //__STATIC_IMAGE_H__
