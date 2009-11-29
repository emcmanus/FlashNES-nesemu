#include "mappers/mapper.h"
#include "mappers/chips/vrc7.h"

static void reset(int hard)
{
	vrc7_init(KONAMI_VRC7B);
}

MAPPER_INES(85,reset,0,vrc7_line,vrc7_state);
