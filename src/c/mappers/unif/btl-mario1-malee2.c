#include "mappers/mapper.h"

static u8 *prg6;
static u8 *sram7;

static u8 read6(u32 addr)
{
	return(prg6[addr & 0x7FF]);
}

static u8 read7(u32 addr)
{
	return(sram7[addr & 0x7FF]);
}

static void write7(u32 addr,u8 data)
{
	sram7[addr & 0x7FF] = data;
}

static void reset(int hard)
{
	nes_setsramsize(1);

	mem_setprg4(6,8);
	mem_setsram4(7,0);
	mem_setprg32(8,0);
	mem_setchr8(0,0);

	prg6 = mem_getreadptr(6);
	sram7 = mem_getreadptr(7);

	mem_setreadptr(6,0);
	mem_setreadptr(7,0);
	mem_setwriteptr(7,0);

	mem_setread(6,read6);
	mem_setread(7,read7);
	mem_setwrite(7,write7);
}

MAPPER_UNIF(btl_mario1_malee2,"BTL-MARIO1-MALEE2",reset,0,0,0,INFO(32 + 4,8,0,2,0,0));
