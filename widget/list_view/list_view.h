#ifndef __LIST_VIEW_H__
#define __LIST_VIEW_H__

#include "XMLInstal.h"
//#include "layer.h"
#include "view.h"
#include"ParseXML.h"
#include "thread_msg.h"
#include<list>

typedef list<listMap > itemList;

extern void ParaseUpdateXmlWithData(hustr parentName,TiXmlNode* pParent, xmlproc * xml,listMap &setData);


class OnItemTouchListener{
public:
	OnItemTouchListener(){

	}
	virtual ~OnItemTouchListener(){

	}

	virtual void onItemTouchActive(int item){

	}

};






class list_view: public View
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
				drawPressRec();
		}
		else{ //步子大一点才刷新

			if(child_lock){
				children_touch_lock=1;
				//configChildAbsPos();
			}
			cleanPressRec();
		}
		 Flush();

	}

	void doTouchUp()
	{
        cx-=mx;
        cy-=my;
		mx=0;
		my=0;
		cleanPressRec();
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
		Flush();
	}

	void doTouchActive()
	{
		if(list_touch_listener!=NULL&&!children_touch_lock){//有监听器,且不在滑动状态
			int select_item;
			int tx=GetTouchX()-abs_x;
			int ty=GetTouchY()-abs_y;
			if(direction)//水平模式
			{
				tx+=x_page*width;
				select_item=tx/item_w;
			}else{
				ty+=y_page*height;
				select_item=ty/item_h;
			}
			if(select_item<=item_num)
				list_touch_listener->onItemTouchActive(select_item);
		}

	}
	void doRenderConfig()
	{
		//image::Render(&img[0], cx - mx, 0, (int)page_w, (int)height, 0, 0);
		scroll_x=cx - mx;
		scroll_y=cy - my;
//		if(back_color)
//			render_res[0].img=&bg;
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
//		x_page=m_mp["x_page"]->getvalue_int();
//		y_page=m_mp["y_page"]->getvalue_int();
		child_lock=m_mp["child_lock"]->getvalue_int();
		direction=m_mp["direction"]->getvalue_int();
		item_h=m_mp["item_h"]->getvalue_int();
		item_w=m_mp["item_w"]->getvalue_int();
		sel_color=m_mp["select_color"]->getvalue_hex();
//		if (m_mp.exist("back_color"))//在父元素的第几个页面里,0开始算起
//		{
//			back_color= m_mp["back_color"]->getvalue_hex();
//			bg.SetBuffer(width,height,back_color);
//			//Draw_rectangle(&bg,300,200,50,20,0x77aabbff,1);
//		}

		sum_w=width*x_page_num;
		sum_h=height*y_page_num;
		int touch_lock = m_mp["lock"]->getvalue_int();
		touch_init_area(x, y, width, height);
		xml_mgr->AddEleArea( this);
		xml_mgr->AddTimerElement( this);

		press_image.SetBuffer(item_w,item_h);

		Flush();
	}

	void delItem(int pos,itemList &lists){


		itemList::iterator it;
		int i=0;
		listMap *data=NULL;
		for (it = lists.begin(); it != lists.end(); ++it)
		{
			if(i==pos){
				data=&(*it);
				break;
			}
				i++;
		}
		if(data==NULL){
			return ;
		}
		int num=data->size();
		for(int i=0;i<num;i++)
		{
			//HUMap *mp=data[i];
			hustr SN;
			SN.format("%s-%d-%d",name.c_str(),pos,i);
			xml_mgr->DelElement(SN.c_str());
		}
		lists.remove(*data);
		for(int i=pos+1;i<=item_num;i++){
			hustr SN;
			for(int j=0;j<num;j++){
				SN.format("%s-%d-%d",name.c_str(),i,j);
				BaseView *view=(BaseView *)xml_mgr->GetElementByName(SN.c_str());
				if(direction){
					view->setXY((i-1)*item_w,0);
				}else{
					view->setXY(0,(i-1)*item_h);
				}
			}
		}
		item_num--;
		caliPages();
	}


	void addItemToListEnd(const char * adapter,listMap &data,itemList &lists){
		lists.push_back(data);
		addItemInEnd(adapter,data);
	}

	void addItemInEnd(const char * adapter,listMap &data)
	{
		TiXmlDocument doc;
		if (!doc.LoadFile(adapter))
		{
			errexitf("ParaseTinyXmlFile LoadFile %s %s \r\n		TiXmlDocument error :%s\r\n", xml_mgr->filename.c_str(), strerror(errno),
					doc.ErrorDesc());
		}
		TiXmlElement * root = doc.RootElement();

		if (!root)
		{
			return;
		}

		int offset_x=0;
		int offset_y=0;
		if(direction){
			 offset_x=item_w*item_num;
		}else{
			offset_y=item_h*item_num;
		}
		listMap ::iterator it;
		int i=0;
		for(it=data.begin();it!=data.end();++it){
			HUMap &mp=it->second;
			mp["offset_x"]->format("%d",offset_x);
		//	mp["offset_x"]->setvalue(""+offset_x);
			mp["offset_y"]->format("%d",offset_y);
			mp["SN"]->format("%s-%d-%d",name.c_str(),item_num,i);
			i++;
	}
		item_num++;
		caliPages();
		ParaseUpdateXmlWithData(name,root,xml_mgr,data);
	}

	int caliPages(){
		if(direction){ //像又增加模式
			int w=item_num*item_w;
			int temp_x_page=w/width+1;
			if(temp_x_page!=x_page_num){
				x_page_num=temp_x_page;
				top_image.resize(width*x_page_num,height*y_page_num);

			}
		}else{
			int h=item_num*item_h;
			int temp_y_page=h/height+1;
			if(temp_y_page!=y_page_num){
				y_page_num=temp_y_page;
				top_image.resize(width*x_page_num,height*y_page_num);
			}
		}


	}


