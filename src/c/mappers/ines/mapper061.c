#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/chips/latch.h"

static void sync()
{
	if(latch_addr & 0x10) {
		mem_setprg16(0x8,((latch_addr & 0xF) << 1) | ((latch_addr & 0x20) >> 5));
		mem_setprg16(0xC,((latch_addr & 0xF) << 1) | ((latch_addr & 0x20) >> 5));
	}
	else
		mem_setprg32(8,latch_addr & 0xF);
	if(latch_addr & 0x80)
		ppu_setmirroring(MIRROR_H);
	else
		ppu_setmirroring(MIRROR_V);
}

static void reset(int hard)
{
	latch_init(sync);
	mem_setvram8(0,0);
}

MAPPER_INES(61,reset,0,0,latch_state);
