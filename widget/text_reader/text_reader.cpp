#include "text_reader.h"
hucall int init(HUMap & mp,void * data)
{
	log_i("module init\r\n");
}
//	XMLinstan["Radio_buttons"] = Install_Element<Radio_buttons>;
static InstallXMLinstan install("text_reader",Install_Element<text_reader>);

