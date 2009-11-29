#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg32(8,latch_reg & 0x1F);
}

static void reset(int hard)
{
	latch_init(sync);
	nes_setvramsize(1);
	nes_setsramsize(2);
	mem_setvram8(0,0);
	mem_setsram8(6,0);
}

MAPPER_INES(39,reset,0,0,latch_state);
