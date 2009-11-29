#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 reg,disabled;

static void mapper52_write(u32 addr,u8 data);

static void mapper52_sync()
{
	u8 bank[2],mask[2];

	if(reg & 8) {
		mask[0] = 0xF;
		bank[0] = reg & 7;
	}
	else {
		mask[0] = 0x1F;
		bank[0] = reg & 6;
	}
	if(reg & 0x40) {
		mask[1] = 0x7F;
		bank[1] = ((reg & 0x20) >> 3) | ((reg & 0x10) >> 4) | ((reg & 4) >> 1);
	}
	else {
		mask[1] = 0xFF;
		bank[1] = ((reg & 0x20) >> 3) | ((reg & 0x10) >> 4);
	}
	mmc3_syncprg(mask[0],bank[0] << 4);
	mmc3_syncchr(mask[1],bank[1] << 7);
	mmc3_syncmirror();
	if(disabled) {
		mmc3_syncsram();
		mem_setwrite(6,0);
		mem_setwrite(7,0);
	}
	else {
		mem_unsetcpu8(6);
		mem_setwrite(6,mapper52_write);
		mem_setwrite(7,mapper52_write);
	}
}

static void mapper52_write(u32 addr,u8 data)
{
	if(disabled == 0) {
		disabled = 1;
		reg = data;
		mapper52_sync();
	}
}

static void mapper52_init(int hard)
{
	reg = 0;
	disabled = 0;
	mmc3_init(mapper52_sync);
}

MAPPER_INES(52,mapper52_init,0,mmc3_line,mmc3_state);
