#include "list_view.h"


hucall int init(HUMap & mp,void * data)
{
	log_i("module init\r\n");
}

//	XMLinstan["Radio_buttons"] = Install_Element<Radio_buttons>;
static InstallXMLinstan install("list_view",Install_Element<list_view>);

