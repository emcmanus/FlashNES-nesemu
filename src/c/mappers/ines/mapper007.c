#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static void sync()
{
	mem_setprg32(8,latch_reg & 0xF);
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

MAPPER_INES(7,reset,0,0,latch_state);
