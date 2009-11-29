#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static void sync()
{
	int i;

	mmc3_syncprg(0xFF,0);
	mmc3_syncchr(0xFF,0);
	mmc3_syncsram();
	for(i=0;i<8;i++)
		mem_setnt(8 + i,(mmc3_getchrbank(i) & 0x80) >> 7);
}

static void reset(int hard)
{
	mmc3_init(sync);
}

MAPPER_INES(118,reset,0,mmc3_line,mmc3_state);
