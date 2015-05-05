#include "layer.h"
#include "xmlproc.h"
#include "ttf_font.h"
#include "include/autoconf.h"



  Mutex del_lock;

	void element::PraseElement()
	{
		name = m_mp["name"]->getvalue();
		int tmpX = m_mp["x"]->getvalue_int();
		int tmpY = m_mp["y"]->getvalue_int();
		//clean_buf=m_mp["clean"]->getvalue_int();//绘制时是否清除缓存，在只有一个层的时候很管用
		if (m_mp.exist("parentXPage"))//在父元素的第几个页面里,0开始算起
		{
			tmpX+=parent->width* m_mp["parentXPage"]->getvalue_int();
		}
		if (m_mp.exist("parentYPage"))
		{
			tmpY+=parent->height* m_mp["parentYPage"]->getvalue_int();
		}
		width = m_mp["width"]->getvalue_int();
		height = m_mp["height"]->getvalue_int();
		render_width=width;
		render_height=height;
		hide = m_mp["hide"]->getvalue_int();
		if (m_mp.exist("x_page_num"))
		{
			x_page_num = m_mp["x_page_num"]->getvalue_int(); //start from 1
		}
		if (m_mp.exist("y_page_num"))
		{
			y_page_num = m_mp["y_page_num"]->getvalue_int();
		}
		if (m_mp.exist("hide_lay"))
		{
			hide_lay= m_mp["hide_lay"]->getvalue_int();
		}
		//控件被移动
		if (tmpX != x || tmpY != y)
		{
			cleanLastPos();
			x = tmpX;
			y = tmpY;
		}
		if(hasParent()){
		abs_x=x+parent->abs_x-parent->scroll_x;
		abs_y=y+parent->abs_y-parent->scroll_y;
		}else{
			abs_x=x;
			abs_y=y;
		}
		if (m_mp.exist("lay"))
		{
			lay = m_mp["lay"]->getvalue_int();
		}
		else
		{
			lay = 5;
		}
		initstack();
	}

