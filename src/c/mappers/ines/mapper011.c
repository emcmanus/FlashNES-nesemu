#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg32(8,latch_reg & 0xF);
	mem_setchr8(0,(latch_reg >> 4) & 0xF);
}

static void init(int hard)
{
	latch_init(sync);
}

MAPPER_INES(11,init,0,0,latch_state);
