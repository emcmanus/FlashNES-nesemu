#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static void sync()
{
	int i,j;

	mmc3_syncprg(0x3F,0);
	for(i=0;i<8;i++) {
		j = mmc3_getchrbank(i);
		if(j & 0x40)
			mem_setvram1(i,j & 7);
		else
			mem_setchr1(i,j & 0x3F);
	}
	mmc3_syncsram();
	mmc3_syncmirror();
}

static void init(int hard)
{
	nes_setvramsize(1);
	mmc3_init(sync);
}

MAPPER_INES(119,init,0,mmc3_line,mmc3_state);
