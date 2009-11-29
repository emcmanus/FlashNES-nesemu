#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static void sync()
{
	mem_setprg16(0x8,(latch_reg >> 4) & 7);
	mem_setchr8(0,latch_reg & 0xF);
	if(latch_reg & 0x80)
		ppu_setmirroring(MIRROR_1H);
	else
		ppu_setmirroring(MIRROR_1L);
}

static void reset(int hard)
{
	latch_init(sync);
	mem_setprg16(0xC,7);
}

MAPPER_INES(70,reset,0,0,latch_state);
