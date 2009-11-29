#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static void sync()
{
	if(latch_addr & 0x40) {
		mem_setprg16(0x8,latch_addr & 7);
		mem_setprg16(0xC,latch_addr & 7);
	}
	else
		mem_setprg32(8,(latch_addr & 7) >> 1);
	if(nes->rom->chrsize)
		mem_setchr8(0,(latch_addr >> 3) & 7);
	ppu_setmirroring(((latch_addr & 0x80) >> 7) ^ 1);
}

static void reset(int hard)
{
	latch_init(sync);
	if(nes->rom->chrsize == 0) {
		nes_setvramsize(1);
		mem_setvram8(0,0);
	}
}

MAPPER_INES(58,reset,0,0,latch_state);
