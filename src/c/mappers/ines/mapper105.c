#include "mappers/mapper.h"
#include "mappers/chips/mmc1.h"
#include "nes/nes.h"

static u8 prglock,irqenabled,dip;
static u32 irqmax,irqcounter;

static void sync()
{
	static u8 oldreg1 = 0x10;
	u8 reg1 = mmc1_getlowchr();

	mmc1_syncmirror();
	mmc1_syncsram();
	mem_setvram8(0,0);
	if(reg1 & 0x10) {
		irqenabled = 0;
		irqcounter = 0;
	}
	else
		irqenabled = 1;
	if(prglock) {
		mem_setprg32(8,0);
		if((oldreg1 & 0x10) == 0 && (reg1 & 0x10) == 0x10)
			prglock = 0;
	}
	else {
		if(reg1 & 8)
			mmc1_syncprg(7,8);
		else
			mem_setprg32(8,(reg1 >> 1) & 3);
	}
	oldreg1 = reg1;
}

static void reset(int hard)
{
	nes_setsramsize(2);
	nes_setvramsize(1);
	prglock = 1;
	irqcounter = 0;
	irqenabled = 0;
	dip = 4;
	irqmax = 0x20000000 | (dip << 25);
	mmc1_init(sync);
}

static void line(int line,int pcycles)
{
	if(irqenabled == 0)
		return;
	irqcounter += pcycles / 3;
	if(irqcounter >= irqmax)
		dead6502_irq();
}

static void state(int mode,u8 *data)
{
	if(mode >= 2)log_message("dip = %X\n",*data);
	CFG_U8(dip);
	STATE_U8(dip);
	STATE_U8(prglock);
	STATE_U8(irqenabled);
	STATE_U32(irqcounter);
	mmc1_state(mode,data);
}

MAPPER_INES(105,reset,0,line,state);
