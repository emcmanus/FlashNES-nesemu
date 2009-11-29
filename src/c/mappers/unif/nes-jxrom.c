#include "mappers/mapper.h"
#include "mappers/chips/fme7.h"

MAPPER_UNIF(nes_jlrom,"NES-JLROM",fme7_init,0,fme7_line,fme7_state,INFO(256,256,0,0,0,0));
MAPPER_UNIF(nes_jsrom,"NES-JSROM",fme7_init,0,fme7_line,fme7_state,INFO(256,256,8,0,0,0));
