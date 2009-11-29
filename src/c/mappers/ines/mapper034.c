#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg32(8,latch_reg);
	if(nes->rom->chrsize) {
		mem_setchr4(0,latch_reg);
		mem_setchr4(4,latch_reg);
	}
	else
		mem_setvram8(0,0);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_INES(34,reset,0,0,latch_state);
