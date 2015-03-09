#include "cartoon.h"

//XMLinstan["cartoon"] = Install_Element<cartoon>;
static InstallXMLinstan install("cartoon",Install_Element<cartoon>);

hucall int init(HUMap & mp,void * data)
{
	debug("module init\r\n");
}
