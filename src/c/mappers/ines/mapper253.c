#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/latch.h"
#include "nes/ppu/ppu.h"
#include <stdio.h>

static unsigned char prg[2],mirror;
static unsigned short chr[8];
static int irqcounter,irqcycles,irqclock;
static unsigned char irqlatch,irqenable;

static void checkreg(unsigned short reg)
{
}
static void sync()
{
	int i;
	static int vram = 0;

	mem_setprg8(0x8,prg[0]);
	mem_setprg8(0xA,prg[1]);
	mem_setprg16(0xC,-1);
	switch(mirror) {
		case 0: ppu_setmirroring(MIRROR_V); break;
		case 1: ppu_setmirroring(MIRROR_H); break;
		case 2: ppu_setmirroring(MIRROR_1L); break;
		case 3: ppu_setmirroring(MIRROR_1H); break;
	}
	for(i=0;i<8;i++) {
//virtualnes has these two cases...havent encountered them yet
		if(chr[i] == 0x88) {
			vram = 0;
			printf("chr page %d enabled chr\n",i);
			continue;
		}
		else if(chr[i] == 0xC8) {
			vram = 1;
			printf("chr page %d enabled vram\n",i);
			continue;
		}

		if(chr[i] == 4 || chr[i] == 5) {
		//	if(vram)
				mem_setvram1(i,chr[i]);
		//	else
		//		mem_setchr1(i,chr[i]);
		}
		else
			mem_setchr1(i,chr[i]);
	}
}

static void write_upper(u32 addr,u8 data)
{
	if(addr == 0x8010)
		prg[0] = data;
	else if(addr == 0xA010)
		prg[1] = data;
	else if(addr == 0x9400)
		mirror = data & 3;
	switch(addr & 0xF00C) {
		case 0xB000:	chr[0] = (chr[0] & 0xF0) | (data & 0xF);	break;	//take lower 4 bits
		case 0xB004:	chr[0] = (chr[0] & 0x0F) | ((data) << 4);	break;
		case 0xB008:	chr[1] = (chr[1] & 0xF0) | (data & 0xF);	break;	//take lower 4 bits
		case 0xB00C:	chr[1] = (chr[1] & 0x0F) | ((data) << 4);	break;
		case 0xC000:	chr[2] = (chr[2] & 0xF0) | (data & 0xF);	break;	//take lower 4 bits
		case 0xC004:	chr[2] = (chr[2] & 0x0F) | ((data) << 4);	break;
		case 0xC008:	chr[3] = (chr[3] & 0xF0) | (data & 0xF);	break;	//take lower 4 bits
		case 0xC00C:	chr[3] = (chr[3] & 0x0F) | ((data) << 4);	break;
		case 0xD000:	chr[4] = (chr[4] & 0xF0) | (data & 0xF);	break;	//take lower 4 bits
		case 0xD004:	chr[4] = (chr[4] & 0x0F) | ((data) << 4);	break;
		case 0xD008:	chr[5] = (chr[5] & 0xF0) | (data & 0xF);	break;	//take lower 4 bits
		case 0xD00C:	chr[5] = (chr[5] & 0x0F) | ((data) << 4);	break;
		case 0xE000:	chr[6] = (chr[6] & 0xF0) | (data & 0xF);	break;	//take lower 4 bits
		case 0xE004:	chr[6] = (chr[6] & 0x0F) | ((data) << 4);	break;
		case 0xE008:	chr[7] = (chr[7] & 0xF0) | (data & 0xF);	break;	//take lower 4 bits
		case 0xE00C:	chr[7] = (chr[7] & 0x0F) | ((data) << 4);	break;
		case 0xF000: irqlatch = (irqlatch & 0xF0) | (data & 0xF);	break;
		case 0xF004: irqlatch = (irqlatch & 0x0F) | ((data & 0xF) << 4);break;
		case 0xF008:
			irqenable = data & 3;
			if(irqenable & 2) {
				irqcounter = irqlatch;
				irqclock = 0;
			}
			break;
	}
//printf("write at $%04X = $%02X\n",addr,data);
	sync();
}

static void init(int hard)
{
	int i;

	nes_setvramsize(1);
	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	prg[0] = prg[1] = 0;
	chr[0] = 0;
	chr[1] = 1;
	chr[2] = 2;
	chr[3] = 3;
	chr[4] = 4;
	chr[5] = 5;
	chr[6] = 6;
	chr[7] = 7;
	mirror = 0;
	sync();
}

void line(int line,int pcycles)
{
	u8 n;

       if(irqenable & 2 ) {
                if((irqclock += (pcycles / 3)) >= 114) {
                        irqclock -= 114;
                        if(irqcounter == 0xFF) {
                                irqcounter = irqlatch;
                                irqenable = (irqenable & 1) * 3;
                                dead6502_irq();
                        } else {
                                irqcounter++;
                        }
                }
        }
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,2);
	STATE_ARRAY_U8(chr,8);
	STATE_U8(mirror);
	sync();
}

MAPPER_INES(253,init,0,line,state);
