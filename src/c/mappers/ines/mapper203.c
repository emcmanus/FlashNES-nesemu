#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg16(0x8,latch_reg >> 2);
	mem_setprg16(0xC,latch_reg >> 2);
	mem_setchr8(0,latch_reg & 3);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_INES(203,reset,0,0,latch_state);
