#include "ParseXML.h"
#include "XMLInstal.h"
#include <errno.h>
#include "tinyxml/tinyxml.h"
#include "xmlproc.h"


int ParseXMLElement2(hustr parentName,hustr name, HUMap & xmlmp, xmlproc * xml)
{
	log_i("$$$HU$$$ Parse [%s]\r\n", name.c_str());

	//xmlmp.display();
	XMLinstan_tf fun = XMLinstan[name];
	if (fun != NULL)
	{
		//xml->done = 0;
		fun(parentName,xmlmp, xml);  //调用Install_Element(HUMap &xmlmp, xmlproc * xml)
		//xml->done = 1;
	}
	else
	{
		log_i("$$$$$HU$$$$$$$warning :: %s is not an element or cmd \r\n", name.c_str());
	}
}

void setXmlData(HUMap & xmlmp,HUMap  *setData){
	string final_name = xmlmp["name"]->getvalue();
	final_name+=(*setData)["SN"]->getvalue();

	int x=xmlmp["x"]->getvalue_int();
	x+=(*setData)["offset_x"]->getvalue_int();
	int y=xmlmp["y"]->getvalue_int();
	y+=(*setData)["offset_y"]->getvalue_int();
	//log_i("final_name=%s,x=%d,y=%d!!!!!!!!!!!!!!!!!!!!!!!!!\n",final_name.c_str(),x,y);
	xmlmp["name"]->format(final_name.c_str());
	xmlmp["x"]->format("%d",x);
	xmlmp["y"]->format("%d",y);
//为文本框重新赋值
	if ((*setData).exist("txt"))
		xmlmp["txt"]->format((*setData)["txt"]->getvalue());
//为按钮图片重新赋值
	if ((*setData).exist("up"))
		xmlmp["up"]->format((*setData)["up"]->getvalue());
	if ((*setData).exist("dn"))
		xmlmp["dn"]->format((*setData)["dn"]->getvalue());
//为图片重新赋值
	for (int i = 0; i < (*setData).count("node"); i++)
	{
		xmlmp["node"][i]->format((*setData)["node"][i]->getvalue());
	}
}

int ParseXMLElementWithData(hustr parentName,hustr name,HUMap & xmlmp, xmlproc * xml,HUMap  *setData)
{
	log_i("$$$HU$$$ Parse [%s]\r\n", name.c_str());

	//xmlmp.display();
	XMLinstan_tf fun = XMLinstan[name];

	if (fun != NULL)
	{
		//xml->done = 0;
		setXmlData(xmlmp,setData);
		fun(parentName,xmlmp, xml);  //调用Install_Element(HUMap &xmlmp, xmlproc * xml)
		//xml->done = 1;
	}
	else
	{
		log_i("$$$$$HU$$$$$$$warning :: %s is not an element or cmd \r\n", name.c_str());
	}
}


int ParseXMLElementWithFamilyName(hustr parentName,hustr SN,hustr name,HUMap & xmlmp, xmlproc * xml)
{
	log_i("$$$HU$$$ Parse [%s]\r\n", name.c_str());

	//xmlmp.display();
	XMLinstan_tf fun = XMLinstan[name];



	if (fun != NULL)
	{
		//xml->done = 0;
		string final_name =xmlmp["name"]->getvalue()+SN;
		xmlmp["name"]->format(final_name.c_str());
		fun(parentName,xmlmp, xml);  //调用Install_Element(HUMap &xmlmp, xmlproc * xml)
		//xml->done = 1;
	}
	else
	{
		log_i("$$$$$HU$$$$$$$warning :: %s is not an element or cmd \r\n", name.c_str());
	}
}





void ParaseTinyAttribute2(TiXmlNode* pchild, HUMap & xmlmp)
{
	TiXmlAttribute* attr = pchild->ToElement()->FirstAttribute();
	if (attr)
	{
		TiXmlNode* node = pchild;
		while (node)
		{
			while (attr)
			{

				xmlmp[attr->Name()] = attr->Value();
				attr = attr->Next();
			}

			node = node->NextSiblingElement();
		}

	}
}

void ParaseUpdateXml2(TiXmlNode* pParent, HUMap & xmlmp)
{
	if (pParent == NULL)
	{
		return;
	}

	TiXmlNode* pchild = pParent->FirstChild();

	while (pchild)
	{
		int t = pchild->Type();

		if (t == TiXmlNode::TINYXML_COMMENT)
		{
			pchild = pchild->NextSibling();
			continue;
		}

		HUMap &mp = xmlmp.CreateMultiLast(pchild->Value());

		ParaseTinyAttribute2(pchild, mp);

		if (pchild->FirstChild() != NULL && pchild->FirstChild()->Type() == TiXmlNode::TINYXML_TEXT)
		{
			mp.m_val = pchild->FirstChild()->Value();
		}

		if (pchild->FirstChild() == NULL || pchild->FirstChild()->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			ParaseUpdateXml2(pchild, mp);
		}

		pchild = pchild->NextSibling();
	}

}

