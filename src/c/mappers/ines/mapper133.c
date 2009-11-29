#include "defines.h"
#include "mappers/mapper.h"
#include "nes/nes.h"

static u8 reg;
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,(reg >> 2) & 3);
	mem_setchr8(0,reg & 3);
}

static void write_reg(u32 addr,u8 data)
{
	if(addr < 0x4020)
		write4(addr,data);
	else if((addr & 0x120) == 0x120) {
		reg = data;
		sync();
	}
}

static void init(int hard)
{
	write4 = mem_getwrite(4);
	mem_setwrite(4,write_reg);
	reg = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(reg);
	sync();
}

MAPPER_INES(133,init,0,0,state);
