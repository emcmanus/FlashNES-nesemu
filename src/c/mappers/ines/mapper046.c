#include "defines.h"
#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reglo,reghi;

static void sync()
{
	mem_setprg32(8,((reghi & 0xF) << 1) | (reglo & 1));
	mem_setchr8(0,((reghi & 0xF0) >> 1) | ((reglo & 0x70) >> 4));
}

//write high chr/prg bits (write addresses $6000-7FFF)
static void writehi(u32 addr,u8 data)
{
	reghi = data;
	sync();
}

//write low chr/prg bits (write addresses $8000-FFFF)
static void writelo(u32 addr,u8 data)
{
	reglo = data;
	sync();
}

static void reset(int hard)
{
	int i;

	for(i=6;i<8;i++)
		mem_setwrite(i,writehi);
	for(i=8;i<16;i++)
		mem_setwrite(i,writelo);
	reglo = 0;
	reghi = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(reglo);
	STATE_U8(reghi);
	sync();
}

MAPPER_INES(46,reset,0,0,state);
