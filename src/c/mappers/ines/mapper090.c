#include "mappers/mapper.h"
#include "mappers/chips/mapper90.h"

static void reset(int hard)
{
	mapper90_init(MAPPER90A);
}

MAPPER_INES(90,reset,0,mapper90_line,mapper90_state);
