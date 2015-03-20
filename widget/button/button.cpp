#include "button.h"


//XMLinstan["button"] = Install_Element<button>;
static InstallXMLinstan install("button",Install_Element<button>);


hucall int init(HUMap & mp,void * data)
{
	log_i("module init\r\n");
}
