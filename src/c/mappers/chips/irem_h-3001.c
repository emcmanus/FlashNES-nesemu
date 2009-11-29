#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg[3],chr[8],mirror;
static u8 irqcounter,irqenabled;

static void sync()
{
	int i;

	mem_setprg8(0x8,prg[0]);
	mem_setprg8(0xA,prg[1]);
	mem_setprg8(0xC,prg[2]);
	mem_setprg8(0xE,-1);
	for(i=0;i<8;i++)
	   mem_setchr1(i,chr[i]);
	if(mirror & 0x80)
	   ppu_setmirroring(MIRROR_V);
	else
	   ppu_setmirroring(MIRROR_H);
}

static void write_reg(u32 addr,u8 data)
{
	switch(addr & 0xF007) {
		case 0x8000: prg[0] = data; sync();   break;
		case 0x9001: mirror = data; sync();   break;
		case 0x9005: irqcounter = data * 2;   break;
		case 0x9006: irqenabled = irqcounter; break;
		case 0xA000: prg[1] = data; sync();   break;
		case 0xB000:
		case 0xB001:
		case 0xB002:
		case 0xB003:
		case 0xB004:
		case 0xB005:
		case 0xB006:
		case 0xB007: chr[addr & 7] = data; sync(); break;
		case 0xC000: prg[2] = data;        sync(); break;
	}
}

void irem_h3001_line(int line,int pcycles)
{
	if(irqenabled == 0)
	   return;
	if(irqcounter == 0) {
		irqenabled = 0;
		dead6502_irq();
	}
	else
	   irqcounter--;
}

void irem_h3001_init(int hard)
{
	int i;

	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_reg);
	prg[0] = 0;
	prg[1] = 1;
	prg[2] = -2;
	for(i=0;i<8;i++)
		chr[i] = i;
	irqcounter = 0;
	irqenabled = 0;
	mirror = 0;
	sync();
}

void irem_h3001_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,3);
	STATE_ARRAY_U8(chr,8);
	STATE_U8(mirror);
	STATE_U8(irqcounter);
	STATE_U8(irqenabled);
	sync();
}
