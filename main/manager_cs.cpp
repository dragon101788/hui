#include "xmlproc.h"

void CS_manager::PostCS(HUMap::OrderList & mp)
{
	if (stop)
		return;

	lock();
	//mp.display();
	HUMap::OrderList::iterator it;
	for (it = mp.begin(); it != mp.end(); ++it)
	{
		if((*it).m_key=="name")
		{
			continue;
		}
		//log_i("parse %s\r\n",(*it).m_key.c_str());
		//(*it).display();
		ParseXMLElement2(NULL,(*it).m_key, (*it), elemgr);
		//log_i("CS_manager::PostCS %s\r\n", it->first.c_str());
//		for(int i=0;i<it->second.BrotherCount();i++)
//		{
//			log_i("CS_manager::PostCS %s[%d]\r\n", it->first.c_str(),i);
//			HUMap & tmp = it->second[i];
//			//tmp.display();
//			ParseXMLElement2(it->first, tmp, elemgr);
//		}

	}
	unlock();
}

void CS_manager::PostCS(const char * str)
{
	if (stop)
		return;

	lock();
	log_i("PostCS %s ++\r\n", str);
	iterator it;

	it = m_cs.find(str);
	if (it != m_cs.end())
	{
		//log_i("PostCS find %s\r\n", str);
		//it->second.display();
		//log_i("count=%d\r\n",cnt);
		PostCS(it->second);

	}
	else
	{
		log_i("PostCS can't find cs [%s]\r\n", str);
	}
	log_i("PostCS %s--\r\n", str);
	unlock();
}

void CS_manager::PostScfg(HUMap & mp)
{
	lock();
	hustr name = mp["name"]->getvalue();
	//log_i("PostCS name = %s\r\n", name.c_str());

	element * ele = elemgr->GetElementByName(name);
	if (ele != NULL)
	{
		ele->ModifXmlMap(mp);
	}
	else
	{
		log_w("PostCS can't find element [%s]\r\n", name.c_str());
	}
	unlock();
}
void CS_manager::PostPartialConfig(HUMap & mp)
{
	lock();
	hustr name = mp["name"]->getvalue();
	//log_i("PostCS name = %s\r\n", name.c_str());

	element * ele = elemgr->GetElementByName(name);
	if (ele != NULL)
	{
		ele->GetPartialConfig(mp);
	}
	else
	{
		log_i("PostConfig can't find element [%s]\r\n", name.c_str());
	}
	unlock();
}
