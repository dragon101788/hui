#ifndef __XML_INSTAL_H__
#define __XML_INSTAL_H__

#include "hulib.h"
#include "xmlproc.h"
#include <map>
using namespace std;

typedef void (*XMLinstan_tf)(hustr parentName,HUMap & xmlmp, xmlproc * xml);
extern map<hustr, XMLinstan_tf> XMLinstan;

template<class T>
void Install_Element(hustr parentName,HUMap &xmlmp, xmlproc * xml)
{
	//xml->mtx.lock();
	//log_i("parent name=%s !!!!\n",parentName.c_str());
	const char * name = xmlmp["name"]->getvalue();
	element * ele;

	element_manager::iterator it;
	for (it = xml->begin(); it != xml->end(); ++it)
	{
		 ele = it->second;
		if(ele->name==name){
			log_i("element %s has already added!!!\n",name);
			return ;
		}
	}
	T * te = new T;
	te->m_mp.fetch(xmlmp);
	te->xml_mgr = xml;
	te->mgr = xml;
	if(parentName!=NULL){
		element * parent;
		parent=xml->GetElementByName(parentName.c_str());
		if(parent!=NULL){
			te->parent=parent;
			parent->tobeParent(name,te);
			//parent->AddElement(name, te);  //父元素添加子元素
		}
	}
	te->FlushConfig();
	//xml->mtx.unlock();


}
/****************************************
 * 逻辑本地化处理
 * 一个页面里面添加一个window_Control 控件用于控制元素动作
 */
template<class T>
void install_page_control(hustr parentName,HUMap &xmlmp, xmlproc * xml)
{
	if(xml->windCtl!=NULL){
		return ;//一个页面只能有一个页面控制器
	}
	T * te = new T;
	te->m_mp.fetch(xmlmp);
	te->viewManager = xml;
	te->FlushConfig();
	xml->windCtl=te;

}

class InstallXMLinstan
{
public:
	InstallXMLinstan(const char * name, XMLinstan_tf fun)
	{
		XMLinstan[name] = fun;
	}
};
#define InstallXMLinstanPageCtl(a,b) static InstallXMLinstan install##_##b(a,install_page_control<b>)
#define InstallXMLinstanWidget(a,b) static InstallXMLinstan install##_##b(a,Install_Element<b>)
#ifdef HUMODULE
#define hucall extern "C"
#else
#define hucall static
#endif

#endif //__XML_INSTAL_H__
