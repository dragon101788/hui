#include "radio_bottons.h"

//	XMLinstan["Radio_buttons"] = Install_Element<Radio_buttons>;
static InstallXMLinstan install("Radio_buttons",Install_Element<Radio_buttons>);


hucall int init(HUMap & mp,void * data)
{
	log_i("module init\r\n");
}
