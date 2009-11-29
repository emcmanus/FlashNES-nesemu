#include "mappers/mapper.h"
#include "mappers/boards/biomiraclea.h"

MAPPER_UNIF(btl_biomiraclea,"BTL-BioMiracleA",
	biomiraclea_init,0,biomiraclea_line,biomiraclea_state,
	INFO(128,0,0,0,8,0)
);
