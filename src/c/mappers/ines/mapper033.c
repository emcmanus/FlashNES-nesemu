#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"

static u8 prg[2],chr[6],mirror;

static void sync()
{
	int i;

	mem_setchr2(0,chr[0]);
	mem_setchr2(2,chr[1]);
	for(i=4;i<8;i++)
		mem_setchr1(i,chr[i - 2]);
	mem_setprg8(0x8,prg[0]);
	mem_setprg8(0xA,prg[1]);
	mem_setprg16(0xC,0xFF);
}

static void write_upper(u32 addr,u8 data)
{
	switch(addr) {
		case 0x8000:
			mirror = data & 0x40;
			prg[0] = data & 0x3F;
			sync();
			break;
		case 0x8001: prg[1] = data & 0x3F; sync(); break;
		case 0x8002: chr[0] = data; sync(); break;
		case 0x8003: chr[1] = data; sync(); break;
		case 0xA000: chr[2] = data; sync(); break;
		case 0xA001: chr[3] = data; sync(); break;
		case 0xA002: chr[4] = data; sync(); break;
		case 0xA003: chr[5] = data; sync(); break;
	}
}

static void init(int hard)
{
	int i;

	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	for(i=0;i<6;i++) {
		prg[i & 1] = 0;
		chr[i] = 0;
	}
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,2);
	STATE_ARRAY_U8(chr,6);
	STATE_U8(mirror);
	sync();
}

MAPPER_INES(33,init,0,0,state);
