#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/chips/mmc3.h"

static u8 reg;

static void sync()
{
	mmc3_syncprg(0xFF,0);
	if(nes->rom->chrsize)
		mmc3_syncchr(0xFF,0);
	else
		mmc3_syncvram(7,0);
	if(nes->rom->mirroring & 8)
	   ppu_setmirroring(MIRROR_4);
	else
		mmc3_syncmirror();
	mmc3_syncsram();
}

static void write5(u32 addr,u8 data)
{
	data &= 2;
	if(reg != data) {
		reg = data;
		mmc3_syncprg(0xFF,0);
		mmc3_syncchr(0xFF,0);
	}
//	log_message("mapper249 write: $%04X = $%02X\n",addr,data);
}

static void init(int hard)
{
	mem_setwrite(5,write5);
	reg = 0;
	mmc3_init(sync);
}

MAPPER_INES(249,init,0,mmc3_line,mmc3_state);
