#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 control,mirror;
static u8 irqsource,irqlatch,irqreload,irqenabled,irqcounter;
static u8 prg[3],chr[8];

static void sync()
{
	int i;
	u8 chrxor = (control & 0x80) >> 5;

	if((control & 0x40) == 0) {
		mem_setprg8(0x8,prg[0]);
		mem_setprg8(0xA,prg[1]);
		mem_setprg8(0xC,prg[2]);
	}
	else {
		mem_setprg8(0xA,prg[0]);
		mem_setprg8(0xC,prg[1]);
		mem_setprg8(0x8,prg[2]);
	}
	mem_setprg8(0xE,-1);
	for(i=0;i<8;i++)
		mem_setchr1(i ^ chrxor,chr[i]);
	if((control & 0x80) == 0) {
		mem_setchr2(0 ^ chrxor,chr[0] >> 1);
		mem_setchr2(2 ^ chrxor,chr[2] >> 1);
	}
	ppu_setmirroring(mirror);
}

static void write_upper(u32 addr,u8 data)
{
	switch(addr) {
		//control register
		case 0x8000:
			control = data;
			break;
		case 0x8001:
			switch(control & 0xF) {
				case 0x0: chr[0] = data; break;
				case 0x1: chr[2] = data; break;
				case 0x2: chr[4] = data; break;
				case 0x3: chr[5] = data; break;
				case 0x4: chr[6] = data; break;
				case 0x5: chr[7] = data; break;
				case 0x6: prg[0] = data; break;
				case 0x7: prg[1] = data; break;
				case 0x8: chr[1] = data; break;
				case 0x9: chr[3] = data; break;
				case 0xF: prg[2] = data; break;
			}
			break;
		case 0xA000:
			mirror = data & 1;
			break;
		case 0xA001:
			break;
		case 0xC000:
			irqlatch = data;
			break;
		case 0xC001:
			irqsource = data & 1;
			irqreload = 1;
			break;
		case 0xE000:
			irqenabled = 0;
			break;
		case 0xE001:
			irqenabled = 1;
			break;
	}
	sync();
}

static void init(int hard)
{
	int i;

	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	control = 0;
	mirror = 0;
	prg[0] = prg[1] = prg[2] = 0xFF;
	for(i=0;i<8;i++)
		chr[i] = i;
	irqsource = 0;
	irqlatch = 0;
	irqreload = 0;
	irqenabled = 0;
	irqcounter = 0;
	sync();
}

static void clock_irqcounter()
{
	if(irqcounter == 0 || irqreload) {
		irqcounter = irqlatch + 1;
		irqreload = 0;
		return;
	}
	if(irqcounter == 1 && irqenabled)
		dead6502_irq();
	irqcounter--;
}

static void line(int line,int pcycles)
{
	int i;

	if(irqsource == 0) {
		if((nes->ppu.ctrl1 & 0x18) != 0)
			clock_irqcounter();
	}
	else {
		for(i=0;i<pcycles / 3 / 4;i++)
			clock_irqcounter();
	}
}

static void state(int mode,u8 *data)
{
	STATE_U8(control);
	STATE_ARRAY_U8(prg,3);
	STATE_ARRAY_U8(chr,8);
	STATE_U8(mirror);
	STATE_U8(irqreload);
	STATE_U8(irqlatch);
	STATE_U8(irqsource);
	STATE_U8(irqenabled);
	STATE_U8(irqcounter);
}

MAPPER_INES(64,init,0,line,state);
