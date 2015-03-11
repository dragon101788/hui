#ifndef __LAYER_H__
#define __LAYER_H__

#include "ParaseXML.h"
#include "codec.h"
#include "schedule.h"
#include <deque>
#include <set>
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
			printf("Repeat to add element %s\r\n", name);
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
//			printf("can't del layer element %s\r\n", name);
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


class element: public schedule_ele, public image,public element_manager, virtual public Mutex //元素本身也是元素管理者
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
		printf("warning element bash doGetInfo\r\n");
	}
	virtual void doDelete()
	{
		printf("warning element bash OnDelete\r\n");
		if(parent!=NULL){
			parent->DelElement(name.c_str());  //从父元素的子元素列表里面删除自己
		}
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
	void Delete()
	{
		doDelete();
		revocation();
		hide = 1;
		Render();
		ResetEB();
	}
	void onSchedule()
	{
		debug("$$$HU$$$ Render_layer::[%s]\r\n", name.c_str());
		if(parent!=NULL)
		debug(" parent is %s !!!!!!!!!!1\n",parent->name.c_str());
		Render();
		//debug("$$$HU$$$ Render_layer::[%s]OK\r\n", name.c_str());
	}
	 element()
	{
		hide = 0;
		x = 0;
		y = 0;
		height = 0;
		width = 0;
		lay = 0;
		mgr = NULL;
		is_father=false;

		//RegistdoFlushConfig(element);
	}

	virtual ~element()
	{
		debug("###HU### distroy element %s\r\n", name.c_str());
		backstack();
		map<int, image>::iterator it;
		for (it = res.begin(); it != res.end(); ++it)
		{
			it->second.destroy();
		}
	}
	void Flush();
	void revocation();

	void Render();
	void Back();

	void FlushConfig();
	void FlushConfigReduced();

	void ParseModifRes();
	void PraseElement()
	{
		name = m_mp["name"]->getvalue();
		int tmpX = m_mp["x"]->getvalue_int();
		int tmpY = m_mp["y"]->getvalue_int();
		width = m_mp["width"]->getvalue_int();
		height = m_mp["height"]->getvalue_int();
		hide = m_mp["hide"]->getvalue_int();

		//控件被移动
		if (tmpX != x || tmpY != y)
		{
			Back();
			x = tmpX;
			y = tmpY;
		}
		debug(
				"$$$HU$$$ ElementPrase %s x=%d y=%d width=%d height=%d hide=%d\r\n",
				name.c_str(), x, y, width, height, hide);

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
			//printf("%s SetBuffer width=%d height=%d\r\n", name.c_str(), width, height);
			SetBuffer(width, height);
			path.format("ele-%s %dx%d", name.c_str(), width, height);
		}


		initstack();

	}
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
	void ResetEB()
	{
		if (!eb.empty())
		{
			list<element *>::iterator it;
			element * ele ;
			for (it = eb.begin(); it != eb.end(); ++it)
			{
				ele = *it;
				ele->et.remove(this);
			}
		}
	}
	void RenderEB()
	{
		if (!eb.empty())
		{
			list<element *>::iterator it;
			element * ele;
			int s_ofx ; //源x
			int d_ofx ; //目标x
			int s_ofy ; //源x
			int d_ofy ; //目标x
			for (it = eb.begin(); it != eb.end(); ++it)
			{
				ele = *it;
				if (ele->hide == 0)
				{
					//printf("$$$HU$$$ RenderEB %s <-- %s\r\n", name.c_str(), ele->name.c_str());

					 s_ofx = 0; //源x
					 d_ofx = 0; //目标x
					if (ele->x < x)
					{
						s_ofx = x - ele->x;
						d_ofx = 0;
					}
					else if (ele->x > x)
					{
						s_ofx = 0;
						//d_ofx = width - (x + width - ele->x);
						d_ofx = ele->x - x;
					}

					 s_ofy = 0; //源x
					 d_ofy = 0; //目标x
					if (ele->y < y)
					{
						s_ofy = y - ele->y;
						d_ofy = 0;
					}
					else if (ele->y > y)
					{
						s_ofy = 0;
						//d_ofy = height - (y + height - ele->y);
						d_ofy = ele->y - y;
					}

					AreaCopy(ele, s_ofx, s_ofy, width, height, d_ofx, d_ofy);
				}
				//RollBack::RollBackBlock(*it, x, y, width, height);
				//(*it)->Render();
			}
		}
	}
	void RenderET()
	{
		if (!et.empty())
		{
			list<element *>::iterator it;
			element * ele;
			for (it = et.begin(); it != et.end(); ++it)
			{
				 ele = *it;
				if (ele->hide == 0)
				{
					//printf("$$$HU$$$ RenderET %s <-- %s\r\n", name.c_str(), ele->name.c_str());
					ele->Render();
				}
			}
		}

	}
	class Cmpare
	{
	public:
		bool operator()(const element * st1, const element * st2) const
		{
			return st1->lay < st2->lay;
		}
	};
	void initstack();
	void addet(element * ele)
	{
		list<element *>::iterator it;
		for (it = et.begin(); it != et.end(); ++it)
		{
			if (*it == ele)
			{
				break;
			}
		}
		if (it == et.end())
		{
			//printf("$$$HU$$$ [%s] add [%s] ET\r\n",name.c_str(),ele->name.c_str());
			et.push_back(ele);
			et.sort(Cmpare());
		}
	}
	void addeb(element * ele)
	{
		list<element *>::iterator it;
		for (it = eb.begin(); it != eb.end(); ++it)
		{
			if (*it == ele)
			{
				break;
			}
		}
		if (it == eb.end())
		{
			//printf("$$$HU$$$ [%s] add [%s] EB\r\n",name.c_str(),ele->name.c_str());
			eb.push_back(ele);
			eb.sort(Cmpare());
		}
	}
	void delet(element * ele)
	{
		et.remove(ele);
	}
	void deleb(element * ele)
	{
		eb.remove(ele);
	}
	void backstack()
	{
		list<element *>::iterator it;
		for (it = eb.begin(); it != eb.end(); ++it)
		{
			(*it)->delet(this);
		}

		for (it = et.begin(); it != et.end(); ++it)
		{
			(*it)->deleb(this);
		}
	}
	void SetRes(int id, const char * path)
	{
		if (res[id].path != path || res[id].isNULL())
		{
			//printf("SetRes %d %s\r\n",id,path);
			res[id].SetResource(path);
		}
	}
	void tobeFather(const char * name,element * son){ //调此函数会添加一个儿子
		if(!is_father){  //还不是父亲
			out.SetBuffer(width,height);//成为父亲你得想有一个家
			is_father=true;
		}
		AddElement( name, son);
	}
	bool isFather(){
		return is_father;
	}
	bool hasFather(){
		return parent!=NULL;
	}

	hustr name;
	int hide;
	int x;
	int y;
	int width;
	int height;
	int lay;
	HUMap m_mp;
	element * parent;

	xmlproc * xml_mgr;
	map<int, image> res;
	image  out;  //用于子元素将自己绘制到此处
	schedule_draw * mgr;
	list<element *> et;					//�ϲ�ؼ�
	list<element *> eb;					//�ײ�ؼ�

	protected:
	bool is_father;

};








#endif //__LAYER_H__