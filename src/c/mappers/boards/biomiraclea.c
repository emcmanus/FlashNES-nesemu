#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg,mirror,irqenabled;
static int irqcounter;

static void sync()
{
	mem_setprg8(0x6,prg);
	mem_setprg32(8,3);
	mem_setvram8(0,0);
	if(mirror)
		ppu_setmirroring(MIRROR_H);
	else
		ppu_setmirroring(MIRROR_V);
}

static void write_reg(u32 addr,u8 data)
{
	switch(addr & 0xE003) {
		case 0xE000: prg = data & 0xF; sync(); break;
		case 0xE001: mirror = data & 8; sync(); break;
		case 0xE002:
			if(data & 2)
				irqenabled = 1;
			else {
				irqenabled = 0;
				irqcounter = 24576 - (113 * 16);
			}
			break;
	}
}

void biomiraclea_line(int line,int pcycles)
{
	if(line < 21 || line >= 261)
		return;
	if(irqenabled == 0 && irqcounter)
		return;
	irqcounter -= pcycles / 3;
	if(irqcounter < 0)
		dead6502_irq();
}

void biomiraclea_init(int hard)
{
	int i;

	for(i=0xE;i<0x10;i++)
		mem_setwrite(i,write_reg);
	nes_setvramsize(1);
	prg = 0;
	mirror = 0;
	irqenabled = 0;
	irqcounter = 0;
	sync();
}

void biomiraclea_state(int mode,u8 *data)
{
	STATE_U8(prg);
	STATE_U8(mirror);
	STATE_U8(irqenabled);
	sync();
}
