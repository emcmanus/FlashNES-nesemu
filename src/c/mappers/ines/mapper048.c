#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"

static u8 prg[2],chr[6],mirror;
static u8 irqcounter,irqlatch,irqreload,irqenabled;

static void sync()
{
	int i;

	mem_setchr2(0,chr[0]);
	mem_setchr2(2,chr[1]);
	for(i=4;i<8;i++)
		mem_setchr1(i,chr[i - 2]);
	mem_setprg8(0x8,prg[0]);
	mem_setprg8(0xA,prg[1]);
	mem_setprg16(0xC,0xFF);
}

static void mapper48_write(u32 addr,u8 data)
{
	switch(addr) {
		case 0x8000:
			mirror = data & 0x40;
			prg[0] = data & 0x3F;
			sync();
			break;
		case 0x8001: prg[1] = data & 0x3F; sync(); break;
		case 0x8002: chr[0] = data; sync(); break;
		case 0x8003: chr[1] = data; sync(); break;
		case 0xA000: chr[2] = data; sync(); break;
		case 0xA001: chr[3] = data; sync(); break;
		case 0xA002: chr[4] = data; sync(); break;
		case 0xA003: chr[5] = data; sync(); break;
		case 0xC000: irqlatch = data; break;
		case 0xC001: irqreload = 1; break;
		case 0xC002: irqenabled = 1; break;
		case 0xC003: irqenabled = 0; break;
	}
}

static void mapper48_init(int hard)
{
	int i;

	for(i=8;i<16;i++)
		mem_setwrite(i,mapper48_write);
	for(i=0;i<6;i++) {
		prg[i & 1] = 0;
		chr[i] = 0;
	}
	sync();
}

void mapper48_line(int line,int pcycles)
{
	if(line < 21 || line > 260)
		return;

	if(irqenabled && (nes->ppu.ctrl1 & 0x18)) {
		u8 n = irqcounter;

		if((irqcounter == 0) || irqreload) {
			irqreload = 0;
			irqcounter = irqlatch;
		}
		else
		   irqcounter--;
		if((irqcounter == 0) && n && irqenabled)
			dead6502_irq();
	}
}

static void mapper48_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,2);
	STATE_ARRAY_U8(chr,6);
	STATE_U8(mirror);
	sync();
}

MAPPER_INES(48,mapper48_init,0,mapper48_line,mapper48_state);
