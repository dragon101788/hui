#ifndef __PAGE_CONTROL_H__
#define __PAGE_CONTROL_H__



#include "ParaseXML.h"
#include "codec.h"
#include "schedule.h"
//#include "thread_keypad.h"
#include <deque>
#include <set>



class BaseView;
class xmlproc;

class EventListener{
public:
	 EventListener(){

	}
	virtual ~EventListener(){

	}

	virtual void onDrag(const char *ele_name){

	}
	virtual void onTouchDown(const char *ele_name){
		//log_s("eventListener  doTouchDown\n");
	}
	virtual void onTouchUp(const char *ele_name){

	}
	virtual void onTouchActive(const char *ele_name){

	}
	virtual void onTimer(const char *ele_name,int time){

	}
};


class ScreenHandler{
	public:

	ScreenHandler(){
		isRunning=0;
	}
	virtual ~ScreenHandler(){
		deleteThis();
	}

	void getKeyValue(int key_value){
		onKeyPressed(key_value);
	}


	/******************
	 * 加载完成
	 */
	 void loadDone(){
		 onLoadDone();
	}
	 void deleteThis(){
		 onDelete();
	}
	/******************
	 * 切换到前台
	 */
	 void coming(const char *lastFile){
		 isRunning=0;
		onComing(lastFile);
		//onResume();
	}
	/******************
	 * 切换到后台
	 */
	 void leaving(){
		// onPause();
		 isRunning=1;
		onLeaving();

	}



	virtual void onDelete(){

	}
	/***********************
	 *
	 * 加载完成，页面必须实现
	 */
	virtual void onLoadDone()=0;
	/******************
	 * 切换到前台，页面必须实现
	 */
	virtual void onComing(const char * lastFile)=0;
	/******************
	 * 切换到后台，页面必须实现
	 */
	virtual void onLeaving()=0;
	//按键回调函数
	virtual void onKeyPressed(int key){
		log_s("key_pressed value=%d!!!\n",key);
	}
//	/******************
//	 * 暂停
//	 */
//	 void pause(){
//		 isRunning=0;
//		 onPause();
//	}
//	/******************
//	 * 恢复
//	 */
//	void resume(){
//		isRunning=1;
//		onResume();
//
//	}
//	/******************
//	 * 暂停
//	 */
//	virtual void onPause(){
//
//	}
//	/******************
//	 * 恢复
//	 */
//	virtual void onResume(){
//	}

//	virtual void onTouchOn(){
//
//	}
//	virtual void onTouchUp(){
//
//	}


	virtual void doFlushConfig(){
		//name = m_mp["name"]->getvalue();
	}
	void FlushConfig(){

		doFlushConfig();

	}
	void gotoPage(const char * xml){

	}
	bool isRun() const{
		return isRunning;
	}
	BaseView * findViewByName(const char *name);

	bool isRunning;
	xmlproc *viewManager;
	HUMap m_mp;
	hustr name;
};











#endif //__LAYER_H__