#include "screenHandler.h"
#include "xmlproc.h"

DirectProcTimer g_dirctExec;
	BaseView * ScreenHandler::findViewByName(const char *name){
		BaseView * view=(BaseView *)viewManager->GetElementByName(name);
		if(view==NULL){
			log_e("!!!!!!!!!!name is incorrect!!!!!please check in page %s \n",this->name.c_str());
			exit(-1);
		}
		return view;
	}
