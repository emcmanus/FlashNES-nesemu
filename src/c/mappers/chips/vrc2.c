#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/chips/vrc2.h"

static u8 vrc2a_map[] = {0,1,2,3,1};
static u8 vrc2b_map[] = {0,2,1,3,0};
static u8 *map;
static u8 prg[2],chr[8],mirror;

static void sync()
{
	int i;

	mem_setprg8(0x8,prg[0]);
	mem_setprg8(0xA,prg[1]);
	mem_setprg16(0xC,0x1F);
	for(i=0;i<8;i++)
		mem_setchr1(i,chr[i] >> map[4]);
	if(mirror & 1)
		ppu_setmirroring(MIRROR_H);
	else
		ppu_setmirroring(MIRROR_V);
}

static void write_vrc2(u32 addr,u8 data)
{
	data &= 0x1F;
	switch(addr & 0xF000) {
		case 0x8000:
			prg[0] = data;
			break;
		case 0x9000:
			mirror = data;
			break;
		case 0xA000:
			prg[1] = data;
			break;
		case 0xB000:
			switch(map[addr & 3]) {
				case 0: chr[0] = (chr[0] & 0xF0) | ((data << 0) & 0x0F); break;
				case 1: chr[1] = (chr[1] & 0xF0) | ((data << 0) & 0x0F); break;
				case 2: chr[0] = (chr[0] & 0x0F) | ((data << 4) & 0xF0); break;
				case 3: chr[1] = (chr[1] & 0x0F) | ((data << 4) & 0xF0); break;
			}
			break;
		case 0xC000:
			switch(map[addr & 3]) {
				case 0: chr[2] = (chr[2] & 0xF0) | ((data << 0) & 0x0F); break;
				case 1: chr[3] = (chr[3] & 0xF0) | ((data << 0) & 0x0F); break;
				case 2: chr[2] = (chr[2] & 0x0F) | ((data << 4) & 0xF0); break;
				case 3: chr[3] = (chr[3] & 0x0F) | ((data << 4) & 0xF0); break;
			}
			break;
		case 0xD000:
			switch(map[addr & 3]) {
				case 0: chr[4] = (chr[4] & 0xF0) | ((data << 0) & 0x0F); break;
				case 1: chr[5] = (chr[5] & 0xF0) | ((data << 0) & 0x0F); break;
				case 2: chr[4] = (chr[4] & 0x0F) | ((data << 4) & 0xF0); break;
				case 3: chr[5] = (chr[5] & 0x0F) | ((data << 4) & 0xF0); break;
			}
			break;
		case 0xE000:
			switch(map[addr & 3]) {
				case 0: chr[6] = (chr[6] & 0xF0) | ((data << 0) & 0x0F); break;
				case 1: chr[7] = (chr[7] & 0xF0) | ((data << 0) & 0x0F); break;
				case 2: chr[6] = (chr[6] & 0x0F) | ((data << 4) & 0xF0); break;
				case 3: chr[7] = (chr[7] & 0x0F) | ((data << 4) & 0xF0); break;
			}
			break;
	}
	sync();
}

void vrc2_init(int revision)
{
	int i;

	switch(revision) {
		default:
		case KONAMI_VRC2A: map = vrc2a_map; break;
		case KONAMI_VRC2B: map = vrc2b_map; break;
	}
	for(i=8;i<0x10;i++)
		mem_setwrite(i,write_vrc2);
	for(i=0;i<8;i++) {
		prg[i & 1] = 0;
		chr[i] = 0;
	}
	mirror = 0;
	sync();
}

void vrc2_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,2);
	STATE_ARRAY_U8(chr,8);
	STATE_U8(mirror);
	sync();
}
