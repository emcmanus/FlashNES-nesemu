#include "mappers/mapper.h"

static u8 reg = 0;

static void sync()
{
	mem_setprg16(0x8,reg);
	mem_setprg16(0xC,reg);
	mem_setchr8(0,reg);
}

static void reset(int hard)
{
	if(hard == 0)
		reg = (reg + 1) & 3;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(reg);
	sync();
}

MAPPER_INES(60,reset,0,0,0);
