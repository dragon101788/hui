#ifndef __TIMER_THREAD_H__
#define __TIMER_THREAD_H__


#include "hulib.h"
#include"main/screen_handler.h"

extern int go;
#define DELAY 1000//1ms
class ScreenHandler;
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
		lock();//锁是必须的，不然run（）会崩溃
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
