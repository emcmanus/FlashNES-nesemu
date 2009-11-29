#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg16(0x8,0);
	mem_setprg16(0xC,0xFF);
	mem_setvram8(0,0);
	if(latch_reg & 0xF) {
		if(latch_reg != 0x13)
			mem_setchr8(0,0);
	}
	//need to disable writes to vram...
}

static void reset(int hard)
{
	int i;

	nes_setvramsize(2);
	for(i=0;i<KB(8);i++)
		nes->rom->vram[i] = 0x12;
	latch_init(sync);
}

MAPPER_INES(185,reset,0,0,latch_state);
