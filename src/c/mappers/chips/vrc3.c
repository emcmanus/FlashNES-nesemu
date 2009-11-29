#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg;
static u8 irqenabled;
static u32 irqcounter;

static void sync()
{
	mem_setprg16(0x8,prg);
	mem_setprg16(0xC,-1);
	mem_setvram8(0,0);
}

static void write_lo(u32 addr,u8 data)
{
	data &= 0xF;
	switch(addr & 0xF000) {
		case 0x8000:
			irqcounter = (irqcounter & 0xFFF0) | (data << 0);
			break;
		case 0x9000:
			irqcounter = (irqcounter & 0xFF0F) | (data << 4);
			break;
		case 0xA000:
			irqcounter = (irqcounter & 0xF0FF) | (data << 8);
			break;
		case 0xB000:
			irqcounter = (irqcounter & 0x0FFF) | (data << 12);
			break;
	}
}

static void write_hi(u32 addr,u8 data)
{
	data &= 0xF;
	switch(addr & 0xF000) {
		case 0xC000:
			irqenabled = data & 2;
			break;
		case 0xF000:
			prg = data;
			sync();
			break;
	}
}

void vrc3_init(int hard)
{
	int i;

	for(i=0x8;i<0xC;i++)
		mem_setwrite(i,write_lo);
	for(i=0xC;i<0x10;i++)
		mem_setwrite(i,write_hi);
	prg = 0;
	irqenabled = 0;
	irqcounter = 0;
	sync();
}

void vrc3_line(int line,int pcycles)
{
	if(irqenabled == 0)
		return;

	if(irqcounter < 0x10000)
		irqcounter += 256;

	else {
		irqenabled = 0;
		irqcounter = 0;
		nes_irq();
	}
}

void vrc3_state(int mode,u8 *data)
{
	STATE_U8(prg);
	STATE_U8(irqenabled);
	STATE_U32(irqcounter);
	sync();
}
