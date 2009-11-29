#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg16(0x8,latch_reg & UNIF_PRGMASK16);
	mem_setprg16(0xC,0xFF);
}

static void reset(int hard)
{
	latch_init(sync);
	mem_setvram8(0,0);
}

MAPPER_UNIF(nes_unrom,"NES-UNROM",reset,0,0,latch_state,INFO(128,0,0,0,8,0));
MAPPER_UNIF(hvc_unrom,"HVC-UNROM",reset,0,0,latch_state,INFO(128,0,0,0,8,0));
MAPPER_UNIF(konami_unrom,"KONAMI-UNROM",reset,0,0,latch_state,INFO(128,0,0,0,8,0));

MAPPER_UNIF(nes_uorom,"NES-UOROM",reset,0,0,latch_state,INFO(256,0,0,0,8,0));
MAPPER_UNIF(hvc_uorom,"HVC-UOROM",reset,0,0,latch_state,INFO(256,0,0,0,8,0));
MAPPER_UNIF(jaleco_uorom,"JALECO-UOROM",reset,0,0,latch_state,INFO(256,0,0,0,8,0));


