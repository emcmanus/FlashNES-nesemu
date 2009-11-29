#include "defines.h"
#include "mappers/mapper.h"

static u8 prg[2],chr;

static void sync()
{
	mem_setprg16(0x8,prg[0]);
	mem_setprg16(0xC,prg[1]);
	mem_setchr8(0,chr);
}

static void write_upper(u32 addr,u8 data)
{
	prg[0] = prg[1] = (addr >> 2) & 3;
	chr = addr & 3;
	sync();
}

static void init(int hard)
{
	int i;

	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	prg[0] = prg[1] = 0;
	chr = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,2);
	STATE_U8(chr);
	sync();
}

MAPPER_INES(214,init,0,0,state);
