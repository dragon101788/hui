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
	map<int, text_node *> nodemp;

	wheel_text(){
		node_num=0;
		step=1;
		num_min=0;
		num_max=1;
		set_num=1;
	}
	~wheel_text(){

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
					nodemp[i] = new text_node;
					nodemp[i]->m_mp.fetch(m_mp["node"][i]);
					nodemp[i]->parent = this;
					nodemp[i]->xml_mgr = xml_mgr;
					nodemp[i]->mgr = mgr;
				}

	    	}
			if(node_num==3){

				if(set_num-step>=num_min&&(set_num+step<=num_max)){
					nodemp[0]->text=hustr("%d",set_num-step);
					nodemp[1]->text=hustr("%d",set_num);
					nodemp[2]->text=hustr("%d",set_num+step);
				}
			}else if(node_num==5){
				if(set_num-step*2>=num_min&&(set_num+step*2<=num_max)){
					nodemp[0]->text=hustr("%d",set_num-step*2);
					nodemp[1]->text=hustr("%d",set_num-step);
					nodemp[2]->text=hustr("%d",set_num);
					nodemp[3]->text=hustr("%d",set_num+step);
					nodemp[3]->text=hustr("%d",set_num+step*2);
				}
			}


};





class text_node: public element
{
public:
	text ttf;

	text_node()
	{
		color = 0;
//		bkcolor=0;
		font = NULL;
		style = FONT_NORMAL;
		size = 16;
		hide = 0;
		lenth = 0;
	}
	~text_node()
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
		color=m_mp["color"]->getvalue_int();

		//bkcolor = m_mp["bkcolor"]->getvalue_int();
		font = m_mp["font"]->getvalue();
		//txt = m_mp["txt"]->getvalue();
		style = (unsigned char) m_mp["style"]->getvalue_int();
		size = m_mp["size"]->getvalue_int();

		ttf.m_font = &font_mp[font];
		printf("get font_mp %x %x\r\n", font_mp[font].face,
				font_mp[font].ft_Lib);
		ttf.fontHeight = size;
		ttf.color = color;
		ttf.style = style;
		ttf.SetBuffer(width, height);
		ttf.DrawText("UTF-8", (char *) txt.c_str(), txt.length());

		Flush();
	}
	void doRender()
	{
		image::Render(&ttf, 0, 0);
		//	image::Render(&img, 0, 0, width, height, 0, 0);
	}

	map<hustr, text>::iterator font_it;
	unsigned int color;
	hustr font;	//保存路径
	hustr txt;	/

	unsigned char style;
	int size;
	int lenth;
	int buf_len;
};

#endif //__STATIC_IMAGE_H__
