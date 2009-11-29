#include "mappers/mapper.h"
#include "mappers/chips/txc_22211.h"

static void reset(int hard)
{
	txc_22211_reset(hard,TXC_22211A);
}

MAPPER_UNIF(unl_22211,"UNL-22211",reset,0,0,txc_22211_state,INFO(512,512,0,0,0,0));
