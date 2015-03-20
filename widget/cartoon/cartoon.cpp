#include "cartoon.h"

//XMLinstan["cartoon"] = Install_Element<cartoon>;
static InstallXMLinstan install("cartoon",Install_Element<cartoon>);

hucall int init(HUMap & mp,void * data)
{
	log_i("module init\r\n");
}
