#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/chips/mmc3.h"

static void sync()
{
	mmc3_syncprg(0xFF,0);
	mmc3_syncchr(0xFF,0);
	if(nes->rom->mirroring & 8)
	   ppu_setmirroring(MIRROR_4);
	else
		mmc3_syncmirror();
	mmc3_syncsram();
}

static void init(int hard)
{
	mmc3_init(sync);
}

MAPPER_INES(165,init,0,0,mmc3_state);
