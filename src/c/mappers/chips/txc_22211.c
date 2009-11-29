#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "mappers/chips/txc_22211.h"

static u8 revision;
static u8 reg[4];
static readfunc_t read4;
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,(reg[2] >> 2) & 3);
	if(revision == TXC_22211B)
		mem_setchr8(0,(((latch_reg ^ reg[2]) >> 3) & 2) | (((latch_reg ^ reg[2]) >> 5) & 1));
	else
		mem_setchr8(0,reg[2] & 3);
}

static u8 read_reg(u32 addr)
{
	if(addr < 0x4020)
		return(read4(addr));
	if(addr == 0x4100) {
		u8 o = 0x40;

		if(revision == TXC_22211C)
			o = 0x41;
		return((reg[1] ^ reg[2]) | o);
	}
	return(0);
}

static void write_reg(u32 addr,u8 data)
{
	if(addr < 0x4020)
		write4(addr,data);
	else if(addr >= 0x4100 && addr < 0x4104)
		reg[addr & 3] = data;
}

void txc_22211_reset(int hard,int r)
{
	revision = r;
	read4 = mem_getread(4);
	write4 = mem_getwrite(4);
	mem_setread(4,read_reg);
	mem_setwrite(4,write_reg);
	reg[0] = reg[1] = reg[2] = reg[3] = 0;
	latch_init(sync);
	mem_setprg32(8,0);
}

void txc_22211_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(reg,4);
	latch_state(mode,data);
}
