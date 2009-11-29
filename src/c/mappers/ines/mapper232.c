#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reg[2];

static void sync()
{
	mem_setprg16(0x8,((reg[0] >> 1) & 0xC) | (reg[1] & 3));
	mem_setprg16(0xC,((reg[0] >> 1) & 0xC) | 3);
	mem_setvram8(0,0);
}

static void write_high(u32 addr,u8 data)
{
	reg[1] = data;
	sync();
}

static void write_low(u32 addr,u8 data)
{
	reg[0] = data;
	sync();
}

static void reset(int hard)
{
	int i;

	for(i=0x8;i<0xC;i++)
		mem_setwrite(i,write_low);
	for(i=0xC;i<0x10;i++)
		mem_setwrite(i,write_high);
	reg[0] = reg[1] = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(reg,2);
	sync();
}

MAPPER_INES(232,reset,0,0,state);
