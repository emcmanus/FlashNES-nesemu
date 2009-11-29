#include "mappers/mapper.h"
#include "mappers/chips/vrc2.h"

static void reset(int hard)
{
	vrc2_init(KONAMI_VRC2A);
}

MAPPER_INES(22,reset,0,0,vrc2_state);
