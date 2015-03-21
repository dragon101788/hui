#ifndef __KEYPAD_LW_H__
#define __KEYPAD_LW_H__
//#include "screenHandler.h"


extern int getKyes(struct input_event ev_key);
extern int keypadInit();
extern  int keyFd;
template<class T>
inline void  getKeyEvent(T *t)
{
	static struct input_event ev_key;
	int key;
	int count = read(keyFd,&ev_key,sizeof(struct input_event));
	for(int i=0; i<(int)count/sizeof(struct input_event); i++)
	{
		key=getKyes(ev_key);
		if(key!=0)
			t->getKey(key);
	}
}


//extern void  getKeyEvent(ScreenHandler *handler);
extern int getKyes(struct input_event ev_key);






#endif //__KEYPAD_LW_H__
