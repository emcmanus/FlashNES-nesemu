#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static void sync()
{
	int i;

	mmc3_syncprg(0xFF,0);
	for(i=0;i<8;i++) {
		if(i == 0)
			mem_setchr4(0,mmc3_getchrbank(i) >> 1);
		if(i == 4)
			mem_setchr2(4,mmc3_getchrbank(i));
		if(i == 5)
			mem_setchr2(6,mmc3_getchrbank(i));
	}
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

MAPPER_INES(197,init,0,mmc3_line,mmc3_state);
