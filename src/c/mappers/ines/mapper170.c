#include "mappers/mapper.h"

static u8 reg;

static u8 read6(u32 addr)
{
	return(reg | ((addr >> 8) & 0x7F));
}

static void write67(u32 addr,u8 data)
{
	reg = (data << 1) & 0x80;
}

static void reset(int hard)
{
	mem_setread(6,read6);
	mem_setwrite(6,write67);
	mem_setwrite(7,write67);
	mem_setprg16(0x8,0);
	mem_setprg16(0xC,-1);
	if(nes->rom->chrsize)
		mem_setchr8(0,0);
	else
		mem_setvram8(0,0);
}

static void state(int mode,u8 *data)
{
	STATE_U8(reg);
}

MAPPER_INES(170,reset,0,0,state);
