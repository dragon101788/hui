#include "slip_fragment.h"


hucall int init(HUMap & mp,void * data)
{
	printf("module init\r\n");
}

//	XMLinstan["Radio_buttons"] = Install_Element<Radio_buttons>;
static InstallXMLinstan install("slip_fragment",Install_Element<slip_fragment>);

