#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 cmd,prg,chr,mirror;
static writefunc_t write4;

static void sync()
{
	mem_setprg16(0x8,(prg << 1) | 0);
	mem_setprg16(0xC,(prg << 1) | 1);
	mem_setchr8(0,chr);
//	log_message("sync: prg = $%X, chr = $%X, chr / 2 = $%X\n",prg,chr,chr/2);
	if(mirror)
		ppu_setmirroring(MIRROR_V);
	else
		ppu_setmirroring(MIRROR_V);
}

static void write4_reg(u32 addr,u8 data)
{
	if(addr < 0x4020) {
		write4(addr,data);
		return;
	}
	data &= 7;
	if((addr & 0x4101) == 0x4100) {
		cmd = data;
	}
	else if((addr & 0x4101) == 0x4101) {
//		log_message("write cmd %d = $%02X\n",cmd,data);
		switch(cmd) {
			case 0:
				prg = 0;
				chr = 3;
				break;
			case 2:
				prg = data & 1;
				chr |= (data & 1) << 3;
				break;
			case 4:
				chr &= ~4;
				chr |= (data & 1) << 2;
				break;
			case 5:
				prg = data & 7;
				break;
			case 6:
				chr &= ~3;
				chr |= (data & 3) << 0;
				break;
			case 7:
				mirror = data & 1;
				break;
		}
	}
	sync();
}

static void reset(int hard)
{
	write4 = mem_getwrite(4);
	mem_setwrite(4,write4_reg);
	cmd = 0;
	prg = 0;
	chr = 0;
	mirror = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(cmd);
	STATE_U8(prg);
	STATE_U8(chr);
	STATE_U8(mirror);
	sync();
}

MAPPER_UNIF(unl_sachen_74ls374n,"UNL-Sachen-74LS374N",reset,0,0,state,INFO(32 + 4,8,0,2,0,0));
