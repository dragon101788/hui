#include "manager_timer.h"

inline int timer_element::GetUpTimer()
{
	return timer_mgr->GetUpTimer();
}
void timer_manager::UpdateTimer()
{
	int cur = GetUpTimer();

	lock();
	iterator it;
	for (it = mp.begin(); it != mp.end();)
	{
		//log_i("UpdateTimer %dms %dms\r\n", cur, (*it)->m_tm);
		if (cur > (*it)->m_tm && (*it)->timer_stop==0)
		{
			int ret = (*it)->Timer(cur);
			if (ret == TIMER_DEL_ELE)
			{
				delete *it;
				mp.erase(it++);
				continue;
			}

		}
		++it;

	}
	unlock();
}
