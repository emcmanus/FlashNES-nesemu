#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static void sync()
{
	mem_setprg16(0x8,latch_reg & 7);
	mem_setprg16(0xC,latch_reg & 7);
	mem_setchr8(0,latch_reg & 7);
	ppu_setmirroring(((latch_reg >> 3) & 1) ^ 1);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_INES(200,reset,0,0,latch_state);

