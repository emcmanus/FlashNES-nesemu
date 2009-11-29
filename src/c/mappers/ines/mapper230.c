#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reg[2],mode = 0;

static void sync()
{
	if(mode == 0) {
		mem_setprg16(0x8,reg[0] & 7);
		mem_setprg16(0xC,7);
		ppu_setmirroring(MIRROR_V);
	}
	else {
		if(reg[1] & 0x20) {
			mem_setprg16(0x8,(reg[1] & 0x1F) + 8);
			mem_setprg16(0xC,(reg[1] & 0x1F) + 8);
		}
		else
			mem_setprg32(8,((reg[1] & 0x1F) >> 1) + 4);
		ppu_setmirroring((reg[1] & 0x40) ? MIRROR_V : MIRROR_H);
	}
}

static void write_upper(u32 addr,u8 data)
{
	reg[mode] = data;
	sync();
}

static void reset(int hard)
{
	int i;

	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	if(hard)
		mode = 1;
	else
		mode ^= 1;
	nes_setvramsize(1);
	mem_setvram8(0,0);
	reg[0] = reg[1] = 0;
	sync();
}

MAPPER_INES(230,reset,0,0,0);

