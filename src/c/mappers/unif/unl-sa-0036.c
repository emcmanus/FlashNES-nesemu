#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reg;
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,0);
	mem_setchr8(0,reg >> 7);
}

static void write_upper(u32 addr,u8 data)
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
	int i;

	write4 = mem_getwrite(4);
	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	reg = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(reg);
	sync();
}

MAPPER_UNIF(unl_sa_0036,"UNL-SA-0036",reset,0,0,state,INFO(32 + 4,8,0,2,0,0));
