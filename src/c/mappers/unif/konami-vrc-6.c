#include "mappers/mapper.h"
#include "mappers/chips/vrc6.h"

static void vrc6a_init(int hard)
{
	vrc6_init(KONAMI_VRC6A);
}

static void vrc6b_init(int hard)
{
	vrc6_init(KONAMI_VRC6B);
}

MAPPER_UNIF(konami_vrc_6, "KONAMI-VRC-6", vrc6b_init,0,0,vrc6_state,INFO(256,256,8,0,0,0));
MAPPER_UNIF(konami_vrc_6a,"KONAMI-VRC-6A",vrc6a_init,0,0,vrc6_state,INFO(256,256,0,0,0,0));
MAPPER_UNIF(konami_vrc_6b,"KONAMI-VRC-6B",vrc6b_init,0,0,vrc6_state,INFO(256,256,8,0,0,0));
