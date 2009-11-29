#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg32(8,latch_reg & 0xF);
	mem_setchr8(0,(latch_reg >> 4) & 0xF);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_UNIF(colordreams_74377,"COLORDREAMS-74*377",reset,0,0,latch_state,INFO(128,0,0,0,8,0));
