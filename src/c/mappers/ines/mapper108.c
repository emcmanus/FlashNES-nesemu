#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static void sync()
{
	mem_setprg8(0x6,latch_reg);
	mem_setprg32(0x8,0xFF);
	mem_setchr8(0,0);
}

static void write_upper(u32 addr,u8 data)
{
	if(addr == 0x8FFF)
		latch_write(addr,data);
}

static void reset(int hard)
{
	int i;

	latch_init(sync);
	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
}

MAPPER_INES(108,reset,0,0,latch_state);
