#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"

static u8 regs[4];

static void sync()
{
	u8 prg = ((latch_addr >> 6) & 0x3F) | ((latch_addr >> 8) & 0x40);
	u8 chr = ((latch_addr >> 0) & 0x3F) | ((latch_addr >> 8) & 0x40);

	if(latch_addr & 0x1000) {
		mem_setprg16(0x8,prg);
		mem_setprg16(0xC,prg);
	}
	else
		mem_setprg32(8,prg);
	mem_setchr8(0,chr);
	ppu_setmirroring(((latch_addr >> 13) & 1) ^ 1);
}

static u8 read5(u32 addr)
{
	if(addr < 0x5800)
		return(0);
	return(regs[addr & 3]);
}

static void write5(u32 addr,u8 data)
{
	if(addr < 0x5800)
		return;
	regs[addr & 3] = data & 0xF;
}

static void reset(int hard)
{
	mem_setread(5,read5);
	mem_setwrite(5,write5);
	latch_init(sync);
}

MAPPER_INES(225,reset,0,0,latch_state);

