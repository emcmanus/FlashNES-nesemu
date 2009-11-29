#include "defines.h"
#include "mmc6.h"
#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"

///TODO: test this code...implement unif

static void (*sync)();
static u8 command;
static u8 prg[2],chr[8];
static u8 mirror;
static u8 sramenabled,sramcontrol;
static u8 irqlatch,irqcounter,irqenabled,irqreload;
static readfunc_t read7;
static writefunc_t write7;

u8 mmc6_getprgbank(int n)
{
	u8 b[4];

	b[0] = prg[0];
	b[1] = prg[1];
	b[2] = 0x3E;
	b[3] = 0x3F;
	if(n & 1)
		return(b[n]);
	return(b[n ^ ((command & 0x40) >> 5)]);
}

u8 mmc6_getchrbank(int n)
{
	return(chr[n ^ ((command & 0x80) >> 5)]);
}

void mmc6_syncprg()
{
	mem_setprg8(0x8,mmc6_getprgbank(0));
	mem_setprg8(0xA,mmc6_getprgbank(1));
	mem_setprg8(0xC,mmc6_getprgbank(2));
	mem_setprg8(0xE,mmc6_getprgbank(3));
}

void mmc6_syncchr()
{
	int i;

	for(i=0;i<8;i++)
		mem_setchr1(i,mmc6_getchrbank(i));
}

void mmc6_syncvram()
{
	int i;

	for(i=0;i<8;i++)
		mem_setvram1(i,mmc6_getchrbank(i));
}

void mmc6_syncsram()
{
	if(sramenabled)
		mem_setsram8(6,0);
	else
		mem_unsetcpu8(6);
}

void mmc6_syncmirror()
{
	if(mirror & 1)
	   ppu_setmirroring(MIRROR_H);
	else
	   ppu_setmirroring(MIRROR_V);
}

u8 mmc6_read_7000(u32 addr)
{
	return(0);
}

void mmc6_write_7000(u32 addr,u8 data)
{

}

void mmc6_init(void (*s)())
{
	int i;

	read7 = mem_getread(7);
	write7 = mem_getwrite(7);
	mem_setread(7,mmc6_read_7000);
	mem_setwrite(7,mmc6_write_7000);
	for(i=8;i<0x10;i++)
		mem_setwrite(i,mmc6_write);
	sync = s;
	command = 0;
	prg[0] = 0;
	prg[1] = 1;
	chr[0] = 0;
	chr[1] = 1;
	chr[2] = 2;
	chr[3] = 3;
	chr[4] = 4;
	chr[5] = 5;
	chr[6] = 6;
	chr[7] = 7;
	mirror = 0;
	sramenabled = 0;
	sramcontrol = 0;
	irqcounter = irqlatch = 0;
	irqenabled = 0;
	sync();
}

void mmc6_write(u32 addr,u8 data)
{
	switch(addr & 0xE001) {
		case 0x8000:
			command = data & 7;
			sramenabled = data & 0x20;
			sync();
			break;
		case 0x8001:
			switch(command) {
				case 0:
					data &= 0xFE;
					chr[0] = data | 0;
					chr[1] = data | 1;
					break;
				case 1:
					data &= 0xFE;
					chr[2] = data | 0;
					chr[3] = data | 1;
					break;
				case 2:
					chr[4] = data;
					break;
				case 3:
					chr[5] = data;
					break;
				case 4:
					chr[6] = data;
					break;
				case 5:
					chr[7] = data;
					break;
				case 6:
					prg[0] = data & 0x3F;
					break;
				case 7:
					prg[1] = data & 0x3F;
					break;
			}
			sync();
			break;
		case 0xA000:
			mirror = data;
			sync();
			break;
		case 0xA001:
			if(sramenabled) {
				sramcontrol = data;
				sync();
			}
			break;
		case 0xC000:
			irqlatch = data;
			break;
		case 0xC001:
			irqreload = 1;
			irqcounter = 0;
			break;
		case 0xE000:
			irqenabled = 0;
			break;
		case 0xE001:
			irqenabled = 1;
			break;
	}
}

void mmc6_line(int line,int pcycles)
{
	//counter is only clocked on visible lines when ppu is rendering
	if(line < 20 || line > 260)
		return;

	if((nes->ppu.ctrl1 & 0x18) == 0 || (nes->ppu.ctrl1 & 0x18) != 0x18)
		return;

	if((irqcounter == 0) || irqreload) {
		irqreload = 0;
		irqcounter = irqlatch;
	}
	else
		irqcounter--;
	if((irqcounter == 0) && irqenabled)
		dead6502_irq();
}

void mmc6_load(u8 *data)
{
	prg[0] = data[0];
	prg[1] = data[1];
	chr[0] = data[2];
	chr[1] = data[3];
	chr[2] = data[4];
	chr[3] = data[5];
	chr[4] = data[6];
	chr[5] = data[7];
	chr[6] = data[8];
	chr[7] = data[9];
	command = data[10];
	mirror = data[11];
	sramenabled = data[12];
	sramcontrol = data[13];
	irqlatch = data[14];
	irqcounter = data[15];
	irqenabled = data[16];
	sync();
}

void mmc6_save(u8 *data)
{
	data[0] = prg[0];
	data[1] = prg[1];
	data[2] = chr[0];
	data[3] = chr[1];
	data[4] = chr[2];
	data[5] = chr[3];
	data[6] = chr[4];
	data[7] = chr[5];
	data[8] = chr[6];
	data[9] = chr[7];
	data[10] = command;
	data[11] = mirror;
	data[12] = sramenabled;
	data[13] = sramcontrol;
	data[14] = irqlatch;
	data[15] = irqcounter;
	data[16] = irqenabled;
}
