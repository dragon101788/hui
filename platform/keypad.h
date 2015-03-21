#ifndef __KEYPAD_LW_H__
#define __KEYPAD_LW_H__
#include "screenHandler.h"


extern int getKyes(struct input_event ev_key);
extern int keypadInit();

extern void  getKeyEvent(ScreenHandler *handler);
extern int getKyes(struct input_event ev_key);






#endif //__KEYPAD_LW_H__
