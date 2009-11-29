#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg16(0x8,0);
	mem_setprg16(0xC,0xFF);
	mem_setchr4(0,latch_reg & 7);
	mem_setchr4(4,(latch_reg >> 4) & 7);
}

static void reset(int hard)
{
	int i;

	latch_init(sync);
	mem_setwrite(6,latch_write);
	mem_setwrite(7,latch_write);
	for(i=8;i<16;i++)
		mem_setwrite(i,latch_write);
}

MAPPER_INES(184,reset,0,0,latch_state);
