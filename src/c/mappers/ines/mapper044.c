#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 reg;
static writefunc_t writeA;

static void sync()
{
	if(reg == 6) {
		mmc3_syncprg(0x1F,reg << 4);
		mmc3_syncchr(0xFF,reg << 7);
	}
	else {
		mmc3_syncprg(0x0F,reg << 4);
		mmc3_syncchr(0x7F,reg << 7);
	}
	mmc3_syncmirror();
}

static void mapper44_write(u32 addr,u8 data)
{
	if(addr & 1) {
		reg = data & 7;
		if(reg == 7)
			reg = 6;
		sync();
	}
	else
		writeA(addr,data);
}

static void mapper44_init(int hard)
{
	mmc3_init(sync);
	reg = 0;
	writeA = mem_getwrite(0xA);
	mem_setwrite(0xA,mapper44_write);
	sync();
}

static void mapper44_state(int mode,u8 *data)
{
	STATE_U8(reg);
	mmc3_state(mode,data);
}

MAPPER_INES(44,mapper44_init,0,mmc3_line,mapper44_state);
