#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static u8 reg,prg,chr;

static void sync()
{
	switch(latch_reg & 0xC0) {
		case 0x00:
			switch(reg & 0xC0) {
				case 0x40: chr = reg & 0xF; break;
				case 0x80: prg = reg & 0xF; break;
			}
			break;
	}
	reg = latch_reg;
	mem_setprg16(0x8,0);
	mem_setprg16(0xC,prg);
	mem_setchr8(0,chr);
}

static void reset(int hard)
{
	reg = 0;
	prg = 0;
	chr = 0;
	latch_init(sync);
}

MAPPER_INES(92,reset,0,0,latch_state);
