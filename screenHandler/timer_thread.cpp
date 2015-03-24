#ifndef __SETTING_GUANJIA_H__
#define __SETTING_GUANJIA_H__

#include "main/screenHandler.h"
#include "hulib.h"

#define DELAY 1000//1ms
class ScreenTimerThread: public thread,private Mutex
{
	int delay;
public:
	ScreenHandler * handler;
	ScreenTimerThread(){
		name = "timer_thread";
		handler=NULL;
	}
	~ScreenTimerThread(){
	}
	int run()
	{

		while (go)
		{
			lock();
			if(handler!=NULL){
			handler->onTimer();
			usleep(delay);
			}else{
				usleep(DELAY);
			}
			unlock();

		}
		log_i("timer thread exit\r\n");
	}

	void SwitchProc(ScreenHandler * hlr,unsigned int period)
	{
		lock();
		handler=hlr;
		delay=period;
		if(handler!=NULL){
			if(!isRuning())
				create();
		}
		unlock();
	}

};

#endif //__STATIC_IMAGE_H__
