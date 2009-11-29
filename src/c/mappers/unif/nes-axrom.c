#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static void sync()
{
	mem_setprg32(8,latch_reg & UNIF_PRGMASK32);
	if(latch_reg & 0x10)
		ppu_setmirroring(MIRROR_1H);
	else
		ppu_setmirroring(MIRROR_1L);
}

static void reset(int hard)
{
	latch_init(sync);
	mem_setvram8(0,0);
}

MAPPER_UNIF(nes_amrom, "NES-AMROM", reset,0,0,latch_state,INFO(128,0,0,0,8,0));
MAPPER_UNIF(nes_anrom, "NES-ANROM", reset,0,0,latch_state,INFO(128,0,0,0,8,0));
MAPPER_UNIF(nes_an1rom,"NES-AN1ROM",reset,0,0,latch_state,INFO(64, 0,0,0,8,0));
MAPPER_UNIF(nes_aorom, "NES-AOROM", reset,0,0,latch_state,INFO(256,0,0,0,8,0));

