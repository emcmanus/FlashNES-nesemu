#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

///TODO: can be implemented with latch

static u8 reg,mode;

static void sync()
{
	int bank,banks[4],x;

	bank = (reg << 1) & 0x7E;
	x = (reg >> 7) & 1;
	switch(mode) {
		case 0:
			banks[0] = bank | 0;
			banks[1] = bank | 1;
			banks[2] = bank | 2;
			banks[3] = bank | 3;
			break;
		case 1:
			banks[0] = bank | 0;
			banks[1] = bank | 1;
			banks[2] = 0x7E;
			banks[3] = 0x7F;
			break;
		case 2:
			banks[0] = bank;
			banks[1] = bank;
			banks[2] = bank;
			banks[3] = bank;
			break;
		case 3:
			banks[0] = bank | 0;
			banks[1] = bank | 1;
			banks[2] = bank | 0;
			banks[3] = bank | 1;
			break;
	}
	mem_setprg8(0x8,banks[0] ^ x);
	mem_setprg8(0xA,banks[1] ^ x);
	mem_setprg8(0xC,banks[2] ^ x);
	mem_setprg8(0xE,banks[3] ^ x);
	ppu_setmirroring((reg & 0x40) ? MIRROR_H : MIRROR_V);
}

void write_upper(u32 addr,u8 data)
{
	reg = data;
	mode = addr & 3;
	sync();
}

static void init(int hard)
{
	int i;

	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	mem_setvram8(0,0);
	reg = 0;
	mode = 0;
	sync();
}

void mapper15_state(int mode,u8 *data)
{
	STATE_U8(reg);
	STATE_U8(mode);
}

MAPPER_INES(15,init,0,0,mapper15_state);
