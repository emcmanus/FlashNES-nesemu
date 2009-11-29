#include "mappers/mapper.h"

static readfunc_t read4;

static u8 read_reg(u32 addr)
{
	if(addr < 0x4020)
		return(read4(addr));
	if(addr & 0x100)
		return((~addr & 0x3F) | (nes->cpu.x & 0xC0));
	return(0xFF);
}

void sachen_tca01_init(int hard)
{
	read4 = mem_getread(4);
	mem_setread(4,read_reg);
	mem_setread(5,read_reg);
	mem_setprg32(8,0);
	mem_setchr8(0,0);
}
