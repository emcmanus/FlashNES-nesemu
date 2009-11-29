#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reg;
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,0);
	mem_setchr8(0,reg >> 7);
}

static void write45(u32 addr,u8 data)
{
	if(addr < 0x4020) {
		write4(addr,data);
		return;
	}
	if(addr & 0x100) {
		reg = data;
		sync();
	}
}

static void reset(int hard)
{
	write4 = mem_getwrite(4);
	mem_setwrite(4,write45);
	mem_setwrite(5,write45);
	reg = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(reg);
	sync();
}

MAPPER_UNIF(unl_sa_72007,"UNL-SA-72007",reset,0,0,state,INFO(32 + 4,8,0,2,0,0));
