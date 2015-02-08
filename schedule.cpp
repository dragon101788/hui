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
	while (1)
	{
		 ele = que.getele();
		if (ele == NULL)
		{
			break;
		}
		else
		{
			ele->onSchedule();
			ret++;
		}
	}
	return ret;
}
