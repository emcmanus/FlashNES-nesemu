#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"

static u8 cmd,prg,mirror;
static u8 chrlo[4],chrhi,chrbank;
static writefunc_t write4;

static void sync()
{
	mem_setprg32(8,prg);
	mem_setchr1(0,chrlo[0]);
	mem_setchr4(4,0xFF);
	if(mirror & 1) {
		mem_setchr1(1,chrlo[0] | ((chrhi & 1) << 4));
		mem_setchr1(2,chrlo[0] | ((chrhi & 2) << 3));
		mem_setchr1(3,chrlo[0] | ((chrhi & 4) << 2) | ((chrbank & 1) << 3));
	}
	else {
		mem_setchr1(1,chrlo[1] | ((chrhi & 1) << 4));
		mem_setchr1(2,chrlo[2] | ((chrhi & 2) << 3));
		mem_setchr1(3,chrlo[3] | ((chrhi & 4) << 2) | ((chrbank & 1) << 3));
	}
	switch(mirror & 6) {
		case 0: ppu_setmirroring2(0,0,0,1); break;
		case 2: ppu_setmirroring(MIRROR_H); break;
		case 4: ppu_setmirroring(MIRROR_V); break;
		case 6: ppu_setmirroring(MIRROR_1L); break;
	}
}

static void write45(u32 addr,u8 data)
{
	if(addr < 0x4020) {
		write4(addr,data);
		return;
	}
//	log_message("mapper137 write: $%04X = $%02X\n",addr,data);
	if((addr & 0x101) == 0x100)
		cmd = data & 7;
	else if((addr & 0x101) == 0x101) {
		switch(cmd) {
			case 0: chrlo[0] = data & 7;	break;
			case 1: chrlo[1] = data & 7;	break;
			case 2: chrlo[2] = data & 7;	break;
			case 3: chrlo[3] = data & 7;	break;
			case 4: chrhi = data & 7;		break;
			case 5: prg = data & 7;			break;
			case 6: chrbank = data & 7;	break;
			case 7: mirror = data & 7;		break;
		}
		sync();
	}
}

static void init(int hard)
{
	write4 = mem_getwrite(4);
	mem_setwrite(4,write45);
	mem_setwrite(5,write45);
	cmd = 0;
	prg = 0;
	mirror = 0;
	chrlo[0] = 0;
	chrlo[1] = 0;
	chrlo[2] = 0;
	chrlo[3] = 0;
	chrhi = 0;
	chrbank = 0;
	sync();
}

MAPPER_INES(137,init,0,0,0);
