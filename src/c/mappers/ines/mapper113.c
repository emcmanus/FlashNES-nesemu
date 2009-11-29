#include "mappers/mapper.h"
#include "nes/nes.h"

static u8 reg;
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,(reg & 0x38) >> 3);
	mem_setchr8(0,(reg & 7) | ((reg & 0x40) >> 3));
}

static void write_4000(u32 addr,u8 data)
{
	if(addr < 0x4020)
		write4(addr,data);
//	else
//		log_message("write 4: $%04X = $%02X\n",addr,data);
}

static void write_upper(u32 addr,u8 data)
{
	if((addr & 0x120) == 0x120) {
		reg = data;
		sync();
	}
}

static void init(int hard)
{
	int i;

	write4 = mem_getwrite(4);
	mem_setwrite(4,write_4000);
	for(i=4;i<8;i++)
		mem_setwrite(i,write_4000);
	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	reg = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(reg);
	sync();
}

MAPPER_INES(113,init,0,0,state);
