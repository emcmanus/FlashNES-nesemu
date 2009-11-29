#include "mappers/mapper.h"

static u8 reg;
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,(reg >> 3) & 1);
	mem_setchr8(0,(reg >> 0) & 7);
}

static void write45(u32 addr,u8 data)
{
	if(addr < 0x4020)
		write4(addr,data);
	else {
//		log_message("mapper 79 write: $%04X = $%02X\n",addr,data);
		if(addr & 0x100) {
			reg = data;
			sync();
		}
	}
}

static void init(int hard)
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

MAPPER_INES(79,init,0,0,state);
