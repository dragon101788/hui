#include "keys_slip_menu.h"


hucall int init(HUMap & mp,void * data)
{
	log_i("module init\r\n");
}

//	XMLinstan["Radio_buttons"] = Install_Element<Radio_buttons>;
static InstallXMLinstan install("keys_slip_menu",Install_Element<keys_slip_menu>);

