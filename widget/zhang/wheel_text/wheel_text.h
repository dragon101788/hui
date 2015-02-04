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
	int node_num;
public:


	wheel_text(){
		node_num=0;
		step=1;
		num_min=0;
		num_max=1;
		set_num=1;
	}
	~wheel_text(){

	}


	class node: public element
	{
	public:
		text ttf;

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
			//id = m_mp["id"]->getvalue_int();
			int red = m_mp["red"]->getvalue_int();
			int green = m_mp["green"]->getvalue_int();
			int blue = m_mp["blue"]->getvalue_int();
			color = (red & 0xff) << 16 | (green & 0xff) << 8 | blue & 0xff;
			font = m_mp["font"]->getvalue();
			style = (unsigned char) m_mp["style"]->getvalue_int();
			size = m_mp["size"]->getvalue_int();

			ttf.m_font = &font_mp[font];
			printf("get font_mp %x %x\r\n", font_mp[font].face,
					font_mp[font].ft_Lib);
			ttf.fontHeight = size;
			ttf.color = color;
			ttf.style = style;
			//printf("width=%d, height= %d\r\n", width,height);
			ttf.SetBuffer(width, height);
			x-=(txt.length())*size/4; //中心对齐，文本框的x值代表文本框文字的中点位置值
			ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length());
			Flush();
		}
		void doRender()
		{
			//printf("in mode doRender()!!!!!\n");
			image::Render(&ttf, 0, 0);
			//	image::Render(&img, 0, 0, width, height, 0, 0);
		}

		map<hustr, text>::iterator font_it;
		unsigned int color;
		hustr font;	//保存路径
		hustr txt;

		unsigned char style;
		int size;
		int lenth;
		int buf_len;
		wheel_text * parent;
	};


	map<int, node *> nodemp;


	void doRender()
			{
				//image::Render(&ttf, 0, 0);
				//	image::Render(&img, 0, 0, width, height, 0, 0);
			}
	void doFlushConfig()
		{

			PraseElement();

			num_max = m_mp["max"]->getvalue_int();
			num_min = m_mp["min"]->getvalue_int();
			step = m_mp["step"]->getvalue_int();
			set_num= m_mp["set_num"]->getvalue_int();

			node_num=m_mp.count("node");
			for (int i = 0; i < node_num; i++)
			{
				if (nodemp[i] == NULL)
				{
					nodemp[i] = new node;
					nodemp[i]->m_mp.fetch(m_mp["node"][i]);
					nodemp[i]->parent = this;
					nodemp[i]->xml_mgr = xml_mgr;
					nodemp[i]->mgr = mgr;
				}

	    	}
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

			//Flush();

			for (int i = 0; i < node_num; i++)
				{
				nodemp[i]->FlushConfig();
				//printf("nodemp[i]->Flush();!!!!!\n");
				//		nodemp[i]->ttf.DrawText("UTF-8", (char *) nodemp[i]->txt.c_str(), nodemp[i]->txt.length());
				//		nodemp[i]->Flush();
				}


		}
};







#endif //__STATIC_IMAGE_H__
