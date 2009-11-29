#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg,chr[4],mirror,irqenabled,irqwrite;
static u32 irqcounter;

static void sync()
{
	int i;

	mem_setprg16(0x8,prg);
	mem_setprg16(0xC,0xFF);
	for(i=0;i<4;i++)
		mem_setchr2(i * 2,chr[i]);
	switch(mirror & 3) {
		case 0: ppu_setmirroring(MIRROR_V); break;
		case 1: ppu_setmirroring(MIRROR_H); break;
		case 2: ppu_setmirroring(MIRROR_1L); break;
		case 3: ppu_setmirroring(MIRROR_1H); break;
	}
}

static void write_upper(u32 addr,u8 data)
{
	switch(addr & 0xF800) {
		case 0x8800: chr[0] = data; break;
		case 0x9800: chr[1] = data; break;
		case 0xA800: chr[2] = data; break;
		case 0xB800: chr[3] = data; break;
		case 0xC800:
			if(irqwrite)
				irqcounter = (irqcounter & 0xFF00) | data;
			else
				irqcounter = (irqcounter & 0x00FF) | (data << 8);
			irqwrite ^= 1;
			break;
		case 0xD800: irqenabled = data & 0x10; irqwrite = 0; break;
		case 0xE800: mirror = data; break;
		case 0xF800: prg = data; break;
	}
	sync();
}

static void reset(int hard)
{
	int i;

	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	prg = 0;
	for(i=0;i<4;i++)
		chr[i] = 0;
	mirror = 0;
	irqenabled = irqwrite = 0;
	irqcounter = 0;
	sync();
}

static void line(int line,int pcycles)
{
	if(irqenabled == 0)
		return;
	irqcounter -= 113;
	if(irqcounter > 0x10000) {
		irqcounter &= 0xFFFF;
		irqenabled = 0;
		dead6502_irq();
	}
}

MAPPER_INES(67,reset,0,line,0);
