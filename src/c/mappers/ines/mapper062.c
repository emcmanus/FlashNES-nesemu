#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static void sync()
{
	u8 prg = ((latch_addr >> 8) & 0x3F) | (latch_addr & 0x40);
	u8 chr = (latch_reg & 3) | ((latch_addr & 0x1F) << 2);

	if(latch_addr & 0x20) {
		mem_setprg16(0x8,prg);
		mem_setprg16(0xC,prg);
	}
	else
		mem_setprg32(8,prg >> 1);
	mem_setchr8(0,chr);
	if(latch_addr & 0x80)
		ppu_setmirroring(MIRROR_H);
	else
		ppu_setmirroring(MIRROR_V);
}

static void reset(int hard)
{
	latch_init(sync);
	nes_setvramsize(1);
	nes_setsramsize(2);
	mem_setvram8(0,0);
	mem_setsram8(6,0);
}

MAPPER_INES(62,reset,0,0,latch_state);
