//#include "touch_ektf2k.h"
#include "manager_touch.h"
#include <tslib.h>


struct tsdev *ts=NULL;
char *tsdevice=NULL;

int TouchInit()
{
	if( (tsdevice = getenv("TSLIB_TSDEVICE")) != NULL )
	{
		ts = ts_open(tsdevice,0);
	}
	else
	{
		if (!(ts = ts_open("/dev/input/event0", 0)))
		{
			log_e("-----touch open failed----\n");
			ts = ts_open("/dev/touchscreen/ucb1x00", 0);
		}
	}
	
	if (!ts) {
		perror("ts_open");
		exit(1);
	}
	
    if (ts_config(ts))
	{	
        perror("ts_config");
        exit(1);
    }
	return 0;
}
int GetTouchSample(touch_sample * samp)
{
	struct ts_sample ts_samp;
	int ret;
	
	while(1)
	{
		log_e("read----------\n");
		ret = ts_read(ts, &ts_samp, 1);
		log_e("samp->x=%d----\n",samp->x);
		if (ret < 0)
		{
			perror("ts_read");
			exit(1);
		}
        if (ret == 1)
        	break;
	}
	samp->x = ts_samp.x;
	samp->y = ts_samp.y;
	samp->pressure = ts_samp.pressure;
//	debug("x=%d y=%d p=%d\n", samp->x, samp->y, samp->pressure);

}
