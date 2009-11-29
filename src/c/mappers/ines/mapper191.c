#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static void sync()
{
	int i;
	u8 p;

	mmc3_syncprg(0xFF,0);
	for(i=0;i<8;i++) {
		p = mmc3_getchrbank(i);
		if(p & 0x80)
			mem_setvram1(i,p & 7);
		else
			mem_setchr1(i,p);
	}
	mmc3_syncsram();
}

static void reset(int hard)
{
	mmc3_init(sync);
	nes_setvramsize(2);
}

MAPPER_INES(191,reset,0,mmc3_line,mmc3_state);