//	void element::rePraseElement()
//	{
//		int tmpX = m_mp["x"]->getvalue_int();
//		int tmpY = m_mp["y"]->getvalue_int();
//		if (m_mp.exist("parentXPage"))//在父元素的第几个页面里,0开始算起
//		{
//			tmpX+=parent->width* m_mp["parentXPage"]->getvalue_int();
//		}
//		if (m_mp.exist("parentYPage"))
//		{
//			tmpY+=parent->height* m_mp["parentYPage"]->getvalue_int();
//		}
//
//		hide = m_mp["hide"]->getvalue_int();
//		if (m_mp.exist("x_page_num"))
//		{
//			x_page_num = m_mp["x_page_num"]->getvalue_int();
//		}
//		if (m_mp.exist("y_page_num"))
//		{
//			y_page_num = m_mp["y_page_num"]->getvalue_int();
//		}
//		//控件被移动
//		if (tmpX != x || tmpY != y)
//		{
//			cleanLastPos();
//			x = tmpX;
//			y = tmpY;
//		}
//		if(hasParent()){
//		abs_x=x+parent->abs_x-parent->scroll_x;
//		abs_y=y+parent->abs_y-parent->scroll_y;
//		}else{
//			abs_x=x;
//			abs_y=y;
//		}
//		if (m_mp.exist("lay"))
//		{
//			lay = m_mp["lay"]->getvalue_int();
//		}
//		initstack();
//	}

	void element::renderLayers()
	{
		int cnt=0;
		if (!layers.empty())
		{
			list<element *>::iterator it;
			map<int,LayerRes>::iterator itp;
			element * ele;
			LayerRes *lay_res;
			int s_ofx ; //源x
			int d_ofx ; //目标x
			int s_ofy ; //源x
			int d_ofy ; //目标x
//			if(clean_buf){
//				cleanArea();
//			}
			for (it = layers.begin(); it != layers.end(); ++it)
			{
				ele = *it;
				if (ele->hide == 0&&ele->lay!=hide_lay)
				{
					//log_i("$$$HU$$$ RenderEB %s <-- %s\r\n", name.c_str(), ele->name.c_str());
					 s_ofx = 0; //源偏移x
					 d_ofx = render_offset_x; //目标偏移x
					if (ele->x < x+render_offset_x)
					{
						s_ofx = x+render_offset_x - ele->x;
						if(s_ofx>ele->width)
							continue;
						//d_ofx = render_offset_x;
					}
					else if (ele->x > x+render_offset_x)
					{
						s_ofx = 0;
						d_ofx = ele->x - x;
						if(d_ofx>render_offset_x+render_width)
							continue;
					}

					 s_ofy = 0; //源x
					 d_ofy = render_offset_y; //目标x
					if (ele->y < y+render_offset_y)
					{
						s_ofy = y +render_offset_y- ele->y;
						if(s_ofy>ele->height)
							continue;
						//d_ofy = 0;
					}
					else if (ele->y > y+render_offset_y)
					{
						s_ofy = 0;
						d_ofy = ele->y - y;
						if(d_ofy>render_offset_y+render_height)
							continue;

					}
					//log_i("%s render_offset_x=%d,render_offset_y=%d!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",name.c_str(),render_offset_x,render_offset_y);
					//log_i("%s -%s s_ofx=%d, s_ofy=%d, d_ofx=%d, d_ofy=%d!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",name.c_str(),ele->name.c_str(),s_ofx,s_ofy,d_ofx,d_ofy);
					if(cnt==0){//最底的元素直接复制
						for (itp = ele->render_res.begin();itp != ele->render_res.end(); itp++)
						{
							cnt++;
							lay_res=&itp->second;
							//log_i("itp->first=%d\n",itp->first);
							int final_s_off_x=s_ofx+ele->scroll_x;
							int final_s_off_y=s_ofy+ele->scroll_y;
							int final_d_off_x=d_ofx+lay_res->dst_x;
							int final_d_off_y=d_ofy+lay_res->dst_y;
							//log_i("+++++final_s_off_y=%d\n",final_s_off_y);
							if(lay_res->dst_x>=ele->width){
								int reduce=(lay_res->dst_x/ele->width)*ele->width;
								final_s_off_x-=reduce;
								final_d_off_x-=reduce;
							}
							if(lay_res->dst_y>=ele->height){
								int reduce=(lay_res->dst_y/ele->height)*ele->height;
								final_s_off_y-=reduce;
								final_d_off_y-=reduce;
							}
						//	log_i("+++++final_s_off_y=%d\n",final_s_off_y);
							if(itp == ele->render_res.begin())
								copyLayer(lay_res->img, final_s_off_x, final_s_off_y, render_width,render_height,final_d_off_x, final_d_off_y);
							else
								renderLayer(lay_res->img, final_s_off_x, final_s_off_y, render_width,render_height,final_d_off_x, final_d_off_y);
						}
						if(ele->isParent()){
							int final_s_off_x=s_ofx+ele->scroll_x;
							int final_s_off_y=s_ofy+ele->scroll_y;
							if(cnt==0){
								copyLayer(&ele->top_image,  final_s_off_x, final_s_off_y, render_width,render_height, d_ofx, d_ofy);
							}else
								renderLayer(&ele->top_image, final_s_off_x, final_s_off_y, render_width,render_height, d_ofx, d_ofy);
						}
					}
					else{
							//log_i("itp->first=%d!!!\n",itp->first);
							for (itp = ele->render_res.begin(); itp != ele->render_res.end(); itp++)
							{
								lay_res=&itp->second;
								int final_s_off_x=s_ofx+ele->scroll_x;
								int final_s_off_y=s_ofy+ele->scroll_y;
								int final_d_off_x=d_ofx+lay_res->dst_x;
								int final_d_off_y=d_ofy+lay_res->dst_y;
								//log_i("+++++final_s_off_y=%d\n",final_s_off_y);
								if(lay_res->dst_x>=ele->width){
									int reduce=(lay_res->dst_x/ele->width)*ele->width;
									final_s_off_x-=reduce;
									final_d_off_x-=reduce;
								}
								if(lay_res->dst_y>=ele->height){
									int reduce=(lay_res->dst_y/ele->height)*ele->height;
									final_s_off_y-=reduce;
									final_d_off_y-=reduce;
								}
								//log_i("+++++final_s_off_y=%d\n",final_s_off_y);
								renderLayer(lay_res->img, final_s_off_x, final_s_off_y, render_width,render_height,final_d_off_x, final_d_off_y);
							}
							if(ele->isParent()){
								int final_s_off_x=s_ofx+ele->scroll_x;
								int final_s_off_y=s_ofy+ele->scroll_y;
								renderLayer(&ele->top_image, final_s_off_x, final_s_off_y, render_width,render_height, d_ofx, d_ofy);
							}
						}
					cnt++;
				}
			}
		//	if(!cnt&&!clean_buf){ //没有队列，为了清除原状态。否则会显示上一次的状态
			if(!cnt){ //没有队列，为了清除原状态。否则会显示上一次的状态
				cleanArea();
			}
		}
	}




