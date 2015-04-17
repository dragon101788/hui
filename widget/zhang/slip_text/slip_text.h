#ifndef __STATIC_IMAGE_H__
#define __STATIC_IMAGE_H__

#include "XMLInstal.h"
#include "view.h"


	static hustr font;     //保存路径
	static unsigned int color;
	static unsigned char style;
	static int size;

class slip_text: public View
{


private:




	int getFileSize(char * strFileName)
	{
	    FILE * fp = fopen(strFileName, "r");
	    fseek(fp, 0L, SEEK_END);
	    int size = ftell(fp);
	    fclose(fp);
	    return size;
	}
	typedef struct page_node    //页链表
	{
		text ttf;
		struct page_node *next;
		struct page_node *prior;
		inline page_node(){
			ttf.m_font = &font_mp[font];
			ttf.fontHeight = size;
			ttf.color = color;
			ttf.style = style;
		}
	}page_node,*page_list;


	int init_list(page_list &L,int w,int h)   //初始化单链表
	{
		L=new page_node;//(page_list)malloc(sizeof(page));   //表头附加结点
		if(!L)
		{
		log_i("L malloc mem fialed!!!!!!\n");
		 exit(-2);
		}
		L->ttf.SetBuffer(w,h);
		L->next=NULL;
		L->prior=NULL;
		return 1;
	}//初始化了一个一个节点的双向链表
	page_list list_find_last(page_list L)
	{
	page_list last=L;
	while(last->next!=NULL)
	{
	last=last->next;
	}
	return last;
	}

	page_list list_find_head(page_list L)
	{
	page_list head=L;
	while(head->prior!=NULL)
	{
	head=head->prior;
	}
	return head;
	}

	int list_how_far_last(page_list L)
	{
	page_list last=L;
	int num=0;
	while(last->next!=NULL)
	{
	num++;
	last=last->next;
	}
	return num;
	}

	int list_how_far_head(page_list L)
	{
	page_list head=L;
	int num=0;
	while(head->prior!=NULL)
	{
	num++;
	//log_i("list_how_far_head:num=%d!!!!\n",num);
	head=head->prior;
	}
	return num;
	}

	void destroy_list(page_list &L)
	{
	page_list i,head,last;
	head=list_find_head(L);
	last=list_find_last(L);
	for(i=head;i!=last->next;i=i->next)
	{
	//       free(i->ttf.pSrcBuffer);
	       delete i;//free(head);//删除原来的头
	}
	}


	int list_add_next_delete_prior(page_list &L,int w,int h)  //前面删一个，后面加一个，往下滑动
	{
		int i=0,j=0;
		page_list last,head;
		last=L;//当前的最后一个
		while(last->next!=NULL)
		{
			i++;
			last=last->next;
		}
		head=L;//当前的第一个
		while(head->prior!=NULL)
		{
			j++;
			head=head->prior;
		}

		page_list newpage=new page_node;//(page_list)malloc(sizeof(page));
		newpage->ttf.SetBuffer(w,h);
		last->next=newpage;
		newpage->prior=last;
		newpage->next=NULL;
	//	log_i("i=%d;j=%d!!!\n",i,j);
		if(j>=3)
			{
			head->next->prior=NULL;
		//	free(head->ttf.pSrcBuffer);
		//	log_i("delete head!!!!\r\n");
			delete head;//free(head);//删除原来的头
			}
		return 0;
	}

	int list_add_prior_delete_next(page_list &L,int w,int h)
	{
		int i=0,j=0;
		page_list last,head;//当前的最后一个
		last=L;//当前的最后一个
		while(last->next!=NULL)
		{
			i++;
			last=last->next;
		}
		head=L;//当前的第一个
		while(head->prior!=NULL)
		{
			j++;
			head=head->prior;
		}

		//log_i("list_add_prior_delete_next: i=%d;j=%d!!!\n",i,j);
		page_list newpage=new page_node;//(page_list)malloc(sizeof(page));
	        newpage->ttf.SetBuffer(w,h);
		head->prior=newpage;
		newpage->next=head;
		newpage->prior=NULL;
		if(i>=4)
			{
			last->prior->next=NULL;
		//	free(last->ttf.pSrcBuffer);
		//	log_i("delete last!!!\n");
			delete last;//free(last);//删除原来的尾
			}
		return 0;
	}





public:



