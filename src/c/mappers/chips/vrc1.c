#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg[3],chr[2],mirror;

static void sync()
{
	mem_setprg16(0x8,prg[0]);
	mem_setprg16(0xA,prg[1]);
	mem_setprg16(0xC,prg[2]);
	mem_setprg16(0xE,0xF);
	mem_setchr4(0,chr[0]);
	mem_setchr4(4,chr[1]);
	if(mirror)
		ppu_setmirroring(MIRROR_H);
	else
		ppu_setmirroring(MIRROR_V);
}

void vrc1_write(u32 addr,u8 data)
{
	data &= 0xF;
	switch(addr & 0xF000) {
		case 0x8000:
			prg[0] = data;
			break;
		case 0x9000:
			mirror = data & 1;
			chr[0] = (chr[0] & 0xF) | ((data & 2) << 3);
			chr[1] = (chr[1] & 0xF) | ((data & 4) << 2);
			break;
		case 0xA000:
		case 0xB000:
			prg[1] = data;
			break;
		case 0xC000:
		case 0xD000:
			prg[2] = data;
			break;
		case 0xE000:
			chr[0] = (chr[0] & 0x10) | data;
			break;
		case 0xF000:
			chr[1] = (chr[1] & 0x10) | data;
			break;
	}
	sync();
}

void vrc1_init(int hard)
{
	int i;

	for(i=8;i<0x10;i++)
		mem_setwrite(i,vrc1_write);
	prg[0] = 0;
	prg[1] = 0;
	prg[2] = 0;
	chr[0] = 0;
	chr[1] = 0;
	mirror = 0;
	sync();
}

void vrc1_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,3);
	STATE_ARRAY_U8(chr,2);
	STATE_U8(mirror);
	sync();
}
