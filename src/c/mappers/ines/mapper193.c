#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 prg,chr[3];
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,-1);
	mem_setprg8(8,prg);
	mem_setchr4(0,chr[0]);
	mem_setchr2(4,chr[1]);
	mem_setchr2(6,chr[2]);
}

static void write4567(u32 addr,u8 data)
{
	if(addr < 0x4018) {
		write4(addr,data);
		return;
	}
	switch(addr & 3) {
		case 0: chr[0] = data >> 2; break;
		case 1: chr[1] = data >> 1; break;
		case 2: chr[2] = data >> 1; break;
		case 3: prg = data; break;
	}
	sync();
}

static void init(int hard)
{
	int i;

	write4 = mem_getwrite(4);
	for(i=4;i<8;i++)
		mem_setwrite(i,write4567);
	prg = 0;
	chr[0] = 0;
	chr[1] = 2;
	chr[2] = 3;
	sync();
	mem_setprg32(8,-1);
}

MAPPER_INES(193,init,0,mmc3_line,mmc3_state);
