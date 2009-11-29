#include "mappers/mapper.h"
#include "mappers/chips/latch.h"

static writefunc_t write4;

static void write45(u32 addr,u8 data)
{
	if(addr < 0x4020)
		write4(addr,data);
	else
		latch_write(addr,data);
}

static void sync()
{
	mem_setprg32(8,latch_reg >> 4);
	mem_setchr8(0,latch_reg & 0xF);
}

static void reset(int hard)
{
	write4 = mem_getwrite(4);
	mem_setwrite(4,write45);
	mem_setwrite(5,write45);
	latch_init(sync);
}

MAPPER_INES(240,reset,0,0,latch_state);

