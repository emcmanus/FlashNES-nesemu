#include "mappers/mapper.h"
#include "mappers/chips/mapper90.h"

static void reset(int hard)
{
	mapper90_init(MAPPER90A);
}

MAPPER_UNIF(
	btl_tek2a,"BTL-TEK2A",
	reset,0,mapper90_line,mapper90_state,
	INFO(512,512,8,0,0,0)
);
