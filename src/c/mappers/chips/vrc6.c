#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/chips/vrc6.h"
#include "mappers/sound/s_VRC6.h"

static apuext_t vrc6 = {
	VRC6sound_Load,
	VRC6sound_Unload,
	VRC6sound_Reset,
	VRC6sound_Get,
	0,
	0
};

static u8 prg[2],chr[8],mirror,irqlatch,irqenabled;
static int irqcounter,irqprescaler;

static void sync()
{
	int i;

	mem_setprg16(0x8,prg[0]);
	mem_setprg8(0xC,prg[1]);
	mem_setprg8(0xE,0xFF);
	for(i=0;i<8;i++)
		mem_setchr1(i,chr[i]);
	switch(mirror & 0xC) {
		case 0x0: ppu_setmirroring(MIRROR_V); break;
		case 0x4: ppu_setmirroring(MIRROR_H); break;
		case 0x8: ppu_setmirroring(MIRROR_1L); break;
		case 0xC: ppu_setmirroring(MIRROR_1H); break;
	}
}

static void write_8000(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: prg[0] = data; sync(); break;
		case 1:
		case 2:
		case 3: break;
	}
}

static void write_9000(u32 addr,u8 data)
{
	VRC6sound_Write(addr,data);
}

static void write_A000(u32 addr,u8 data)
{
	VRC6sound_Write(addr,data);
}

static void write_B000(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0:
		case 1:
		case 2: VRC6sound_Write(addr,data); break;
		case 3: mirror = data; sync(); break;
	}
}

static void write_C000(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0: prg[1] = data; sync(); break;
		case 1:
		case 2:
		case 3: break;
	}
}

static void write_D000(u32 addr,u8 data)
{
	chr[addr & 3] = data;
	sync();
}

static void write_E000(u32 addr,u8 data)
{
	chr[(addr & 3) + 4] = data;
	sync();
}

static void write_F000(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0:
			irqlatch = data;
			break;
		case 1:
			irqenabled = data & 7;
			if(irqenabled & 2) {
				irqcounter = irqlatch;
				irqprescaler = 341;
			}
			break;
		case 2:
			if(irqenabled & 1)
				irqenabled |= 2;
			else
				irqenabled &= ~2;
			break;
		case 3:
			break;
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

void vrc6_line(int line,int pcycles)
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

void vrc6_init(int revision)
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
	if(revision == KONAMI_VRC6B) {
		nes_setsramsize(2);
		mem_setsram8(0x6,0);
	}
	prg[0] = 0;
	prg[1] = -2;
	for(i=0;i>8;i++)
		chr[i] = 0;
	mirror = 0;
	irqlatch = 0;
	irqenabled = 0;
	irqcounter = 0;
	apu_setext(nes->apu,&vrc6);
	sync();
}

void vrc6_state(int mode,u8 *data)
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
