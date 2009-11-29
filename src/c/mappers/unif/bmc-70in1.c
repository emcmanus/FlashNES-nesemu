#include "mappers/mapper.h"
#include "mappers/boards/bmc_70in1.h"

static void bmc_70in1a_reset(int hard)
{
	bmc_70in1_reset(BMC_70IN1A,hard);
}

static void bmc_70in1b_reset(int hard)
{
	bmc_70in1_reset(BMC_70IN1B,hard);
}

MAPPER_UNIF(
	bmc_70in1,"BMC-70in1",
	bmc_70in1a_reset,0,0,bmc_70in1_state,
	INFO(32 + 4,8,0,2,0,0)
);

MAPPER_UNIF(
	bmc_70in1a,"BMC-70in1A",
	bmc_70in1a_reset,0,0,bmc_70in1_state,
	INFO(32 + 4,8,0,2,0,0)
);

MAPPER_UNIF(
	bmc_70in1b,"BMC-70in1B",
	bmc_70in1b_reset,0,0,bmc_70in1_state,
	INFO(32 + 4,8,0,2,0,0)
);
