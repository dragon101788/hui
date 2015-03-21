#include "manager_touch.h"

 int touch_element::GetTouchX()
{
	return touch_mgr->cur_samp.x;
}
 int touch_element::GetTouchY()
{
	return touch_mgr->cur_samp.y;
}
 int touch_element::GetTouchP()
{
	return touch_mgr->cur_samp.pressure;
}

int touch_manager::touch_proc_event(touch_sample * samp)
{
	if (cur_samp.x == samp->x &&cur_samp.y == samp->y&&cur_samp.pressure == samp->pressure)
	{				//过滤重复坐标
		return 0;
	}
	lock();
	cur_samp = *samp;
	if(cur_samp.pressure!=0){ //用于通知用户触摸屏是否按下
		in_touch=1;
		onGlobalTouchPressed();
	}else{
		in_touch=0;
		onGlobalTouchReleased();
	}
	iterator it;
	for (it = mp.begin(); it != mp.end(); ++it)
	{
		touch_element * toe = (*it);
		int lock=0;
		if(toe->hasParent()){
			//children_touch_lock在父控件里面经常动态改变，比如滑动时锁定，所以lock得动态赋值，不能在解析时赋值
			lock=toe->touch_lock |toe->parent->children_touch_lock;
		}
		if (lock==0)
		//if (toe->touch_lock == 0)
		{
			if (toe->isArea(cur_samp.x, cur_samp.y)
					&& cur_samp.pressure != 0)
			{
				toe->touch_area();
				toe->isdn = 1;
			}
			else
			{
				if (toe->isdn == 1)
				{
					toe->free_area();
					toe->isdn = 0;
				}
			}
		}
		else
		{
			//如果锁住了，并且处于按下状态，那么弹起按钮
			if (toe->isdn == 1)
			{
				toe->free_area();
				toe->isdn = 0;
			}

		}
	}
	unlock();

}
