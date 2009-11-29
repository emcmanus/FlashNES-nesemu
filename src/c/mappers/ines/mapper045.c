#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 reg[4];
static u8 regindex;
static writefunc_t write6;

static void mapper45_sync()
{
	mmc3_syncprg(~reg[3] & 0x3F,reg[1]);
	mmc3_syncchr(0xFF >> ((~reg[2]) & 0xF),reg[0] | ((reg[2] & 0xF0) << 4));
	mmc3_syncmirror();
	mmc3_syncsram();
}

static void mapper45_write(u32 addr,u8 data)
{
	write6(addr,data);
	if((reg[3] & 0x40) == 0) {
		reg[regindex++] = data;
		regindex &= 3;
		mapper45_sync();
	}
}

static void mapper45_init(int hard)
{
	mmc3_init(mapper45_sync);
	write6 = mem_getwrite(6);
	mem_setwrite(6,mapper45_write);
	mem_setwrite(7,mapper45_write);
	reg[0] = reg[1] = reg[2] = reg[3] = 0;
	regindex = 0;
}

static void mapper45_state(int mode,u8 *data)
{
	STATE_U8(regindex);
	STATE_ARRAY_U8(reg,4);
	mmc3_state(mode,data);
}

MAPPER_INES(45,mapper45_init,0,mmc3_line,mapper45_state);
