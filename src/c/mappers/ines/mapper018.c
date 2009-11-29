#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg[3],chr[8],mirror,irqenabled;
static u16 irqreload,irqmask;
static int irqcounter;

static void sync()
{
	int i;

	mem_setprg8(0x8,prg[0]);
	mem_setprg8(0xA,prg[1]);
	mem_setprg8(0xC,prg[2]);
	mem_setprg8(0xE,0xFF);
	for(i=0;i<8;i++)
		mem_setchr1(i,chr[i]);
	switch(mirror) {
		case 0: ppu_setmirroring(MIRROR_H); break;
		case 1: ppu_setmirroring(MIRROR_V); break;
		case 2: ppu_setmirroring(MIRROR_1L); break;
		case 3: ppu_setmirroring(MIRROR_1H); break;
	}
}

static void write_prg0(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: prg[0] = (prg[0] & 0xF0) | (data & 0x0F); break;
		case 1: data <<= 4;prg[0] = (prg[0] & 0x0F) | (data & 0xF0); break;
		case 2: prg[1] = (prg[1] & 0xF0) | (data & 0x0F); break;
		case 3: data <<= 4;prg[1] = (prg[1] & 0x0F) | (data & 0xF0); break;
	}
	sync();
}

static void write_prg1(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: prg[2] = (prg[2] & 0xF0) | (data & 0x0F); break;
		case 1: data <<= 4;prg[2] = (prg[2] & 0x0F) | (data & 0xF0); break;
		case 2: break;
		case 3: break;
	}
	sync();
}

static void write_chr0(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: chr[0] = (chr[0] & 0xF0) | (data & 0x0F); break;
		case 1: data <<= 4;chr[0] = (chr[0] & 0x0F) | (data & 0xF0); break;
		case 2: chr[1] = (chr[1] & 0xF0) | (data & 0x0F); break;
		case 3: data <<= 4;chr[1] = (chr[1] & 0x0F) | (data & 0xF0); break;
	}
	sync();
}

static void write_chr1(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: chr[2] = (chr[2] & 0xF0) | (data & 0x0F); break;
		case 1: data <<= 4;chr[2] = (chr[2] & 0x0F) | (data & 0xF0); break;
		case 2: chr[3] = (chr[3] & 0xF0) | (data & 0x0F); break;
		case 3: data <<= 4;chr[3] = (chr[3] & 0x0F) | (data & 0xF0); break;
	}
	sync();
}

static void write_chr2(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: chr[4] = (chr[4] & 0xF0) | (data & 0x0F); break;
		case 1: data <<= 4;chr[4] = (chr[4] & 0x0F) | (data & 0xF0); break;
		case 2: chr[5] = (chr[5] & 0xF0) | (data & 0x0F); break;
		case 3: data <<= 4;chr[5] = (chr[5] & 0x0F) | (data & 0xF0); break;
	}
	sync();
}

static void write_chr3(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: chr[6] = (chr[6] & 0xF0) | (data & 0x0F); break;
		case 1: data <<= 4;chr[6] = (chr[6] & 0x0F) | (data & 0xF0); break;
		case 2: chr[7] = (chr[7] & 0xF0) | (data & 0x0F); break;
		case 3: data <<= 4;chr[7] = (chr[7] & 0x0F) | (data & 0xF0); break;
	}
	sync();
}

static void write_irq0(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: irqreload = (irqreload & 0xFFF0) | ((data & 0xF) << 0); break;
		case 1: irqreload = (irqreload & 0xFF0F) | ((data & 0xF) << 4); break;
		case 2: irqreload = (irqreload & 0xF0FF) | ((data & 0xF) << 8); break;
		case 3: irqreload = (irqreload & 0x0FFF) | ((data & 0xF) << 12); break;
	}
}

static void write_irq1(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: irqcounter = irqreload & irqmask; break;
		case 1:
			irqenabled = data & 0xF;
			data &= 0xE;
			if(data & 8) irqmask = 0xF;
			else if(data & 4) irqmask = 0xFF;
			else if(data & 2) irqmask = 0xFFF;
			else irqmask = 0xFFFF;
			break;
		case 2: mirror = data & 3; sync(); break;
		case 3: break;
	}
}

static void reset(int hard)
{
	int i;

	mem_setwrite(0x8,write_prg0);
	mem_setwrite(0x9,write_prg1);
	mem_setwrite(0xA,write_chr0);
	mem_setwrite(0xB,write_chr1);
	mem_setwrite(0xC,write_chr2);
	mem_setwrite(0xD,write_chr3);
	mem_setwrite(0xE,write_irq0);
	mem_setwrite(0xF,write_irq1);
	prg[0] = prg[1] = prg[2] = 0;
	for(i=0;i<8;i++)
		chr[i] = 0;
	mirror = irqenabled = 0;
	sync();
}

static void line(int line,int pcycles)
{
	if((irqenabled == 0) && irqcounter)
		return;

	if(irqcounter <= 0) {
		dead6502_irq();
		irqcounter = 0;
	}

	irqcounter -= 113;
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,3);
	STATE_ARRAY_U8(chr,8);
	STATE_U8(mirror);
	STATE_U8(irqenabled);
	STATE_U16(irqreload);
	STATE_U16(irqmask);
	sync();
}

MAPPER_INES(18,reset,0,line,state);
