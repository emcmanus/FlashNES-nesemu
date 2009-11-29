#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg16(0x8,latch_reg);
	mem_setprg16(0xC,0xFF);
	mem_setvram8(0,0);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_INES(2,reset,0,0,latch_state);
