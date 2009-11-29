#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg32(8,latch_addr & 0xFF);
	mem_setchr8(0,latch_addr & 0xFF);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_INES(201,reset,0,0,latch_state);

