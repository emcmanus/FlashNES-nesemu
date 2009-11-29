#include "mappers/mapper.h"
#include "mappers/chips/sachen_8259.h"

static void sachen_8259a_init(int hard)
{
	sachen_8259_init(SACHEN_8259A);
}

static void sachen_8259b_init(int hard)
{
	sachen_8259_init(SACHEN_8259B);
}

static void sachen_8259c_init(int hard)
{
	sachen_8259_init(SACHEN_8259C);
}

static void sachen_8259d_init(int hard)
{
	sachen_8259_init(SACHEN_8259D);
}

MAPPER_UNIF(unl_sachen_8259a,"UNL-Sachen-8259A",
	sachen_8259a_init,0,0,sachen_8259_state,
	INFO(128,256,0,0,0,0)
);

MAPPER_UNIF(unl_sachen_8259b,"UNL-Sachen-8259B",
	sachen_8259b_init,0,0,sachen_8259_state,
	INFO(128,128,0,0,0,0)
);

MAPPER_UNIF(unl_sachen_8259c,"UNL-Sachen-8259C",
	sachen_8259c_init,0,0,sachen_8259_state,
	INFO(128,128,0,0,0,0)
);

MAPPER_UNIF(unl_sachen_8259d,"UNL-Sachen-8259D",
	sachen_8259d_init,0,0,sachen_8259_state,
	INFO(128,128,0,0,0,0)
);
