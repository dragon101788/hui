#include "hulib.h"
#include <linux/input.h>
#include "debug.h"
#include "keypad.h"

 int keyFd;


int getKyes(struct input_event ev_key);
int keypadInit()
{
 	 keyFd = open("/dev/event0", 666);
 	if (keyFd < 0)
	{
  		log_e("open device event");
  		exit(1);
 	}
 	return 0;
}




//template<class T>
//void  getKeyEvent(T t)
////void  getKeyEvent(T *t)
//{
//	static struct input_event ev_key;
//	int count = read(touchFd,&ev_key,sizeof(struct input_event));
//
//	for(int i=0; i<(int)count/sizeof(struct input_event); i++)
//	{
//			//p(getKyes(ev_key));
//		//t;
//	}
//}

int getKyes(struct input_event ev_key){
	switch (ev_key.type)
						{
							case EV_KEY:
								switch (ev_key.code) {
									case BTN_TOUCH:
									//if (ev_key.value == 0)
									//	pen_up = 1;
									break;
									case KEY_F1:
										return 1;
									break;
									case KEY_F2:
										return 2;
									break;
									case KEY_F3:
										return 3;
									break;
									case KEY_F4:
										return 4;
									break;
									case KEY_F5:
										return 5;
									break;
									case KEY_F6:
										return 6;
									break;
									case KEY_F7:
										return 7;
									break;
									case KEY_F8:
										return 8;
									break;
									case KEY_F9:
										return 9;
									break;
									case KEY_F10:
										return 10;
									break;
									case KEY_F11:
										return 11;
									break;
									case KEY_F12:
										return 12;
									break;
									case KEY_F13:
										return 13;
									break;
									case KEY_F14:
										//printf("KEY_F14 pressed !!!");
										return 14;
									break;
									case KEY_F15:
									//	printf("KEY_F15 pressed !!!");
										return 15;
									break;
								}
							break;


						}
	return 0;
}