int crossAlgorithm(element * r1, element * r2)
{
	int nMaxLeft = 0;
	int nMaxTop = 0;
	int nMinRight = 0;
	int nMinBottom = 0;

	//计算两矩形可能的相交矩形的边界
	nMaxLeft = r1->x >= r2->x ? r1->x : r2->x;
	nMaxTop = r1->y >= r2->y ? r1->y : r2->y;
	nMinRight =
			(r1->x + r1->width) <= (r2->x + r2->width) ?
					(r1->x + r1->width) : (r2->x + r2->width);
	nMinBottom =
			(r1->y + r1->height) <= (r2->y + r2->height) ?
					(r1->y + r1->height) : (r2->y + r2->height);
	// 判断是否相交
	//log_i("crossAlgorithm %d %d %d %d\r\n",nMaxLeft , nMinRight , nMaxTop , nMinBottom);
	if (nMaxLeft >= nMinRight || nMaxTop >= nMinBottom)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


void element::initstack()
{
	element_manager::iterator it;
	for (it = xml_mgr->begin(); it != xml_mgr->end(); ++it)
	{
		element * ele = it->second;
		if(ele->parent==parent)
		{  //只有当在同一父亲下，或父亲都为0，才相互作用
			if (crossAlgorithm(ele, this))
			{
				if(ele->lay != lay){
					addLayers(ele);   //区域内有重叠元素，并且层小于自己则加入队列
					ele->addLayers(this); //将自己加入队列
				}
			}
		}
	}
	addLayers(this);  //自己也要加在自己的队列中
}

void element::RenderOut()
{
	cfgPartRender();
	if (hide == 0)
	{
		doRenderConfig();
	}
	else
	{
		log_i("Render %s hide\r\n", name.c_str());
	}
	renderLayers();  //如果自己隐藏的话，此函数是不会绘制自己的。
	if(parent!=NULL){
		parent->addDraw(x+render_offset_x,y+render_offset_y,render_width,render_height);
		if(!parent->isParent()){
			parent->tobeParent(name,this);
		}
		if(render_cached)
			parent->Flush();
		else
			parent->RenderOut();

	}else{
		xml_mgr->addDraw(x+render_offset_x,y+render_offset_y,render_width,render_height);
	}


}


void element::cleanLastPos()
{
	lock();

	renderLayers();  //如果自己隐藏的话，此函数是不会绘制自己的。
	if(parent!=NULL){
		log_i("%s draw to parent!!!!!!!\n",name.c_str());
		if(!parent->isParent()){
			parent->tobeParent(name,this);
		}
		//parent->Draw(this,  0, 0, width, height, x, y);//控件输出到父控件
		parent->RenderOut();
	}
	unlock();

}

void element::FlushConfig()
{
	lock();
	doFlushConfig();
	xml_mgr->AddElement(name, this);
	unlock();
}

//void element::FlushConfigReduced()
//{
//	lock();
//	doFlushConfigReduced();
//	xml_mgr->AddElement(name, this);
//	unlock();
//}
void element::Delete()
{
	doDelete();
	if(isParent()){
		delChildren();
	}
	revocation();
	hide = 1;
	RenderOut();
	//ResetLayers();
	//debug("Delete after ResetLayers !!\n");
	if(hasParent()){
		parent->elem.erase(name);
	}
	xml_mgr->elem.erase(name);
}


void element::DeleteByParent()
{
	doDelete();
	if(isParent()){
		delChildren();
	}
	revocation();
	xml_mgr->elem.erase(name);
}


void element::copyLayer(image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
{
	int _dst_x=dst_x+x;
	int _dst_y=dst_y+y;
	if(hasParent()){
		if(ProcArea(&parent->top_image, src_img, src_x, src_y, cp_width, cp_height, _dst_x, _dst_y,x+width,y+height))
			 return;

		::AreaCopy_no_ProcArea(&parent->top_image ,src_img,src_x, src_y, cp_width, cp_height,_dst_x,_dst_y);
		//::Copy_img_to_img(&parent->top_image ,src_img,src_x, src_y, cp_width, cp_height,_dst_x,_dst_y);
	}else{
		if(ProcArea(&xml_mgr->out, src_img, src_x, src_y, cp_width, cp_height, _dst_x, _dst_y,x+width,y+height))
			 return;
		::AreaCopy_no_ProcArea(&xml_mgr->out, src_img,src_x, src_y, cp_width, cp_height,_dst_x,_dst_y);
		//::Copy_img_to_img(&xml_mgr->out, src_img,src_x, src_y, cp_width, cp_height,_dst_x,_dst_y);
	}
}

void element::renderLayer(image * src_img, int src_x, int src_y, int cp_width, int cp_height, int dst_x, int dst_y)
{
	src_img->LoadResource();
	int _dst_x=dst_x+x;
	int _dst_y=dst_y+y;

	if(hasParent()){
		if(ProcArea(&parent->top_image, src_img, src_x, src_y, cp_width, cp_height, _dst_x, _dst_y,x+width,y+height))
			return;
		Render_img_to_img(&parent->top_image, src_img, src_x, src_y, cp_width, cp_height, _dst_x, _dst_y);

	}else{
		if(ProcArea(&xml_mgr->out, src_img, src_x, src_y, cp_width, cp_height, _dst_x, _dst_y,x+width,y+height))
			return;
		Render_img_to_img(&xml_mgr->out, src_img, src_x, src_y, cp_width, cp_height, _dst_x, _dst_y);
	}
}

void element::cleanArea()
{

	if (hasParent()){
		unsigned long src_offset=(unsigned long)parent->top_image.pSrcBuffer+(y+render_offset_y)*parent->top_image.u32Stride+(x+render_offset_x)<<2;
		int cp_size=render_width<<2;
		parent->top_image.lock();
		for(int i=0;i<render_height;i++){
			src_offset+=cp_size*i;
			if(src_offset+cp_size>parent->top_image.SrcSize){
				break;
			}
			memset((void*)src_offset, 0,cp_size);
		}
		parent->top_image.unlock();
	}else{
		unsigned long  src_offset=(unsigned long)xml_mgr->out.pSrcBuffer+(y+render_offset_y)*xml_mgr->out.u32Stride+(x+render_offset_x)<<2;
		int cp_size=render_width<<2;
		xml_mgr->lock();
		for(int i=0;i<render_height;i++){
			src_offset+=cp_size*i;
			if(src_offset+cp_size>xml_mgr->out.SrcSize){
							break;
						}
			memset((void*)src_offset, 0, cp_size);
		}
		xml_mgr->unlock();
	}
}





/*
 * 在加载图片及文字，显示在同一层
 */
void element::ParseModifRes()
{
	for (int i = 0; i < m_mp.count("res"); ++i)
	{
		HUMap  xmlmp = m_mp["res"][i];
		hustr meth = xmlmp["meth"]->getvalue();
		int id = xmlmp["id"]->getvalue_int();
		int src_x = xmlmp["src_x"]->getvalue_int();
		int src_y = xmlmp["src_y"]->getvalue_int();
		int cp_width = xmlmp["cp_width"]->getvalue_int();
		int cp_height = xmlmp["cp_height"]->getvalue_int();
		int dst_x = xmlmp["dst_x"]->getvalue_int();
		int dst_y = xmlmp["dst_y"]->getvalue_int();

		res[id].LoadResource();
		if (cp_width + cp_height == 0)
		{
			cp_width = res[id].GetWidth();
			cp_height = res[id].GetHeight();
		}
		if (meth == "render")
		{
			image file;
			file.SetResource(xmlmp["file"]->getvalue());
			file.LoadResource();

			log_i(
					"$$$HU$$$ XML Draw Render %s to %s res=%d %d %d %d %d %d %d\r\n",
					file.path.c_str(), name.c_str(), id, src_x, src_y, cp_width,
					cp_height, dst_x, dst_y);

			if (!res[id].isNULL())
			{

				//ele->image::Render(&file, src_x, src_y, cp_width, cp_height, dst_x, dst_y);
				lock();

				res[id].Render(&file, src_x, src_y, cp_width, cp_height, dst_x,
						dst_y);
				//ele->Render();
				unlock();

			}
			else
			{
				errexitf("%s res %d no init\r\n", name.c_str(), id);
			}
		}
		else if (meth == "text")
		{
#ifdef CONFIG_USING_FONT
			int red = xmlmp["red"]->getvalue_int();
			int green = xmlmp["green"]->getvalue_int();
			int blue = xmlmp["blue"]->getvalue_int();
			int color = (red & 0xff) << 16 | (green & 0xff) << 8 | blue & 0xff;
			//bkcolor = m_mp["bkcolor"]->getvalue_int();
			hustr font = xmlmp["font"]->getvalue();
			hustr txt = xmlmp["txt"]->getvalue();
			int style = (unsigned char) m_mp["style"]->getvalue_int();
			int size = xmlmp["size"]->getvalue_int();


			text tmpttf;
			tmpttf.m_font = &font_mp[font];
			log_i("get font_mp %s %x %x\r\n",font.nstr() ,tmpttf.m_font->face,
					tmpttf.m_font->ft_Lib);
			tmpttf.fontHeight = size;
			tmpttf.color = color;
			tmpttf.style = style;
			tmpttf.SetBuffer(cp_width, cp_height);
			tmpttf.DrawText( (char *) txt.c_str(), txt.length());

			log_i("ParseModifRes text=%s\r\n",txt.c_str());
			if (!res[id].isNULL())
			{

				//ele->image::Render(&file, src_x, src_y, cp_width, cp_height, dst_x, dst_y);
				lock();

				res[id].Render(&tmpttf, src_x, src_y, cp_width, cp_height,
						dst_x, dst_y);
				//ele->Render();
				unlock();

			}
#endif
		}

	}
	m_mp.erase("res");
}


 void element_manager::DelElement(const char * name)
{
	element * ele = GetElementByName(name);
	if (ele == NULL)
	{
		log_w("can't del layer element %s\r\n", name);
	}
	else
	{
		ele->Delete();
	//	elem.erase(name);
	}
}
 void element_manager_for_ele::DelElement(const char * name)
{
	element * ele = GetElementByName(name);
	if (ele == NULL)
	{
		log_w("can't del layer element %s\r\n", name);
	}
	else
	{
		ele->Delete();
	//	elem.erase(name);
	}
}


 void ele_nest_extend::configChildAbsPos(){  //当父控件的scroll_x改变时，子控件的绝对位置就会改变，父控件需要调用此函数
	 iterator it;
	 element * ele;
		for (it = elem.begin(); it != elem.end(); ++it)
		{
			ele=it->second;
			ele->abs_x=ele->x+abs_x-scroll_x;
			ele->abs_y=ele->y+abs_y-scroll_y;
			ele->onAbsPosChanged();
		}

 }
 void ele_nest_extend::delChildren(){
	iterator it;

	for (it = elem.begin(); it != elem.end(); ++it)
	{
		element *ele=it->second;
		if(ele!=NULL)
		ele->DeleteByParent();
	}
 }
	void ele_nest_extend::resetChildren()
	{
		iterator it;
		element * ele;
		for(it=elem.begin();it!=elem.end();++it){
			ele=it->second;
			if(ele!=NULL){
				ele->parent=NULL;
			}
		}
	}
