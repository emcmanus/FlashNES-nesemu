#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static readfunc_t nesread4;
static writefunc_t neswrite4;
static u8 reg;
static u8 table[4] = {0,2,2,3};

static u8 read4(u32 addr)
{
	if(addr < 0x4020)
		return(nesread4(addr));
	return(reg);
}

static void write4(u32 addr,u8 data)
{
	if(addr < 0x4020) {
		neswrite4(addr,data);
		return;
	}
	reg = table[data & 3];
}

static void reset(int hard)
{
	int i;

	mmc3_init(mmc3_sync);
	nesread4 = mem_getread(4);
	neswrite4 = mem_getwrite(4);
	for(i=4;i<8;i++) {
		mem_setread(i,read4);
		mem_setwrite(i,write4);
	}
}

MAPPER_UNIF(unl_603_5052,"UNL-603-5052",reset,0,mmc3_line,mmc3_state,INFO(256,256,0,0,0,0));
