#include "defines.h"
#include "mappers/mapper.h"

static void init_128(int hard)
{
	mem_setprg16(0x8,0);
	mem_setprg16(0xC,0);
	mem_setchr8(0,0);
}

static void init_256(int hard)
{
	mem_setprg16(0x8,0);
	mem_setprg16(0xC,1);
	mem_setchr8(0,0);
}

MAPPER_UNIF(nes_nrom_128,"NES-NROM-128",init_128,0,0,0,INFO(16,8,0,0,0,0));
MAPPER_UNIF(nes_nrom_256,"NES-NROM-256",init_256,0,0,0,INFO(32,8,0,0,0,0));
