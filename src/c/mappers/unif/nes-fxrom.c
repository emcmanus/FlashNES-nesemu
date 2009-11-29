#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc4.h"

MAPPER_UNIF(
	nes_fjrom,"NES-FJROM",
	mmc4_init,mmc4_tile,0,mmc4_state,
	INFO(128,128,0,0,0,0)
);

MAPPER_UNIF(
	nes_fkrom,"NES-FKROM",
	mmc4_init,mmc4_tile,0,mmc4_state,
	INFO(256,128,0,0,0,0)
);
