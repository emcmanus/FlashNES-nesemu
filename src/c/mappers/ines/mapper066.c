#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg32(8,(latch_reg & 0x30) >> 4);
	mem_setchr8(0,latch_reg & 3);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_INES(66,reset,0,0,latch_state);
