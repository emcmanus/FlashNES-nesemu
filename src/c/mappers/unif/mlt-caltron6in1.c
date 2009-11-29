#include "mappers/mapper.h"
#include "mappers/boards/caltron.h"

MAPPER_UNIF(mlt_caltron6in1,"MLT-Caltron6in1",
	caltron_init,0,0,caltron_state,
	INFO(256,128,0,0,0,0)
);
