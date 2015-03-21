#ifndef __THREAD_KEYPAD_H__
#define __THREAD_KEYPAD_H__

#include "hulib.h"
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include "thread_msg.h"
#include "autoconf.h"
#include"platform/keypad.h"
//extern touch_manager g_touch_mgr;
extern int go;


class KeypadThread:public thread, private Mutex

{


	int run()
	{
		while (go)
		{

				getKeyEvent(m_scrhandler);
		}
		log_i("touch thread exit\r\n");
	}




	void init()
	{
		int ret = keypadInit();
		if (ret == 0)
		{
			create();
		}
	}

	ScreenHandler * m_scrhandler;
public:

	void SwitchProc(ScreenHandler * handler)
	{
		lock();
		m_scrhandler = handler;
		unlock();
	}

	KeypadThread()
	{
		name = "keypad_thread";

	}




};

extern KeypadThread g_th_keypad;

#endif //__THREAD_TOUCH_H__
