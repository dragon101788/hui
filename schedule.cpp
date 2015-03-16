#include "schedule.h"
#include "layer.h"

//schedule_draw g_schedule_draw;

void schedule_draw::del_elemgr(element_manager * mgr)
{
	que.lock();
	element_manager::iterator it;
	for (it = mgr->begin(); it != mgr->end(); ++it)
	{
		schedule_ele * ele = it->second;
		que.delele(ele);
	}
	que.unlock();
}

int schedule_draw::ScheduleProc()
{
	if (stop)
		return 0;

	int ret = 0;
	schedule_ele * ele;
      list<schedule_ele *>::iterator it;

      for (it = que.m_list.begin();; ++it)
		{
			 ele = *it;
			if (ele == NULL)
			{
				que.clean();  //队列在最后清除，是防止绘制一个删掉后又会被重复添加
				break;
			}
			ele->onSchedule();
			ret++;
		}

	return ret;
}
