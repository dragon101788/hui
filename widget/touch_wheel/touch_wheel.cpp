#include "touch_wheel.h"




//XMLinstan["static_text"] = Install_Element<static_text>;
static InstallXMLinstan install1("touch_wheel",Install_Element<touch_wheel>);


hucall int init(HUMap & mp,void * data)
{
	log_i("module init\r\n");
}
