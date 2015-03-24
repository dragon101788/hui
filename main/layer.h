#ifndef __LAYER_H__
#define __LAYER_H__

#include "ParaseXML.h"
#include "codec.h"
#include "schedule.h"
#include <deque>
#include <set>
//#include "image_rotate.h"
#include "rotate.h"
#include "zoom.h"
using namespace std;

class schedule_draw;
class element_manager;
class element;
class xmlproc;

class info: public hustr
{
public:
	void AddInfo(const char * key, const char * value)
	{
		(*this) += key;
		(*this) += "=";
		(*this) += value;
		(*this) += "\r\n";
	}
	void AddInfo(const char * key, int value)
	{
		hustr str("%d", value);
		AddInfo(key, str);
	}
};


class element_manager
{
public:

	virtual void AddElement(const char * name, element * ele)
	{

		if (GetElementByName(name) == NULL)
		{
			elem[name] = ele;
		}
		else
		{
			log_i("Repeat to add element %s\r\n", name);
		}

	}
	void ClearElement()
	{
		elem.clear();
	}
	 void DelElement(const char * name);
//	virtual void DelElement(const char * name)
//	{
//		element * ele = GetElementByName(name);
//		if (ele == NULL)
//		{
//			log_i("can't del layer element %s\r\n", name);
//		}
//		else
//		{
//			ele->Delete();
//			elem.erase(name);
//		}
//	}
	virtual element * GetElementByName(const char * name)
	{
		map<hustr, SmartPtr<element> >::iterator it;
		it = elem.find(name);
		if (it != elem.end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}
	typedef map<hustr, SmartPtr<element> >::iterator iterator;
	iterator begin()
	{
		return elem.begin();
	}
	iterator end()
	{
		return elem.end();
	}
	element_manager()
	{
	}
	virtual ~element_manager()
	{
	}
	map<hustr, SmartPtr<element> > elem; //ʹ������ָ��

};

class element_manager_for_ele
{
public:


	virtual void AddElement(const char * name, element * ele)
	{

		if (GetElementByName(name) == NULL)
		{
			elem[name] = ele;
		}
		else
		{
			log_i("Repeat to add element %s\r\n", name);
		}

	}
	void ClearElement()
	{
		elem.clear();
	}
	 void DelElement(const char * name);

	virtual element * GetElementByName(const char * name)
	{
		iterator it;
		it = elem.find(name);
		if (it != elem.end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}
	typedef map<hustr,element* >::iterator iterator;
	iterator begin()
	{
		return elem.begin();
	}
	iterator end()
	{
		return elem.end();
	}
	element_manager_for_ele()
	{
	}
	virtual ~element_manager_for_ele()
	{
	}
	map<hustr, element * > elem; //ʹ������ָ��

};
/******************************
 * 最元素的最基本类
 * 仅提供显示区域的x,y及高度，宽度
 *
 */
class window{
public:
	int x;       //元素的位置是相当父元素的
	int y;
	int width;
	int height;
	int hide;
	window(){
		x = 0;
		y = 0;
		hide=0;
		height = 0;
		width = 0;
	}
	virtual ~window(){};
};
/*****************************
 * ele_nest_extend 此类为了扩展元素嵌套功能独立出来的。如果需要实现控件嵌套，元素必须继承它
 *
 */
class ele_nest_extend:virtual public window, virtual public element_manager_for_ele, virtual public Mutex{
public:

	ele_nest_extend(){
		abs_x=0;//触摸的位置是相对屏幕绝对的
		abs_y=0;
		x_page_num=1; //方向页数
		y_page_num=1;
		scroll_x=0; //卷轴x，窗口处在当前内容的位置 ，用于内容比窗口大的元素
	    scroll_y=0;
	    render_offset_x=0;//以下四个参数实现元素部分输出到绘图容器，实现控件的部分刷新
	    render_offset_y=0;
		render_width=0;
		render_height=0;
		parent=NULL;
		is_parent=false;
		//children_x_lock = 0;
		//children_y_lock = 0;
		children_touch_lock=0;
		render_cached=0;
		isDraw=0;
	}
	~ele_nest_extend(){
		//Destroy();
	}


	void tobeParent(const char * name,element * son){ //调此函数会添加一个儿子
		if(!is_parent){  //还不是父亲
			top_image.SetBuffer(width*x_page_num,height*y_page_num);//成为父亲你得想有一个家
			is_parent=true;
		}
		AddElement( name, son);
	}
	bool isParent() const{
		return is_parent;
	}
	bool hasParent()const{
		return parent!=NULL;
	}
	void configChildAbsPos();

	 virtual void onAbsPosChanged(){  //用于刷新触摸

	 }
	 void delChildren();

	void Draw(image *src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y){
		lock();
		render_offset_x=dst_x%width;//偏移位置不能超过页面宽度
		render_offset_y=dst_y%height;
		render_width=cp_width;
		render_height=cp_height;
		top_image.AreaCopy(src_img, src_x, src_y, cp_width, cp_height, dst_x, dst_y);//控件输出到父控件
		isDraw++;
		unlock();
	}
	void resetRenderOffset(){
		render_offset_x=0;
		render_offset_y=0;
		render_width=width;
		render_height=height;
	}


	void resetChildren();

	void Destroy(){
		ClearElement();
	}


