#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static u8 reg[2];

static void sync()
{
	mmc3_syncprg(0x3F,0);
	if(reg[0] & 0x80)
		mem_setprg16(8,reg[0] & 0xF);
	mmc3_syncchr(0xFF,(reg[1] & 1) << 8);
	mmc3_syncmirror();
	mmc3_syncsram();
}

static void write67(u32 addr,u8 data)
{
	reg[addr & 1] = data;
	sync();
}

static void reset(int hard)
{
	nes_setsramsize(2);
	reg[0] = reg[1] = 0;
	mem_setwrite(6,write67);
	mem_setwrite(7,write67);
	mmc3_init(sync);
}

MAPPER_INES(115,reset,0,mmc3_line,mmc3_state);
