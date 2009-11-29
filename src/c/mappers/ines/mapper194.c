#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static void sync()
{
	int i;
	u8 p;

	mmc3_syncprg(0xFF,0);
	for(i=0;i<8;i++) {
		p = mmc3_getchrbank(i);
		if(p < 2)
			mem_setvram1(i,p);
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

MAPPER_INES(194,reset,0,mmc3_line,mmc3_state);
