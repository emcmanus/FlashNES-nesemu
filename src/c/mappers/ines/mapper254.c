#include "defines.h"
#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg,chr,mirror,irqenabled;
static int irqcounter;

static void sync()
{
	mem_setprg8(0x6,prg);
	mem_setprg32(8,3);
	mem_setchr8(0,chr);
	if(mirror)
		ppu_setmirroring(MIRROR_H);
	else
		ppu_setmirroring(MIRROR_V);
}

static void write_upper(u32 addr,u8 data)
{
	switch(addr & 0xE003) {
		case 0x8000:
		case 0x8001:
		case 0x8002:
		case 0x8003:
			chr = data;
			break;
		case 0xE000:
			prg = data & 0xF;
			sync();
			break;
		case 0xE001:
			mirror = data & 8;
			sync();
			break;
		case 0xE002:
			if(data & 2)
				irqenabled = 1;
			else {
				irqenabled = 0;
				irqcounter = 24576;
			}
			break;
	}
	sync();
}

static void line(int line,int pcycles)
{
	if(irqenabled == 0)
		return;
	irqcounter -= 114;
	if(irqcounter < 0)
		dead6502_irq();
}

static void init(int hard)
{
	int i;

	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	prg = 0;
	chr = 0;
	mirror = 0;
	irqenabled = 0;
	irqcounter = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_U8(prg);
	STATE_U8(chr);
	STATE_U8(mirror);
	STATE_U8(irqenabled);
	sync();
}

MAPPER_INES(254,init,0,line,state);
