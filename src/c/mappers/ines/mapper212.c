#include "defines.h"
#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg[2],chr,mirror;

static void sync()
{
	if(prg[1] < 0xFF) {
		mem_setprg16(0x8,prg[0]);
		mem_setprg16(0xC,prg[1]);
	}
	else
		mem_setprg32(0x8,prg[0]);
	mem_setchr8(0,chr);
	if(mirror == 0)
		ppu_setmirroring(MIRROR_V);
	else
		ppu_setmirroring(MIRROR_H);
}

static void write_upper(u32 addr,u8 data)
{
	if((addr & 0x4000) == 0x4000) {
		prg[0] = (addr & 7) >> 1;
		prg[1] = 0xFF;
	}
	else {
		prg[0] = addr & 7;
		prg[1] = addr & 7;
	}
	chr = addr & 7;
	mirror = addr & 8;
	sync();
}

static void init(int hard)
{
	int i;

	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	prg[0] = prg[1] = 0;
	chr = 0;
	mirror = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,2);
	STATE_U8(chr);
	STATE_U8(mirror);
	sync();
}

MAPPER_INES(212,init,0,0,state);