int ParseXMLElement2(hustr parentName,hustr name, HUMap & xmlmp, xmlproc * xml);
void ParaseUpdateXml3(hustr parentName,TiXmlNode* pParent, xmlproc * xml)
{
	if (pParent == NULL)
	{
		return;
	}

	TiXmlNode* pchild = pParent->FirstChild();

	while (pchild)
	{
		int t = pchild->Type();

		if (t == TiXmlNode::TINYXML_COMMENT)
		{
			pchild = pchild->NextSibling();
			continue;
		}

		const char * name = pchild->Value();
		HUMap mp(name);

		ParaseTinyAttribute2(pchild, mp);

		if (pchild->FirstChild() != NULL && pchild->FirstChild()->Type() == TiXmlNode::TINYXML_TEXT)
		{
			mp.m_val = pchild->FirstChild()->Value();
		}

		if (pchild->FirstChild() == NULL || pchild->FirstChild()->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			ParaseUpdateXml2(pchild, mp);
		}



		ParseXMLElement2(parentName,name, mp, xml);
		//added by zhangtian this is a ele name ,not ele type
		hustr  parent_name = mp["name"]->getvalue();
		//added by zhangtian for element nest
		if (pchild->FirstChild() != NULL && pchild->FirstChild()->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			ParaseUpdateXml3(parent_name,pchild, xml);//用于元素嵌套
		}
		pchild = pchild->NextSibling();
	}

}
void ParaseUpdateXmlWithFamilyName(hustr parentName,hustr familyName,TiXmlNode* pParent, xmlproc * xml)
{
	if (pParent == NULL)
	{
		return;
	}

	TiXmlNode* pchild = pParent->FirstChild();

	while (pchild)
	{
		int t = pchild->Type();

		if (t == TiXmlNode::TINYXML_COMMENT)
		{
			pchild = pchild->NextSibling();
			continue;
		}

		const char * name = pchild->Value();
		HUMap mp(name);

		ParaseTinyAttribute2(pchild, mp);

		if (pchild->FirstChild() != NULL && pchild->FirstChild()->Type() == TiXmlNode::TINYXML_TEXT)
		{
			mp.m_val = pchild->FirstChild()->Value();
		}

		if (pchild->FirstChild() == NULL || pchild->FirstChild()->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			ParaseUpdateXml2(pchild, mp);
		}



		ParseXMLElementWithFamilyName(parentName,familyName,name, mp, xml);
		//added by zhangtian this is a ele name ,not ele type
		hustr  parent_name = mp["name"]->getvalue();
		//added by zhangtian for element nest
		if (pchild->FirstChild() != NULL && pchild->FirstChild()->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			ParaseUpdateXmlWithFamilyName(parent_name,familyName,pchild, xml);//用于元素嵌套
		}
		pchild = pchild->NextSibling();
	}

}

//添加SN 主要用于list_view这种需要适配器的高级控件，元素名称不能事先给定
void ParaseUpdateXmlWithData(hustr parentName,TiXmlNode* pParent, xmlproc * xml,listMap &setData)
{
	if (pParent == NULL)
	{
		return;
	}

	TiXmlNode* pchild = pParent->FirstChild();

	int cnt=0;
	while (pchild)
	{
		int t = pchild->Type();

		if (t == TiXmlNode::TINYXML_COMMENT)
		{
			pchild = pchild->NextSibling();
			continue;
		}

		const char * name = pchild->Value();
		HUMap mp(name);

		ParaseTinyAttribute2(pchild, mp);

		if (pchild->FirstChild() != NULL && pchild->FirstChild()->Type() == TiXmlNode::TINYXML_TEXT)
		{
			mp.m_val = pchild->FirstChild()->Value();
		}

		if (pchild->FirstChild() == NULL || pchild->FirstChild()->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			ParaseUpdateXml2(pchild, mp);
		}

		ParseXMLElementWithData(parentName,name, mp, xml,setData[cnt]);
		//added by zhangtian this is a ele name ,not ele type
		hustr  parent_name = mp["name"]->getvalue();
		//added by zhangtian for element nest
		if (pchild->FirstChild() != NULL && pchild->FirstChild()->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			HUMap &father=*(setData[cnt]);
			hustr SN=father["SN"]->getvalue();
			ParaseUpdateXmlWithFamilyName(parent_name,SN,pchild, xml);//用于元素嵌套
		}
		pchild = pchild->NextSibling();
		cnt++;
	}

}

void ParaseTinyXmlFile(const char * file, xmlproc * xml)
{
	TiXmlDocument doc;
	if (!doc.LoadFile(file))
	{
		errexitf("ParaseTinyXmlFile LoadFile %s %s \r\n		TiXmlDocument error :%s\r\n", xml->filename.c_str(), strerror(errno),
				doc.ErrorDesc());
	}

	TiXmlElement * root = doc.RootElement();

	if (!root)
	{
		return;
	}

	ParaseUpdateXml3(NULL,root, xml);

}