	int abs_x;//触摸的位置是相对屏幕绝对的
	int abs_y;
	int x_page_num; //方向页数
	int y_page_num;
	int scroll_x; //卷轴x，窗口处在当前内容的位置 ，用于内容比窗口大的元素
	int scroll_y;
	int render_offset_x;//以下四个参数实现元素部分输出到绘图容器，实现控件的部分刷新
	int render_offset_y;
	int render_width;
	int render_height;
	//int children_x_lock;
	//int children_y_lock;
	int children_touch_lock;
	int isDraw;
	int render_cached;  //输出到父元素后，父元素不立即render
	element * parent;

	image top_image;//元素的最高一层，专门用来绘制子空间





private:
	bool is_parent;
};




class element:virtual public window,public image,public schedule_ele, virtual public ele_nest_extend, virtual public Mutex //元素本身也是元素管理者
{
public:


	virtual void doFlushConfig()=0;
	virtual void doFlushConfigReduced(){};
	virtual void doRender() = 0;
//	virtual void doRender(){
//		if(!elem.empty())//有子元素，则需要输出子元素
//		image::Render(&out, 0, 0, width, height, 0, 0);
//	}
	virtual void doGetInfo(info & info)
	{
		//log_i("warning element bash doGetInfo\r\n");
	}
	virtual void doDelete()
	{
		log_i("warning element bash OnDelete\r\n");

	}

	void GetElementInfo(info & info)
	{
		info.AddInfo("name", name);
		info.AddInfo("x", x);
		info.AddInfo("y", y);
		info.AddInfo("width", width);
		info.AddInfo("height", height);
		info.AddInfo("hide", hide);
		info.AddInfo("lay", lay);
	}
	void GetInfo(info & info)
	{
		lock();
		doGetInfo(info);
		unlock();
	}
	void Delete();
	void DeleteByParent();//父控件删除时，调用此函数删除子元素
	void onSchedule()
	{
		log_i("$$$HU$$$ Render_layer::[%s]\r\n", name.c_str());
		RenderOut();
	}


	void Flush()
	{
		if (mgr != NULL)
		{
			lock();
			//resetRenderOffset();//父控件主动绘制时恢复整个控件输出
			mgr->que.addele(this);
			unlock();
		}
		else
		{
			errexitf("$$$$HU$$$$ Flush element %s manager is NULL\r\n",
					name.c_str());
		}
	}

	void revocation()
	{
		if (mgr != NULL)
		{
			lock();
			mgr->que.delele(this);
			unlock();
		}
		else
		{
			errexitf("$$$$HU$$$$ revocation element %s manager is NULL\r\n",
					name.c_str());
		}
	}



	void RenderOut();
	void cleanLastPos();

	void FlushConfig();
	void FlushConfigReduced();

	void ParseModifRes();
	void PraseElement();
/*********************************
 * for sdcfg
 * called by FlushConfigReduced()
 */
	void rePraseElement();


	void ModifXmlMap(HUMap &mp)
	{
		lock();
		HUMap::iterator it;
		for (it = mp.begin(); it != mp.end(); ++it)
		{
			m_mp[it.key().c_str()] = it.value().getvalue();
		}
		FlushConfig();
		unlock();
	}

	void GetPartialConfig(HUMap &mp)
	{
		lock();
		HUMap::iterator it;
		for (it = mp.begin(); it != mp.end(); ++it)
		{
			m_mp[it.key().c_str()] = it.value().getvalue();
		}
		FlushConfigReduced();
		unlock();
	}
	void ResetLayers()
	{
		if (!layers.empty())
		{
			list<element *>::iterator it;
			element * ele ;
			for (it = layers.begin(); it != layers.end(); ++it)
			{
				ele = *it;
				if(*it!=this)
				ele->layers.remove(this);
			}
		}
	}


	void renderLayers();


	class Cmpare
	{
	public:
		bool operator()(const element * st1, const element * st2) const
		{
			return st1->lay < st2->lay;
		}
	};
	void initstack();





 inline	void addLayers(element * ele)
	{
		list<element *>::iterator it;
		for (it = layers.begin(); it != layers.end(); ++it)
		{
			if (*it == ele)
			{
				break;
			}
		}
		if (it == layers.end())
		{
			//log_i("$$$HU$$$ [%s] add [%s] ET\r\n",name.c_str(),ele->name.c_str());
			layers.push_back(ele);
			layers.sort(Cmpare());
		}
	}

	void delLayers(element * ele)
	{
		layers.remove(ele);
	}

	void SetRes(int id, const char * path)
	{
		if (res[id].path != path || res[id].isNULL())
		{
			//log_i("SetRes %d %s\r\n",id,path);
			res[id].SetResource(path);
		}
	}


	 element()
	{
		lay = 0;
		mgr = NULL;
		//cur_res=NULL;//默认没有资源需要绘制
		hide_lay=-1;
		//prender_res.clear();
	}

	virtual ~element()
	{
		log_i("###HU### distroy element %s\r\n", name.c_str());
		//backstack();
		ResetLayers();
		prender_res.clear();
		map<int, image>::iterator it;
		for (it = res.begin(); it != res.end(); ++it)
		{
			it->second.destroy();
		}
	}

	hustr name;
	int lay;
	int hide_lay;//向对此元素此层隐藏
	HUMap m_mp;
	xmlproc * xml_mgr;
	map<int, image> res;
	//image * cur_res;
	map<int, image*> prender_res;//由原来的单一指针改为指针map，可以在doRender里面同时绘制几层图
	schedule_draw * mgr;
	list<element *> layers;     //底顶合一队列


};








#endif //__LAYER_H__