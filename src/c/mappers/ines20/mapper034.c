#include "defines.h"
#include "mappers/mapper.h"

static u8 reg;

static void sync()
{
	mem_setprg32(8,reg);
	if(nes->rom->chrsize) {
		mem_setchr4(0,reg);
		mem_setchr4(4,reg);
	}
	else
		mem_setvram8(0,0);
}

static void write(u32 addr,u8 data)
{
	reg = data;
	sync();
}

static void reset(int hard)
{
	int i;

	for(i=7;i<16;i++)
		mem_setwrite(i,write);
	reg = 0;
	sync();
}

static void state(u8 *data)
{
	reg = *data;
	sync();
}

MAPPER_INES(34,reset,0,0,state);
