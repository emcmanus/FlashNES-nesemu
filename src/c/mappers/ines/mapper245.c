#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static void sync()
{
	int i;

	mmc3_syncprg(0x3F,(mmc3_getchrbank(0) & 2) << 5);
	if(nes->rom->chrsize)
		mmc3_syncchr(0xFF,0);
	else
		mmc3_syncvram(0xFF,0);
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

MAPPER_INES(245,reset,0,mmc3_line,mmc3_state);
