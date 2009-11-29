#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg16(0x8,0);
	mem_setprg16(0xC,latch_reg);
	mem_setvram8(0,0);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_INES(180,reset,0,0,latch_state);
