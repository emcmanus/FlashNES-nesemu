#include "defines.h"
#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg,mirror;

static void sync()
{
	mem_setprg32(8,prg);
	mem_setvram8(0,0);
	switch(mirror) {
		case 0:	ppu_setmirroring(MIRROR_H);	break;
		case 1:	ppu_setmirroring(MIRROR_V);	break;
		case 2:	ppu_setmirroring(MIRROR_1L);	break;
		case 3:	ppu_setmirroring(MIRROR_1H);	break;
	}
}

static void write_upper(u32 addr,u8 data)
{
	prg = (addr >> 3) & 0xF;
	mirror = data & 3;
	sync();
}

static void init(int hard)
{
	int i;

	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	nes_setsramsize(2);
	mem_setsram8(6,0);
	prg = 0;
	mirror = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(prg);
	STATE_U8(mirror);
	sync();
}

MAPPER_INES(242,init,0,0,state);
