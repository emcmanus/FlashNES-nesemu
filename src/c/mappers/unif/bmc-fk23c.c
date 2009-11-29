#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 regs[8];

static void sync()
{
	if((regs[0] & 7) == 4) {
		mem_setprg32(8,regs[1] >> 1);
	}
	else if((regs[0] & 7) == 3) {

	}
	else {
		if(regs[3] & 2)
			mem_setprg8(8,regs[1] >> 1);
		mmc3_syncprg(0xFF,0);
	}
	if(nes->rom->chrsize)
		mmc3_syncchr(0xFF,0);
	else
		mmc3_syncvram(7,0);
	if(nes->rom->mirroring & 8)
	   ppu_setmirroring(MIRROR_4);
	else
		mmc3_syncmirror();
	mmc3_syncsram();
}

static void reset(int hard)
{
	mmc3_init(sync);
}

MAPPER_UNIF(bmc_fk23c,"BMC-FK23C",reset,0,mmc3_line,mmc3_state,INFO(32 + 4,8,0,2,0,0));
