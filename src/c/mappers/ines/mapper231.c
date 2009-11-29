#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static void sync()
{
	u8 prg = (latch_addr & 0x1E) | ((latch_addr >> 5) & 1);

	mem_setprg16(0x8,prg & 0x1E);
	mem_setprg16(0xC,prg);
	mem_setvram8(0,0);
	ppu_setmirroring(((latch_addr >> 7) & 1) ^ 1);
}

static void reset(int hard)
{
	latch_init(sync);
}

MAPPER_INES(231,reset,0,0,latch_state);

