#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/sound/s_FME7.h"

static apuext_t fme7 = {
	FME7sound_Load,
	FME7sound_Unload,
	FME7sound_Reset,
	FME7sound_Get,
	0,
	0
};

static u8 command,prg[4],chr[8],mirror,irqenabled;
static int irqcounter;

void fme7_sync()
{
	int i;

	//special case prg page
	if(prg[0] & 0x40)
		if(prg[0] & 0x80)
			mem_setsram8(6,0);
		else
			mem_unsetcpu8(6);
	else
		mem_setprg8(6,prg[0] & 0x3F);

	//set prg pages
	mem_setprg8(0x8,prg[1]);
	mem_setprg8(0xA,prg[2]);
	mem_setprg8(0xC,prg[3]);
	mem_setprg8(0xE,-1);

	//set chr pages
	for(i=0;i<8;i++)
		mem_setchr1(i,chr[i]);

	switch(mirror) {
		case 0: ppu_setmirroring(MIRROR_V); break;
		case 1: ppu_setmirroring(MIRROR_H); break;
		case 2: ppu_setmirroring(MIRROR_1L); break;
		case 3: ppu_setmirroring(MIRROR_1H); break;
	}
}

static void fme7_write_command(u32 addr,u8 data)
{
	command = data & 0xF;
}

static void fme7_write_data(u32 addr,u8 data)
{
	switch(command) {
		case 0x0: chr[0] = data; break;
		case 0x1: chr[1] = data; break;
		case 0x2: chr[2] = data; break;
		case 0x3: chr[3] = data; break;
		case 0x4: chr[4] = data; break;
		case 0x5: chr[5] = data; break;
		case 0x6: chr[6] = data; break;
		case 0x7: chr[7] = data; break;
		case 0x8: prg[0] = data; break;
		case 0x9: prg[1] = data; break;
		case 0xA: prg[2] = data; break;
		case 0xB: prg[3] = data; break;
		case 0xC: mirror = data; break;
		case 0xD:
			irqenabled = data & 0x81;
//			if(irqenabled != 0x81)
//				dead6502_irq();
			break;
		case 0xE:
			irqcounter &= 0xFF00;
			irqcounter |= data;
			break;
		case 0xF:
			irqcounter &= 0x00FF;
			irqcounter |= data << 8;
			break;
	}
	fme7_sync();
}

static void fme7_write_sound(u32 addr,u8 data)
{
	FME7sound_Write(addr,data);
	fme7_sync();
}

void fme7_line(int line,int pcycles)
{
	if((irqenabled & 0x80) == 0)
		return;
	irqcounter -= 113;
	if((irqcounter <= 0) && (irqenabled & 1))
		dead6502_irq();
}

void fme7_init(int hard)
{
	int i;

	for(i=0x8;i<0xA;i++)
		mem_setwrite(i,fme7_write_command);
	for(i=0xA;i<0xC;i++)
		mem_setwrite(i,fme7_write_data);
	for(i=0xC;i<0x10;i++)
		mem_setwrite(i,fme7_write_sound);
	for(i=0;i<8;i++) {
		prg[i & 3] = i & 3;
		chr[i] = i;
	}
	nes_setsramsize(2);
	command = 0;
	mirror = 0;
	irqcounter = 0;
	irqenabled = 0;
	apu_setext(nes->apu,&fme7);
	fme7_sync();
}

void fme7_state(int mode,u8 *data)
{
	STATE_U8(command);
	STATE_ARRAY_U8(prg,4);
	STATE_ARRAY_U8(chr,8);
	STATE_U8(mirror);
	STATE_U8(irqenabled);
	STATE_U32(irqcounter);
	fme7_sync();
}
