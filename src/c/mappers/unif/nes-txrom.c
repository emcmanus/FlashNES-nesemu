#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static void reset(int hard)
{
	mmc3_init(mmc3_sync);
}

MAPPER_UNIF(nes_tbrom, "NES-TBROM", reset,0,mmc3_line,mmc3_state,INFO( 64, 64,0,0,0,0));
MAPPER_UNIF(nes_terom, "NES-TEROM", reset,0,mmc3_line,mmc3_state,INFO( 32, 64,0,0,0,0));
MAPPER_UNIF(nes_tfrom, "NES-TFROM", reset,0,mmc3_line,mmc3_state,INFO(512, 64,0,0,0,0));
MAPPER_UNIF(nes_tgrom, "NES-TGROM", reset,0,mmc3_line,mmc3_state,INFO(512,  0,0,0,8,0));
MAPPER_UNIF(nes_tkrom, "NES-TKROM", reset,0,mmc3_line,mmc3_state,INFO(512,256,8,0,0,0));
MAPPER_UNIF(nes_tlrom, "NES-TLROM", reset,0,mmc3_line,mmc3_state,INFO(512,256,0,0,0,0));
MAPPER_UNIF(nes_tl1rom,"NES-TL1ROM",reset,0,mmc3_line,mmc3_state,INFO(512,256,0,0,0,0));
MAPPER_UNIF(nes_tl2rom,"NES-TL2ROM",reset,0,mmc3_line,mmc3_state,INFO(512,256,0,0,0,0));
MAPPER_UNIF(nes_tnrom, "NES-TNROM", reset,0,mmc3_line,mmc3_state,INFO(512,  0,8,0,8,0));
MAPPER_UNIF(nes_tr1rom,"NES-TR1ROM",reset,0,mmc3_line,mmc3_state,INFO(512, 64,0,0,0,MIRROR_4));
MAPPER_UNIF(nes_tsrom, "NES-TSROM", reset,0,mmc3_line,mmc3_state,INFO(512,256,0,8,0,0));
MAPPER_UNIF(nes_tvrom, "NES-TVROM", reset,0,mmc3_line,mmc3_state,INFO( 64, 64,0,0,0,MIRROR_4));

MAPPER_UNIF(nes_tksrom,"NES-TKSROM",reset,0,mmc3_line,mmc3_state,INFO(512,128,8,0,0,0));
MAPPER_UNIF(nes_tlsrom,"NES-TLSROM",reset,0,mmc3_line,mmc3_state,INFO(512,128,0,0,0,0));

MAPPER_UNIF(nes_tqrom, "NES-TQROM", reset,0,mmc3_line,mmc3_state,INFO(128, 64,0,0,8,0));
