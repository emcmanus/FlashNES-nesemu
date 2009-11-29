#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 regindex,mirror,reg[8];

static void sync()
{
	mem_setsram8(6,0);
	mem_setprg8(0x8,reg[0]);
	mem_setprg8(0xA,reg[1]);
	mem_setprg16(0xC,0xFF);
	mem_setchr2(0,reg[2] >> 1);
	mem_setchr2(2,reg[3] >> 1);
	mem_setchr1(4,reg[4]);
	mem_setchr1(5,reg[5]);
	mem_setchr1(6,reg[6]);
	mem_setchr1(7,reg[7]);
	ppu_setmirroring(mirror ? MIRROR_H : MIRROR_V);
}

static void write_upper(u32 addr,u8 data)
{
//	log_message("mapper112 write: $%04X = $%02X\n",addr,data);
	switch(addr & 0xE001) {
		case 0x8000:
			regindex = data & 7;
			break;
//		case 0x8001:
//			mirror = ~data & 1;
//			break;
		case 0xA000:
			reg[regindex] = data;
			break;
		case 0xE000:
			mirror = data & 1;
			break;
	}
	sync();
}

static void init(int hard)
{
	int i;

	nes_setsramsize(2);
	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	regindex = 0;
	reg[0] = 0;
	reg[1] = 1;
	reg[2] = 0;
	reg[3] = 2;
	reg[4] = 4;
	reg[5] = 5;
	reg[6] = 6;
	reg[7] = 7;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(reg,8);
	STATE_U8(regindex);
	STATE_U8(mirror);
	sync();
}

MAPPER_INES(112,init,0,0,state);
