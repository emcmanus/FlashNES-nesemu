#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reg[3];
static readfunc_t readF;

static void sync()
{
	if(reg[0] & 0x40) {
		mem_setprg32(8,reg[0] & 0xF);
		mem_setchr8(0,((reg[0] & 0xF) << 2) | ((reg[2] >> 4) & 3));
	}
	else {
		mem_setprg32(8,(reg[0] & 0xE) | (reg[1] & 1));
		mem_setchr8(0,((reg[0] & 0xE) << 2) | ((reg[2] >> 4) & 7));
	}
	ppu_setmirroring((reg[0] >> 7) ? MIRROR_H : MIRROR_V);
}

static void writeF(u32 addr,u8 data)
{
	switch(addr & 0xFFF8) {
		case 0xFF80:
		case 0xFF88:
		case 0xFF90:
		case 0xFF98:
			if(reg[0] == 0)
				reg[0] = data;
			sync();
			break;
		case 0xFFE0:
		case 0xFFE8:
		case 0xFFF0:
		case 0xFFF8:
			reg[2] = data;
			sync();
			break;
	}
}

static u8 readF_reg(u32 addr)
{
	u8 data = readF(addr);

	if((addr & 0xF80) == 0xF80)
		writeF(addr,data);
	return(data);
}

static void init(int hard)
{
	readF = mem_getread(0xF);
	mem_setread(0xF,readF_reg);
	mem_setwrite(0xF,writeF);
	reg[0] = reg[1] = reg[2] = 0;
	sync();
}

MAPPER_INES(234,init,0,0,0);