void setItems(const char * file,itemList &lists){

		item_num=0;
		itemList::iterator it;
		for(it=lists.begin();it!=lists.end();++it){
			listMap &data=*it;
			addItemInEnd(file,data);
		}

	}

void setOnItemTouchListener(OnItemTouchListener *L){
	list_touch_listener=L;
}

	void drawPressRec(){
		if(!has_pressed){
			int select_item;
			int tx=GetTouchX()-abs_x;
			int ty=GetTouchY()-abs_y;
			if(direction)//水平模式
			{

				if(ty<item_h){
					tx+=x_page*width;
					select_item=tx/item_w;
					if(select_item<=item_num){
						int dst_x=select_item*item_w;
						//dst_x=dst_x%width;
						press_image.drawRectangle(item_w,item_h,0,0,sel_color,0);
						render_res[0].translate(dst_x,0);
						render_res[0].img=&press_image;
						has_pressed=1;
					}
				}
			}else {
				if(tx<item_w){
					ty+=y_page*height;
					select_item=ty/item_h;

					if(select_item<=item_num){
						int dst_y=select_item*item_h;
						//dst_y=dst_y%height;
						log_i("--------in drawrec select=%d,dst_y=%d\n",select_item,dst_y);
						press_image.drawRectangle(item_w,item_h,0,0,sel_color,0);
						render_res[0].translate(0,dst_y);
						render_res[0].img=&press_image;
						has_pressed=1;
					}
				}
			}
		}
	}
	void cleanPressRec(){
		if(has_pressed){
			has_pressed=0;
			press_image.cleanBuf();
		}
	}
	list_view()
	{
		has_pressed=0;
		item_num=0;
		sum_w = 0;
		sum_h = 0;
		item_h=0;
		item_w=0;
		direction=0; //0 :v ;1:h
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
		back_color=0;
		list_touch_listener=NULL;
		sel_color=0;

	}
	~list_view()
	{

	}
	void doDelete()
	{
		xml_mgr->DelTimerElement(this);
		xml_mgr->DelTouchElement(this);
	}
	image press_image;
	unsigned int sel_color;
	int has_pressed;
	int item_h;
	int item_w;
	int direction; //0 :v ;1:h
	int item_num;
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
	int back_color;
	image bg;
	OnItemTouchListener * list_touch_listener;

};







#endif //__STATIC_IMAGE_H__
