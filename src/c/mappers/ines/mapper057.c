#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reg[2];

static void sync()
{
	if(reg[1] & 0x10) //prg mode 1
		mem_setprg32(8,reg[1] >> 6);
	else {
		mem_setprg16(0x8,(reg[1] >> 5) & 7);
		mem_setprg16(0xC,(reg[1] >> 5) & 7);
	}
	mem_setchr8(0,(reg[0] & 7) | (reg[1] & 7) | ((reg[0] & 0x40) >> 3));
	if(reg[0] & 8)
		ppu_setmirroring(MIRROR_H);
	else
		ppu_setmirroring(MIRROR_V);
}

static void write_upper(u32 addr,u8 data)
{
	if((addr & 0x8800) == 0x8800)
		reg[1] = data;
	else
		reg[0] = data;
	sync();
}

static void reset(int hard)
{
	int i;

	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	reg[0] = reg[1] = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(reg,2);
	sync();
}

MAPPER_INES(57,reset,0,0,state);
