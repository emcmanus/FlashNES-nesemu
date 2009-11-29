#include "mappers/mapper.h"
#include "mappers/chips/namco_106.h"

///TODO
///mirroring on this mapper is hardwired, and the mirroring registers do not
///control it on the chip.

MAPPER_INES(210,namco_106_init,0,namco_106_line,namco_106_state);
