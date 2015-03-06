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
	debug("parent name=%s !!!!\n",parentName.c_str());
	const char * name = xmlmp["name"]->getvalue();
	element * ele;
	element_manager::iterator it;
	for (it = xml->begin(); it != xml->end(); ++it)
	{
		 ele = it->second;
		if(ele->name==name){
			printf("element %s has already added!!!\n",name);
			return ;
		}
	}
	T * te = new T;
	te->m_mp.fetch(xmlmp);
	te->xml_mgr = xml;
	te->mgr = xml;
	if(parentName!=NULL)
	te->parent=xml->GetElementByName(parentName.c_str());
	te->FlushConfig();
	//xml->mtx.unlock();


}



class InstallXMLinstan
{
public:
	InstallXMLinstan(const char * name, XMLinstan_tf fun)
	{
		XMLinstan[name] = fun;
	}
};

#define InstallXMLinstanWidget(a,b) static InstallXMLinstan install##_##b(a,Install_Element<b>);
#ifdef HUMODULE
#define hucall extern "C"
#else
#define hucall static
#endif

#endif //__XML_INSTAL_H__
