#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 banksize,prgbank,chrbank;

static void sync()
{
	u8 masks[] = {0x3F,0x1F,0x0F,0x01,0x03,0x00,0x00,0x00};

	mmc3_syncprg(masks[banksize & 7],prgbank << 1);
	if(banksize & 0x20)
		mmc3_syncvram(7,0);
	else
		mmc3_syncchr(0xFF,chrbank << 3);
	mmc3_syncmirror();
	mmc3_syncsram();
}

static void write5(u32 addr,u8 data)
{
	if(addr == 0x5FF0) banksize = data;
	if(addr == 0x5FF1) prgbank = data;
	if(addr == 0x5FF2) chrbank = data;
}

static void reset(int hard)
{
	mmc3_init(sync);
	mem_setwrite(5,write5);
	nes_setvramsize(1);
	banksize = 0;
	prgbank = 0;
	chrbank = 0;
}

MAPPER_UNIF(
	bmc_super24in1sc03,"BMC-Super24in1SC03",
	reset,0,mmc3_line,mmc3_state,
	INFO(4096,2048,0,0,0,0)
);
