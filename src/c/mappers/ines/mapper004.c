#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static void reset(int hard)
{
	mmc3_init(mmc3_sync);
}

MAPPER_INES(4,reset,0,mmc3_line,mmc3_state);