	void doDelete()
	{

		xml_mgr->DelTimerElement(this);
		xml_mgr->DelTouchElement(this);
	}
	void doTouchDown()
	{
		Flush();
	}

	void doTouchUp()
	{
		sx -= move_x();
		sy -= move_y();
	
		if (sy<0)
		{
			if(page==1)
			sy = 0;
		}
		       
		else  if(sy>0)
		 {
			if(page==const_page)
			sy=0;
			else if(sy>=height)//向下换页
			{
				if(page==const_page-1)
					sy=height;
			}		
	
		}
		if(flag_set_timer)//在抬手处处理定时任务目的在于解决滑动卡顿
		{
			TimerSet(0);
			flag_set_timer=0;
		}
		log_i("sy=%d!!\n",sy);
		Flush();
	}

	int doTimer(int tm)
	{	
		log_i("doTimer \n");
		if(cache_down)//当sy大于height时，准备缓存下一页
		{     
			if(page<const_page-1) 
			{
				//ttf.cleanBuf();
				if(print_num<txt_len)
				{
					 list_add_next_delete_prior(mylist,width,height);
					 page_list last=list_find_last(mylist);
					 if(word_num_mp[page+1]!=0){
						 print_num=word_num_mp[page+1];
					 }
					 print_num+=last->ttf.DrawText(buffer+print_num,txt_len);
					 word_num_mp[page+2]=print_num;


                		//print_num+=ttf.DrawText_unicode((wchar_t *) WStr+word_num_mp[page+2], lenth_once);

				if(print_num>=txt_len)
					const_page=page+2;
				}
			}
			cache_down=0;
		}

		else if(cache_up)//当sy小于height时，准备缓存上一页
		{
			if(page!=1)
			{			
				list_add_prior_delete_next(mylist,width,height);
				page_list head=list_find_head(mylist);
				print_num=word_num_mp[page-2];
				print_num+=head->ttf.DrawText(buffer+print_num,txt_len);


			}
			cache_up=0;
		}

		TimerStop();
	}
	void doTouchActive()
	{
		
	

	}
	void doRenderConfig()
	{
		int py = sy - move_y();
		log_i("sy=%d,py=%d!!\n",sy,py);
		if (py<0)
		{
			if(page==1)
			py = 0;
			else  //向上换页
			{
				py+=height;
				sy+=height;
				mylist=mylist->prior;
			//	{
				page--;
				log_i("page--,page=%d\n!!!!!!! ",page);
				if(list_how_far_head(mylist)==0)//下面只剩一个缓存页面
				{
					while(cache_up) log_i("wait for cache_up clean!!!!!!1\n");//等待上一次的缓存完成
					cache_up=1;
					//log_i("start cache up\n!!!!!\n");
					flag_set_timer=1;
				//	TimerSet(0);
				}
			        	
			}
		}
		       
		else  if(py>0)
		 {
			if(page==const_page)
			py=0;
			else if(py>=height)//向下换页
			{
			//	page++;
				if(page==const_page-1)
				py=height;
				else
				{
					page++;
					py-=height;
					sy-=height;
					mylist=mylist->next;
					log_i("page++,page=%d\n!!!!!!! ",page);
					if(list_how_far_last(mylist)==1)//下面只剩一个缓存页面
					{
						while(cache_down) log_i("wait for cache_down clean!!!!!!!\n");
						cache_down=1;
						//log_i("start cache_down!!!!!!!!\n");
						//TimerSet(0);
						flag_set_timer=1;
					}
				}
					
			}		
	
		}
		log_i("page=%d,const_page=%d,print_num=%d,txt_len=%d,sy=%d,py=%d!!\n",page,const_page,print_num,txt_len,sy,py);

		render_res[0].translate(move_x()-sx,-py);
		render_res[0].img=&mylist->ttf;
		if(const_page>1){
			render_res[1].translate(move_x()-sx,height-py);
			render_res[1].img=&mylist->next->ttf;
		}
		else
			render_res.erase(1);

//
//		image::Render(&mylist->ttf, sx - move_x(), py, width, height-py, 0, 0);//page start from 0
//		if(const_page>1)
//		image::Render(&mylist->next->ttf, sx - move_x(), 0, width,py, 0, height-py);
	
	}

