#include "mappers/mapper.h"
#include "mappers/chips/txc_22211.h"

static void reset(int hard)
{
	txc_22211_reset(hard,TXC_22211B);
}

MAPPER_INES(172,reset,0,0,txc_22211_state);
