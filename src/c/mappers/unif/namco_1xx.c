#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/namco_106.h"
#include "mappers/chips/mmc3.h"

MAPPER_UNIF(namco_106,"NAMCO-106",
	namco_106_init,0,namco_106_line,namco_106_state,
	INFO(256,256,0,0,0,0)
);

static void reset(int hard)
{
	mmc3_init(mmc3_sync);
}

//namco mmc3 (an old revision of it -- less features)
MAPPER_UNIF(namco_108,"NAMCO-108",reset,0,mmc3_line,mmc3_state,INFO(256,256,0,0,0,0));
MAPPER_UNIF(namco_109,"NAMCO-109",reset,0,mmc3_line,mmc3_state,INFO(256,256,0,0,0,0));
MAPPER_UNIF(namco_118,"NAMCO-118",reset,0,mmc3_line,mmc3_state,INFO(256,256,0,0,0,0));
