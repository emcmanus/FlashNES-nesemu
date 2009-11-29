#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/chips/vrc7.h"
#include "mappers/sound/s_VRC7.h"

static apuext_t vrc7 = {
	VRC7sound_Load,
	VRC7sound_Unload,
	VRC7sound_Reset,
	VRC7sound_Get,
	0,
	0
};

static u8 prg[2],chr[8],mirror,irqlatch,irqenabled;
static int irqcounter,irqprescaler;

static void sync()
{
	int i;

	mem_setprg8(0x8,prg[0]);
	mem_setprg8(0xA,prg[1]);
	mem_setprg8(0xC,prg[2]);
	mem_setprg8(0xE,0xFF);
	if(nes->rom->chrsize) {
		for(i=0;i<8;i++)
			mem_setchr1(i,chr[i]);
	}
	else {
		for(i=0;i<8;i++)
			mem_setvram1(i,chr[i] & 7);
	}
	switch(mirror) {
		case 0: ppu_setmirroring(MIRROR_V); break;
		case 1: ppu_setmirroring(MIRROR_H); break;
		case 2: ppu_setmirroring(MIRROR_1L); break;
		case 3: ppu_setmirroring(MIRROR_1H); break;
	}
}

static void write_8000(u32 addr,u8 data)
{
	if(addr & 0x18)
		prg[1] = data;
	else
		prg[0] = data;
	sync();
}

static void write_9000(u32 addr,u8 data)
{
	if(addr & 0x18)
		VRC7sound_Write(addr,data);
	else {
		prg[2] = data;
		sync();
	}
}

static void write_A000(u32 addr,u8 data)
{
	if(addr & 0x18)
		chr[1] = data;
	else
		chr[0] = data;
	sync();
}

static void write_B000(u32 addr,u8 data)
{
	if(addr & 0x18)
		chr[3] = data;
	else
		chr[2] = data;
	sync();
}

static void write_C000(u32 addr,u8 data)
{
	if(addr & 0x18)
		chr[5] = data;
	else
		chr[4] = data;
	sync();
}

static void write_D000(u32 addr,u8 data)
{
	if(addr & 0x18)
		chr[7] = data;
	else
		chr[6] = data;
	sync();
}

static void write_E000(u32 addr,u8 data)
{
	if(addr & 0x18)
		irqlatch = data;
	else
		mirror = data & 3;
}

static void write_F000(u32 addr,u8 data)
{
	if(addr & 0x18) {
		irqenabled &= ~2;
		irqenabled |= (irqenabled & 1) << 1;
	}
	else {
		irqenabled = data & 7;
		if(irqenabled & 2) {
			irqcounter = irqlatch;
			irqprescaler = 341;
		}
	}
}

static void clockirq(int clocks)
{
	while(clocks--) {
		if(irqcounter >= 0xFF) {
			irqcounter = irqlatch;
			dead6502_irq();
		}
		else
			irqcounter++;
	}
}

void vrc7_line(int line,int pcycles)
{
	if((irqenabled & 2) == 0)
		return;

	if((irqenabled & 4) == 0) {
		irqprescaler -= pcycles;
		if(irqprescaler < 0) {
			irqprescaler += 341;
			clockirq(1);
		}
	}
	else
		clockirq(pcycles / 3);
}

void vrc7_init(int revision)
{
	int i;

	mem_setwrite(0x8,write_8000);
	mem_setwrite(0x9,write_9000);
	mem_setwrite(0xA,write_A000);
	mem_setwrite(0xB,write_B000);
	mem_setwrite(0xC,write_C000);
	mem_setwrite(0xD,write_D000);
	mem_setwrite(0xE,write_E000);
	mem_setwrite(0xF,write_F000);
	if(revision == KONAMI_VRC7B) {
		nes_setsramsize(2);
		nes_setvramsize(1);
		mem_setsram8(0x6,0);
		mem_setvram8(0,0);
	}
	prg[0] = 0;
	prg[1] = -2;
	prg[2] = -2;
	for(i=0;i>8;i++)
		chr[i] = 0;
	mirror = 0;
	irqlatch = 0;
	irqenabled = 0;
	irqcounter = 0;
	apu_setext(nes->apu,&vrc7);
	sync();
}

void vrc7_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,2);
	STATE_ARRAY_U8(chr,8);
	STATE_U8(mirror);
	STATE_U8(irqenabled);
	STATE_U8(irqlatch);
	STATE_U32(irqprescaler);
	STATE_U32(irqcounter);
	sync();
}
