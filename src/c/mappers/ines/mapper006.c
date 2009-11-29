#include "defines.h"
#include "mappers/mapper.h"

static u8 reg;
static u8 mode;
static writefunc_t write4;

static void sync()
{
	if(mode == 0x80) {
		mem_setchr8(0,(reg >> 0) & 3);
	}
	else {
		mem_setprg16(0x8,(reg >> 2) & 0xF);
		mem_setprg16(0xC,0xFF);
		mem_setvram8(0,reg & 3);
	}
}

void write_4000(u32 addr,u8 data)
{
	if(addr < 0x4020) {
		write4(addr,data);
		return;
	}

//	log_message("mapper6 write: $%04X = $%02X\n",addr,data);
	switch(addr) {
		case 0x42FE:
			mode = data & 0x80;
			break;
	}
	sync();
}

static void write_upper(u32 addr,u8 data)
{
//	log_message("mapper6 write: $%04X = $%02X\n",addr,data);
	reg = data;
	sync();
}

static void reset(int hard)
{
	int i;

	write4 = mem_getwrite(4);
	mem_setwrite(4,write_4000);
	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	nes_setsramsize(2);
	mem_setsram8(6,0);
	reg = 0;
	sync();
}

MAPPER_INES(6,reset,0,0,0);
