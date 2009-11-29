#include "defines.h"
#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"

static u8 prg;
static u8 irqenabled;
static writefunc_t write4;

static void sync()
{
	mem_setprg8(0xC,prg);
}

void mapper50_write(u32 addr,u8 data)
{
	if(addr < 0x4020)
		write4(addr,data);

	if((addr & 0x60) != 0x20)
		return;

	if((addr & 0x100) == 0) {
		prg = ((data & 1) << 2) | ((data & 6) >> 1) | (data & 8);
		sync();
	}

	else {
		if(data & 1)
			irqenabled = 1;
		else {
			irqenabled = 0;
//			irqcounter = 0;
		}
	}
}

void mapper50_line(int line,int pcycles)
{
	if(irqenabled == 0)
		return;

	if(line == 41)
		nes_irq();
}

void mapper50_init(int hard)
{
	int i;

	write4 = mem_getwrite(4);
	for(i=4;i<6;i++)
		mem_setwrite(i,mapper50_write);
//	for(i=8;i<0x10;i++)
//		mem_setwrite(i,mapper50_write);
	prg = 0;
	irqenabled = 0;
	mem_setprg8(0x6,0xF);
	mem_setprg8(0x8,0x8);
	mem_setprg8(0xA,0x9);
	mem_setprg8(0xC,0);
	mem_setprg8(0xE,0xB);
	mem_setvram8(0,0);
}

void mapper50_state(int mode,u8 *data)
{
	STATE_U8(prg);
	STATE_U8(irqenabled);
	sync();
}

MAPPER_INES(50,mapper50_init,0,mapper50_line,mapper50_state);
