#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/mmc1.h"

static void reset(int hard)
{
	mmc1_init(mmc1_sync);
}

MAPPER_UNIF(nes_sarom, "NES-SAROM", reset,0,0,mmc1_state,INFO( 64, 64, 8,0,0,0));
MAPPER_UNIF(nes_sbrom, "NES-SBROM", reset,0,0,mmc1_state,INFO( 64, 64, 0,0,0,0));
MAPPER_UNIF(nes_scrom, "NES-SCROM", reset,0,0,mmc1_state,INFO( 64,128, 0,0,0,0));
MAPPER_UNIF(nes_serom, "NES-SEROM", reset,0,0,mmc1_state,INFO( 32, 64, 0,0,0,0));
MAPPER_UNIF(nes_sfrom, "NES-SFROM", reset,0,0,mmc1_state,INFO(256, 64, 0,0,0,0));
MAPPER_UNIF(nes_sgrom, "NES-SGROM", reset,0,0,mmc1_state,INFO(256,  0, 0,0,8,0));
MAPPER_UNIF(nes_shrom, "NES-SHROM", reset,0,0,mmc1_state,INFO( 32,128, 0,0,0,0));
MAPPER_UNIF(nes_sjrom, "NES-SJROM", reset,0,0,mmc1_state,INFO(256, 64, 8,0,0,0));
MAPPER_UNIF(nes_skrom, "NES-SKROM", reset,0,0,mmc1_state,INFO(256,128, 8,0,0,0));
MAPPER_UNIF(nes_slrom, "NES-SLROM", reset,0,0,mmc1_state,INFO(256,128, 0,0,0,0));
MAPPER_UNIF(nes_snrom, "NES-SNROM", reset,0,0,mmc1_state,INFO(256,  0, 0,0,8,0));
MAPPER_UNIF(nes_sorom, "NES-SOROM", reset,0,0,mmc1_state,INFO(256,  0, 8,8,8,0));
MAPPER_UNIF(nes_surom, "NES-SUROM", reset,0,0,mmc1_state,INFO(512,  0, 8,0,8,0));
MAPPER_UNIF(nes_sxrom, "NES-SXROM", reset,0,0,mmc1_state,INFO(512,  0,32,0,8,0));
