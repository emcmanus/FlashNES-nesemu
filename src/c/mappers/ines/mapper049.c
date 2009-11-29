#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 reg;

static void sync()
{
	if(reg & 1)
		mmc3_syncprg(0xF,(reg & 0xC0) >> 2);
	else
		mem_setprg32(0x8,(reg & 0x30) >> 4);
	mmc3_syncchr(0x7F,(reg & 0xC0) << 1);
	mmc3_syncmirror();
}

static void mapper49_write(u32 addr,u8 data)
{
	reg = data;
	sync();
}

static void mapper49_init(int hard)
{
	mmc3_init(sync);
	reg = 0;
	mem_setwrite(6,mapper49_write);
	mem_setwrite(7,mapper49_write);
}

static void mapper49_state(int mode,u8 *data)
{
	STATE_U8(reg);
	mmc3_state(mode,data);
	sync();
}

MAPPER_INES(49,mapper49_init,0,mmc3_line,mapper49_state);
