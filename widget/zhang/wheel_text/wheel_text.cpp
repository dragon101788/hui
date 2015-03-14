#include "wheel_text.h"




//XMLinstan["static_text"] = Install_Element<static_text>;
static InstallXMLinstan install1("wheel_text",Install_Element<wheel_text>);


hucall int init(HUMap & mp,void * data)
{
	printf("module init\r\n");
}
