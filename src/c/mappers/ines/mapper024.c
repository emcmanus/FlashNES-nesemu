#include "mappers/mapper.h"
#include "mappers/chips/vrc6.h"

static void reset(int hard)
{
	vrc6_init(KONAMI_VRC6A);
}

MAPPER_INES(24,reset,0,0,vrc6_state);
