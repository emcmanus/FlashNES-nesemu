#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"
#include "mmc5.h"

static u8 prgsize,prgselect,prg[5];

static void syncprg()
{
	mem_setprg8(0x6,prg[0] & 0x7F);
	switch(prgsize) {
		case 0:
			mem_setprg32(8,prg[4]);
			break;
		case 1:
			mem_setprg16(0x8,prg[2]);
			mem_setprg16(0xC,prg[2]);
			break;
		case 2:
			mem_setprg16(0x8,prg[2]);
			mem_setprg8(0xC,prg[3]);
			mem_setprg8(0xE,prg[4]);
			break;
		case 3:
			mem_setprg8(0x8,prg[1]);
			mem_setprg8(0xA,prg[2]);
			mem_setprg8(0xC,prg[3]);
			mem_setprg8(0xE,prg[4]);
			break;
	}
}

static void mmc5_sync()
{
	syncprg();
}

void mmc5_write(u32 addr,u8 data)
{
	switch(addr) {
		case 0x5100:
			prgsize = data & 3;
			syncprg();
			break;
		case 0x5113:
		case 0x5114:
		case 0x5115:
		case 0x5116:
		case 0x5117:
			prgselect = addr - 0x5113;
			syncprg();
			break;
	}
	mmc5_sync();
}

void mmc5_init(int hard)
{
	mem_setwrite(5,mmc5_write);
	prgsize = 0;
	prgselect = 0;
	mmc5_sync();
}

void mmc5_state(int mode,u8 *data)
{
}
