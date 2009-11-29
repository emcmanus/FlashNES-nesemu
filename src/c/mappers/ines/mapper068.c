#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"
#include "nes/ppu/ppu.h"

static u8 prg,chr[4],nt[2],mirror;

static void sync()
{
	int i;

	mem_setprg16(0x8,prg);
	mem_setprg16(0xC,0xFF);
	for(i=0;i<4;i++)
		mem_setchr2(i * 2,chr[i]);
	switch(mirror & 0x13) {
		case 0: ppu_setmirroring(MIRROR_V); break;
		case 1: ppu_setmirroring(MIRROR_H); break;
		case 2: ppu_setmirroring(MIRROR_1L); break;
		case 3: ppu_setmirroring(MIRROR_1H); break;
		case 0x10:
		case 0x12:
			mem_setnt(8 + 0,(mmc3_getchrbank(i) & 0x80) >> 7);
			ppu_setnt_chr(0,nt[0] | 0x80);
			ppu_setnt_chr(1,nt[1] | 0x80);
			ppu_setnt_chr(2,nt[0] | 0x80);
			ppu_setnt_chr(3,nt[1] | 0x80);
			break;
		case 0x11:
		case 0x13:
			ppu_setnt_chr(0,nt[0] | 0x80);
			ppu_setnt_chr(1,nt[0] | 0x80);
			ppu_setnt_chr(2,nt[1] | 0x80);
			ppu_setnt_chr(3,nt[1] | 0x80);
			break;
	}
}

static void write_upper(u32 addr,u8 data)
{
	switch(addr & 0xF000) {
		case 0x8000: chr[0] = data; break;
		case 0x9000: chr[1] = data; break;
		case 0xA000: chr[2] = data; break;
		case 0xB000: chr[3] = data; break;
		case 0xC000: nt[0] = data & 0x7F; break;
		case 0xD000: nt[1] = data & 0x7F; break;
		case 0xE000: mirror = data; break;
		case 0xF000: prg = data; break;
	}
	sync();
}

static void reset(int hard)
{
	int i;

	nes_setsramsize(2);
	mem_setsram8(6,0);
	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_upper);
	prg = 0;
	for(i=0;i<4;i++)
		chr[i] = i;
	mirror = 0;
	sync();
}

MAPPER_INES(68,reset,0,0,0);
