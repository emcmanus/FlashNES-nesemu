#include "mappers/mapper.h"
#include "mappers/chips/vrc6.h"

static void reset(int hard)
{
	vrc6_init(KONAMI_VRC6B);
}

MAPPER_INES(26,reset,0,vrc6_line,vrc6_state);
