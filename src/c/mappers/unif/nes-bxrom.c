#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg32(8,latch_reg);
	mem_setvram8(0,0);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_UNIF(nes_bnrom,"NES-BNROM",reset,0,0,latch_state,INFO(128,0,0,0,8,0));
MAPPER_UNIF(irem_bnrom,"IREM-BNROM",reset,0,0,latch_state,INFO(128,0,0,0,8,0));
