#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 prg[4],chr[4];
static readfunc_t read6;
static writefunc_t write6;

static void sync()
{
	int i;

	for(i=0;i<4;i++)
		mem_setprg8(8 + (i * 2),prg[i]);
	for(i=0;i<4;i++)
		mem_setchr2(i * 2,chr[i]);
}

static u8 read6_reg(u32 addr)
{
	if(addr >= 0x6800)
		return(read6(addr));
	return(0);
}

static void write6_reg(u32 addr,u8 data)
{
	if(addr >= 0x6800) {
		write6(addr,data);
		return;
	}
	switch(addr & 0x6007) {
		case 0x6000: prg[0] = data; break;
		case 0x6001: prg[1] = data; break;
		case 0x6002: prg[2] = data; break;
		case 0x6003: prg[3] = data; break;
		case 0x6004: chr[0] = data; break;
		case 0x6005: chr[1] = data; break;
		case 0x6006: chr[2] = data; break;
		case 0x6007: chr[3] = data; break;
	}
	sync();
}

static void reset(int hard)
{
	int i;

	read6 = mem_getread(6);
	write6 = mem_getwrite(6);
	mem_setread(6,read6_reg);
	mem_setwrite(6,write6_reg);
	nes_setsramsize(2);
	mem_setsram8(6,0);
	for(i=0;i<4;i++)
		prg[i] = 0;
	prg[3] = 0xFF;
	for(i=0;i<4;i++)
		chr[i] = 0;
	sync();
}

static void state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,4);
	STATE_ARRAY_U8(chr,4);
	sync();
}

MAPPER_INES(246,reset,0,0,state);
