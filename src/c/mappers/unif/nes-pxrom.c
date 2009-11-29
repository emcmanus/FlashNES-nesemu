#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc2.h"

MAPPER_UNIF(
	nes_pnrom,"NES-PNROM",
	mmc2_init,mmc2_tile,0,mmc2_state,
	INFO(128,128,0,0,0,0)
);

MAPPER_UNIF(
	nes_peeorom,"NES-PEEOROM",
	mmc2_init,mmc2_tile,0,mmc2_state,
	INFO(128,128,0,0,0,0)
);
