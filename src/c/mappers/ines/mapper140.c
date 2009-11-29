#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg32(0x8,(latch_reg >> 4) & 3);
	mem_setchr8(0,latch_reg & 0xF);
}

static void reset(int hard)
{
	mem_setwrite(6,latch_write);
	mem_setwrite(7,latch_write);
	latch_init(sync);
}

MAPPER_INES(140,reset,0,0,latch_state);

