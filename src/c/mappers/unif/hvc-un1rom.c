#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg16(0x8,(latch_reg >> 2) & UNIF_PRGMASK16);
	mem_setprg16(0xC,3);
}

static void reset(int hard)
{
	latch_init(sync);
	mem_setvram8(0,0);
}

MAPPER_UNIF(hvc_un1rom,"HVC-UN1ROM",reset,0,0,latch_state,INFO(128,0,0,0,8,0));
