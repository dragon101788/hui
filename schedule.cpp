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
      list<schedule_ele * >  que_delet_dupli;
      list<schedule_ele *>::iterator it;
	while (1)
		{

			 ele = que.getele();

			if (ele == NULL)
			{
				break;
			}
			/*******************
			 * 添加这段是为了排除重复的元素在重复render
			 */
			for (it = que_delet_dupli.begin(); it != que_delet_dupli.end(); ++it)
			{
				if (*it == ele)
				{
					break;
				}
			}
			if (it != que_delet_dupli.end())
			{
				continue;
			}
	/*********************************/
			{
				ele->onSchedule();
				ret++;
			}
			que_delet_dupli.push_back(ele);

		}

	return ret;
}
