#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 reg;
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,reg & 3);
	mmc3_syncchr(0xFF,0);
	mmc3_syncmirror();
	mmc3_syncsram();
}

static void write4_reg(u32 addr,u8 data)
{
	if(addr == 0x4120) {
		reg = data | (data >> 4);
		sync();
	}
	else
		write4(addr,data);
}

static void init(int hard)
{
	write4 = mem_getwrite(4);
	mem_setwrite(4,write4_reg);
	reg = 0;
	mmc3_init(sync);
}

MAPPER_INES(189,init,0,mmc3_line,mmc3_state);
