#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"

static u8 prg[2],chr[8],mirror;
static u8 prgswap;
static u8 irqlatch,irqcounter,irqenabled;

static void sync()
{
	int i;

	if(prgswap & 2)
		mem_setprg8(0xC,prg[0]);
	else
		mem_setprg8(0x8,prg[0]);
	for(i=0;i<8;i++)
		mem_setchr1(i,chr[i]);
	switch(mirror) {
		case 0: ppu_setmirroring(MIRROR_V); break;
		case 1: ppu_setmirroring(MIRROR_H); break;
		case 2: ppu_setmirroring(MIRROR_1L); break;
		case 3: ppu_setmirroring(MIRROR_1H); break;
	}
}

static void write_upper(u32 addr,u8 data)
{
	u32 reg;

//	log_message("mapper27 write: $%04X = $%02X\n",addr,data);
	switch(addr & 0xF000) {
		case 0x8000:
//			log_message("mapper27 write: prg0: $%04X = $%02X\n",addr,data);
			prg[0] = data;
			sync();
			break;
		case 0xA000:
//			log_message("mapper27 write: prg1: $%04X = $%02X\n",addr,data);
			prg[1] = data;
			sync();
			break;
		case 0x9000:
			switch(addr & 0xF) {
				case 0:
//					log_message("mapper27 write: mirror: $%04X = $%02X\n",addr,data);
					mirror = data & 3;
					sync();
					break;
				case 2:
//					log_message("mapper27 write: prgswap: $%04X = $%02X\n",addr,data);
					prgswap = data;
					sync();
					break;
			}
			break;
		case 0xB000:
		case 0xC000:
		case 0xD000:
		case 0xE000:
			reg = ((((addr >> 12) + 1) & 3) << 1) | ((addr & 2) >> 1);
//			log_message("writing to chr reg %d.%d = $%02X\n",reg,addr&1,data);
			if((addr & 1) == 0)
				chr[reg] = (chr[reg] & 0x00F) | (data << 8);
			else
				chr[reg] = (chr[reg] & 0xFF0) | (data & 0xF);
			break;
		case 0xF000:
			switch(addr & 3) {
				case 0: irqlatch = (irqlatch & 0xF0) | (data & 0xF0); break;
				case 1: irqlatch = (irqlatch & 0x0F) | ((data & 0xF) << 4); break;
				case 2:
					irqenabled = data & 3;
					if(data & 2)
						irqcounter = irqlatch - 1;
					break;
				case 3:
					irqenabled = ((irqenabled & 1) << 1) | (irqenabled & 1);
					break;
			}
			break;
	}
}

static void reset(int hard)
{
	int i;

	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	for(i=0;i<8;i++) {
		prg[i & 1] = 0;
		chr[i] = 0;
	}
	prgswap = 0;
	mirror = 0;
	irqenabled = 0;
	irqlatch = irqcounter = 0;
	//is this correct?
	mem_setprg32(8,0xFF);
	sync();
}

static void line(int line,int pcycles)
{
	if((irqenabled & 2) == 0)
		return;
	if(irqcounter == 0xFF) {
		irqcounter = irqlatch + 1;
		dead6502_irq();
	}
	else
		irqcounter++;
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,2);
	STATE_ARRAY_U8(chr,8);
	STATE_U8(mirror);
	sync();
}

MAPPER_INES(27,reset,0,line,state);
