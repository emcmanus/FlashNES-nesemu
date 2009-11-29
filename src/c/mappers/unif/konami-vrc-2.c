#include "mappers/mapper.h"
#include "mappers/chips/vrc2.h"

static void vrc2a_init(int hard)
{
	vrc2_init(KONAMI_VRC2A);
}

static void vrc2b_init(int hard)
{
	vrc2_init(KONAMI_VRC2B);
	mem_setsram8(6,0);
}

MAPPER_UNIF(konami_vrc_2, "KONAMI-VRC-2", vrc2b_init,0,0,vrc2_state,INFO(128,256,0,0,0,0));
MAPPER_UNIF(konami_vrc_2a,"KONAMI-VRC-2A",vrc2a_init,0,0,vrc2_state,INFO(128,256,0,0,0,0));
MAPPER_UNIF(konami_vrc_2b,"KONAMI-VRC-2B",vrc2b_init,0,0,vrc2_state,INFO(128,256,8,0,0,0));
