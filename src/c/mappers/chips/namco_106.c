#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/sound/s_N106.h"

static apuext_t n106 = {
	N106sound_Load,
	N106sound_Unload,
	N106sound_Reset,
	N106sound_Get,
	0,
	0
};

static u8 chr[8],nt[8],prg[4],irqenabled;
static int irqcounter;
static readfunc_t read4;
static writefunc_t write4;

static void sync()
{
	int i;

	mem_setsram8(6,0);

	for(i=0;i<4;i++)
		mem_setprg8(8 + (i * 2),prg[i] & 0x3F);

	for(i=0;i<8;i++) {
		if((chr[i] < 0xE0) || (prg[1] & (0x40 << (i >> 2))))
			mem_setchr1(i,chr[i]);
		else
			mem_setvram1(i,chr[i] & 0x1F);
	}

	for(i=0;i<4;i++) {
		if(nt[i] < 0xE0) {
			mem_setchr1(0x8 + i,nt[i]);
			mem_setchr1(0xC + i,nt[i]);
		}
		else {
			ppu_setnt(i,nt[i] & 1);
			ppu_setnt(i,nt[i] & 1);
		}
	}

	if(prg[0] & 0x40)
		ppu_setmirroring(MIRROR_V);
}

static u8 read_4000(u32 addr)
{
	if(addr & 0x800)
		return(N106sound_Read(addr));
	return(read4(addr));
}

static void write_4000(u32 addr,u8 data)
{
	if(addr & 0x800)
		N106sound_Write(addr,data);
	else
		write4(addr,data);
}

static void write_5000(u32 addr,u8 data)
{
	if(addr & 0x800) {
		irqcounter &= 0x00FF;
		irqcounter |= (data & 0x7F) << 8;
		irqenabled = data & 0x80;
	}
	else {
		irqcounter &= 0xFF00;
		irqcounter |= data;
	}
}

static void write_upper(u32 addr,u8 data)
{
	switch(addr & 0xF800) {
		case 0x8000: chr[0] = data; break;
		case 0x8800: chr[1] = data; break;
		case 0x9000: chr[2] = data; break;
		case 0x9800: chr[3] = data; break;
		case 0xA000: chr[4] = data; break;
		case 0xA800: chr[5] = data; break;
		case 0xB000: chr[6] = data; break;
		case 0xB800: chr[7] = data; break;
		case 0xC000: nt[0] = data; break;
		case 0xC800: nt[1] = data; break;
		case 0xD000: nt[2] = data; break;
		case 0xD800: nt[3] = data; break;
		case 0xE000: prg[0] = data; break;
		case 0xE800: prg[1] = data; break;
		case 0xF000: prg[2] = data; break;
		case 0xF800: N106sound_Write(addr,data); break;
	}
	sync();
}

void namco_106_line(int line,int pcycles)
{
	if(irqenabled == 0)
		return;

	irqcounter += 113;

	if(irqcounter >= 0x8000)
		dead6502_irq();
}

void namco_106_init(int hard)
{
	int i;

	read4 = mem_getread(4);
	write4 = mem_getwrite(4);
	mem_setread(4,read_4000);
	mem_setwrite(4,write_4000);
	mem_setwrite(5,write_5000);
	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	nes_setvramsize(1);
	for(i=0;i<8;i++) {
		chr[i] = 0xFF;
		nt[i & 3] = 0xFF;
		prg[i & 3] = 0xFF;
	}
	irqenabled = 0;
	irqcounter = 0;
	apu_setext(nes->apu,&n106);
	sync();
}

void namco_106_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(chr,8);
	STATE_ARRAY_U8(nt,4);
	STATE_ARRAY_U8(prg,4);
	STATE_U8(irqenabled);
	STATE_U16(irqcounter);
	sync();
}