	void doFlushConfig()
	{
		PraseElement();
		TouchParaseXml(m_mp);
		color = m_mp["color"]->getvalue_hex();
		font = m_mp["font"]->getvalue();
		txt_name = m_mp["file"]->getvalue();
		txt_len = getFileSize((char*)txt_name.c_str());
		if((txt_file=fopen(txt_name.c_str(),"r+"))==NULL)	
		{	
			log_i("Cannot open output file.\n");
		}
			log_i("txt_len=%d!!!!!!!!!!!!!!!!\n",txt_len);
		buffer=(char *)malloc(txt_len);//记得free()
		//txt=buffer;
		fread((char *)buffer,1,txt_len,txt_file);
		log_i("fread end!!!");
		style = (unsigned char) m_mp["style"]->getvalue_int();
		size = m_mp["size"]->getvalue_int();
		int col_num=height/(size*1.2);

		init_list(mylist,width,height);//初始化一个有三个元素的链表

		word_num_mp[0]=print_num;
        //print_num+=ttf.DrawText_unicode((wchar_t *) WStr+print_num, lenth_once);
        print_num+=mylist->ttf.DrawText(buffer+print_num,txt_len);
		word_num_mp[1]=print_num;
		const_page=100000;
		if(print_num>=txt_len)  //只有一页
		const_page=1;
		else
		{
			list_add_next_delete_prior(mylist,width,height);
			print_num+=mylist->next->ttf.DrawText(buffer+print_num,txt_len);
			word_num_mp[2]=print_num;//第3页开始字数
			if(print_num>=txt_len)
				const_page=2;
			else{
				list_add_next_delete_prior(mylist,width,height);
				print_num+=mylist->next->next->ttf.DrawText(buffer+print_num,txt_len);
				word_num_mp[4]=print_num;
				if(print_num>=txt_len)
					const_page=3;
			}
		}

		log_i("11111111114\n");
		touch_init_area(x, y, width, height);
		xml_mgr->AddEleArea(this);
		xml_mgr->AddTimerElement( this);
		Flush();
	}

	slip_text()
	{
		sx = 0;
		sy = 0;
		font=NULL;     //保存路径
		txt=NULL;      //i
		style=0;
		size=0;
		lenth=0;
		lenth_final=0;
        page=1;
        const_page=1;
		cache_up =0;
		cache_down =0;
		flag_set_timer=0;
		print_num=0;
	}
	~slip_text()
	{
		fclose(txt_file);
		free(buffer);
		destroy_list(mylist);
		word_num_mp.clear();
	//	delete mylist;

	}


	int sx;
	int sy;

	hustr txt;      //i
	char * buffer;      //保存文本

	hustr txt_name;


	int lenth;
	int lenth_final; //最后一页缓存字数
    int page;
	int const_page;
	int cache_up; 
	int cache_down;
	FILE *txt_file;
	int txt_len;
    page_list mylist;
	unsigned long print_num;
	int flag_set_timer;
	map<int,unsigned long> word_num_mp;	
};

#endif //__STATIC_IMAGE_H__
