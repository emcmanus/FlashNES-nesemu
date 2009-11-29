#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setvram4(4,latch_reg & 3);
}

static void init(int hard)
{
	latch_init(sync);
	mem_setprg32(8,0);
	mem_setvram8(0,0);
	sync();
}

MAPPER_INES(13,init,0,0,latch_state);
