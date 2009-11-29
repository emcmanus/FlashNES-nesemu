#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/chips/vrc4.h"

static u8 vrc4a_map[] = {0,2,1,3,1};
static u8 vrc4b_map[] = {0,1,2,3,0};
static u8 vrc4c_map[] = {0,2,1,3,6};
static u8 vrc4d_map[] = {0,1,2,3,2};
static u8 vrc4e_map[] = {0,2,1,3,2};
static u8 *map;

static u8 prgflip;

static u8 prg[2],chr[8],mirror;
static u8 irqenabled,irqlatch;
static int irqprescaler,irqcounter;

static void sync()
{
	int i;

	mem_setsram8(0x6,0);
	mem_setprg8(0x8,prg[0]);
	if(prgflip) {
		mem_setprg8(0xA,0x1E);
		mem_setprg8(0xC,prg[1]);
	}
	else {
		mem_setprg8(0xA,prg[1]);
		mem_setprg8(0xC,0x1E);
	}
	mem_setprg8(0xE,0x1F);
	for(i=0;i<8;i++)
		mem_setchr1(i,chr[i]);
	switch(mirror & 3) {
		case 0: ppu_setmirroring(MIRROR_V); break;
		case 1: ppu_setmirroring(MIRROR_H); break;
		case 2: ppu_setmirroring(MIRROR_1H); break;
		case 3: ppu_setmirroring(MIRROR_1L); break;
	}
}

static void write_vrc4(u32 addr,u8 data)
{
//	log_message("vrc4 write: $%04X = $%02X\n",addr,data);
	data &= 0x1F;
	switch(addr & 0xF000) {
		case 0x8000: prg[0] = data; break;
		case 0x9000:
			switch(map[(addr >> map[4]) & 3]) {
				case 0: mirror = data & 1; break;
				case 1: prgflip = data & 2; break;
			}
			break;
		case 0xA000: prg[1] = data; break;
		case 0xB000:
			switch(map[(addr >> map[4]) & 3]) {
				case 0: chr[0] = (chr[0] & 0xF0) | ((data << 0) & 0x0F); break;
				case 1: chr[1] = (chr[1] & 0xF0) | ((data << 0) & 0x0F); break;
				case 2: chr[0] = (chr[0] & 0x0F) | ((data << 4) & 0xF0); break;
				case 3: chr[1] = (chr[1] & 0x0F) | ((data << 4) & 0xF0); break;
			}
			break;
		case 0xC000:
			switch(map[(addr >> map[4]) & 3]) {
				case 0: chr[2] = (chr[2] & 0xF0) | ((data << 0) & 0x0F); break;
				case 1: chr[3] = (chr[3] & 0xF0) | ((data << 0) & 0x0F); break;
				case 2: chr[2] = (chr[2] & 0x0F) | ((data << 4) & 0xF0); break;
				case 3: chr[3] = (chr[3] & 0x0F) | ((data << 4) & 0xF0); break;
			}
			break;
		case 0xD000:
			switch(map[(addr >> map[4]) & 3]) {
				case 0: chr[4] = (chr[4] & 0xF0) | ((data << 0) & 0x0F); break;
				case 1: chr[5] = (chr[5] & 0xF0) | ((data << 0) & 0x0F); break;
				case 2: chr[4] = (chr[4] & 0x0F) | ((data << 4) & 0xF0); break;
				case 3: chr[5] = (chr[5] & 0x0F) | ((data << 4) & 0xF0); break;
			}
			break;
		case 0xE000:
			switch(map[(addr >> map[4]) & 3]) {
				case 0: chr[6] = (chr[6] & 0xF0) | ((data << 0) & 0x0F); break;
				case 1: chr[7] = (chr[7] & 0xF0) | ((data << 0) & 0x0F); break;
				case 2: chr[6] = (chr[6] & 0x0F) | ((data << 4) & 0xF0); break;
				case 3: chr[7] = (chr[7] & 0x0F) | ((data << 4) & 0xF0); break;
			}
			break;
		case 0xF000:
			switch(map[(addr >> map[4]) & 3]) {
				case 0:
					irqlatch = (irqlatch & 0xF0) | (data & 0xF);
					break;
				case 1:
					irqenabled = data & 7;
					if(irqenabled & 2) {
						irqcounter = irqlatch;
						irqprescaler = 341;
					}
					break;
				case 2:
					irqlatch = (irqlatch & 0x0F) | ((data & 0xF) << 4);
					break;
				case 3:
					if(irqenabled & 1)
						irqenabled |= 2;
					else
						irqenabled &= ~2;
					break;
			}
			break;
	}
	sync();
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

void vrc4_line(int line,int pcycles)
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

void vrc4_init(int revision)
{
	int i;

	switch(revision) {
		default:
		case KONAMI_VRC4A: map = vrc4a_map; break;
		case KONAMI_VRC4B: map = vrc4b_map; break;
		case KONAMI_VRC4C: map = vrc4c_map; break;
		case KONAMI_VRC4D: map = vrc4d_map; break;
		case KONAMI_VRC4E: map = vrc4e_map; break;
	}
	nes_setsramsize(2);
	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_vrc4);
	for(i=0;i<8;i++) {
		prg[i & 1] = 0;
		chr[i] = 0;
	}
	mirror = 0;
	sync();
}

void vrc4_state(int mode,u8 *data)
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
