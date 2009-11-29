#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 reg;

static void sync()
{
	mmc3_syncprg(0xF,reg << 4);
	mmc3_syncchr(0x7F,reg << 7);
	mmc3_syncmirror();
	mmc3_syncsram();
}

static void write67(u32 addr,u8 data)
{
	reg = data;
	sync();
}

static void reset(int hard)
{
	reg = 0;
	mmc3_init(sync);
	mem_setwrite(6,write67);
	mem_setwrite(7,write67);
}

MAPPER_INES(47,reset,0,mmc3_line,mmc3_state);
