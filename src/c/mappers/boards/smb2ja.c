#include "mappers/mapper.h"

static u8 prg,irqenabled;
static u64 irqcounter;
static writefunc_t write4;

static void sync()
{
	mem_setprg8(0xC,prg);
}

static void write_reg(u32 addr,u8 data)
{
	switch(addr & 0xF000) {
		case 0x4000:
			if(addr == 0x4020 || addr == 0x4021)
				return;
			write4(addr,data);
			break;
		case 0x8000:
		case 0x9000:
			irqenabled = 0;
			irqcounter = 0;
			break;
		case 0xA000:
		case 0xB000:
			irqenabled = 1;
			break;
		case 0xE000:
		case 0xF000:
			prg = data & 7;
			sync();
			break;
	}
}

void smb2ja_line(int line,int pcycles)
{
	if(irqenabled == 0)
		return;

	if(line == 37)
		nes_irq();
}

void smb2ja_init(int hard)
{
	int i;

	write4 = mem_getwrite(4);
	mem_setwrite(4,write_reg);
	for(i=6;i<0x10;i++)
		mem_setwrite(i,write_reg);
	prg = 0;
	irqenabled = 0;
	mem_setprg8(0x6,6);
	mem_setprg8(0x8,4);
	mem_setprg8(0xA,5);
	mem_setprg8(0xC,0);
	mem_setprg8(0xE,7);
	mem_setchr8(0,0);
}

void smb2ja_state(int mode,u8 *data)
{
	STATE_U8(prg);
	STATE_U8(irqenabled);
	sync();
}
