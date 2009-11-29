#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setchr8(0,latch_reg & UNIF_CHRMASK8);
}

static void reset(int hard)
{
	latch_init(sync);
	mem_setprg32(8,0);
}

MAPPER_UNIF(nes_cnrom,"NES-CNROM",reset,0,0,latch_state,INFO(32,32,0,0,0,0));
MAPPER_UNIF(taito_cnrom,"TAITO-CNROM",reset,0,0,latch_state,INFO(32,32,0,0,0,0));
MAPPER_UNIF(konami_cnrom,"KONAMI-CNROM",reset,0,0,latch_state,INFO(32,32,0,0,0,0));
