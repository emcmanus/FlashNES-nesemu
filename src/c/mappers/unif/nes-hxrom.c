#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static void reset(int hard)
{
	mmc3_init(mmc3_sync);
}

MAPPER_UNIF(nes_hkrom,"NES-HKROM",reset,0,mmc3_line,mmc3_state,INFO(512,256,0,0,0,0));
