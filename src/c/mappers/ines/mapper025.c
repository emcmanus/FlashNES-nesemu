#include "mappers/mapper.h"
#include "mappers/chips/vrc4.h"

static void reset(int hard)
{
	//could also be vrc4d, need a crc check or something...
	vrc4_init(KONAMI_VRC4B);
}

MAPPER_INES(25,reset,0,vrc4_line,vrc4_state);
