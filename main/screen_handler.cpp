#include "screen_handler.h"
#include "screenHandler/timer_thread.h"
#include "xmlproc.h"

DirectProcTimer g_dirctExec;
void ScreenHandler::setTimer(ScreenTimerThread *thread,unsigned int period){
	thread->SwitchProc(this,period);
}

	BaseView * ScreenHandler::findViewByName(const char *name){
		BaseView * view=(BaseView *)viewManager->GetElementByName(name);
		if(view==NULL){
			log_e("************************ERROR***************************\n");
			log_e("%s view %s name is incorrect,please check in page!!\n",this->name.c_str(),name);
			log_e("******************************************************** \n");
			exit(-1);
		}
		return view;
	}
