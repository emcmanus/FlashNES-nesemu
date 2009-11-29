#include "mappers/mapper.h"
#include "mappers/chips/fme7.h"

MAPPER_UNIF(sunsoft_fme_7,"SUNSOFT-FME-7",
	fme7_init,0,fme7_line,fme7_state,
	INFO(256,256,0,0,0,0)
);
