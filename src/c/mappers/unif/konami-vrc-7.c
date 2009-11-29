#include "mappers/mapper.h"
#include "mappers/chips/vrc7.h"

static void vrc7a_init(int hard)
{
	vrc7_init(KONAMI_VRC7A);
}

static void vrc7b_init(int hard)
{
	vrc7_init(KONAMI_VRC7B);
}

MAPPER_UNIF(konami_vrc_7, "KONAMI-VRC-7", vrc7b_init,0,vrc7_line,vrc7_state,INFO(512,256,8,0,8,0));
MAPPER_UNIF(konami_vrc_7a,"KONAMI-VRC-7A",vrc7a_init,0,vrc7_line,vrc7_state,INFO(512,256,0,0,0,0));
MAPPER_UNIF(konami_vrc_7b,"KONAMI-VRC-7B",vrc7b_init,0,vrc7_line,vrc7_state,INFO(512,256,8,0,8,0));
