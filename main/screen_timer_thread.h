#ifndef __TIMER_THREAD_H__
#define __TIMER_THREAD_H__


#include "hulib.h"
#include"main/screen_handler.h"

extern int go;
#define DELAY 10000//1ms
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
			if(handler!=NULL){
			lock();
			handler->onTimer();
			unlock();
			usleep(delay);
			}else{
				usleep(DELAY);
			}


		}
		log_i("timer thread exit\r\n");
	}

	void SwitchProc(ScreenHandler * hlr,unsigned int period)
	{
		if(handler==hlr)
			return;
		lock();//锁是必须的，不然run（）会崩溃
		delay=period;
		handler=hlr;
		unlock();
		if(handler!=NULL){
			if(!isRuning())
				create();
		}

	}

};

#endif //__STATIC_IMAGE_H__
