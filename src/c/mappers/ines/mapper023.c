#include "mappers/mapper.h"
#include "mappers/chips/vrc2.h"

static void reset(int hard)
{
	nes_setsramsize(2);
	mem_setsram8(0x6,0);
	vrc2_init(KONAMI_VRC2B);
}

MAPPER_INES(23,reset,0,0,vrc2_state);
