#include "mappers/mapper.h"
#include "nes/nes.h"

static u8 reg[2];

static void sync()
{
	mem_setprg16(0xC,0x1F);
	switch(reg[0] & 0x70) {
		case 0x00:
		case 0x20:
		case 0x40:
		case 0x60:
			mem_setprg16(8,((reg[1] << 5) & 0x20) | ((reg[0] >> 1) & 0x10) | (reg[0] & 0xF));
			break;
		case 0x50:
			mem_setprg32(8,(((reg[1] << 5) & 0x20) >> 1) | (reg[0] & 0xF));
			break;
		case 0x70:
			mem_setprg16(8,((reg[1] << 5) & 0x20) | ((reg[0] << 1) & 0x10) | (reg[0] & 0xF));
			break;
	}
}

static void write5D(u32 addr,u8 data)
{
	if((addr & 0x200) != 0)
		return;
	reg[(addr >> 8) & 1] = data;
	sync();
}

static void reset(int hard)
{
	mem_setwrite(0x5,write5D);
	mem_setwrite(0xD,write5D);
	nes_setsramsize(2);
	mem_setvram8(0,0);
	mem_setsram8(6,0);
	reg[0] = reg[1] = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(reg,2);
	sync();
}

MAPPER_INES(164,reset,0,0,state);
