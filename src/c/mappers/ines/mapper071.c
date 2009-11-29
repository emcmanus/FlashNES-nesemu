#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reg[2];

static void sync()
{
	mem_setprg16(0x8,reg[1]);
	mem_setprg16(0xC,0xFF);
	mem_setvram8(0,0);
	ppu_setmirroring((reg[0] & 0x10) ? MIRROR_1H : MIRROR_1L);
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

static void init(int hard)
{
	int i;

	mem_setwrite(0x9,write_low);
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

MAPPER_INES(71,init,0,0,state);
