#include "defines.h"
#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"
#include "mmc4.h"

static u8 PRGBank,Mirroring;
static u8 LatchA[2],LatchAState;
static u8 LatchB[2],LatchBState;

void mmc4_sync()
{
	mem_setsram8(0x6,0);
	mem_setprg16(0x8,PRGBank);
	mem_setprg16(0xC,0xF);
	mem_setchr4(0,LatchA[LatchAState]);
	mem_setchr4(4,LatchB[LatchBState]);
	ppu_setmirroring(Mirroring);
}

void mmc4_init(int hard)
{
	int i;

	for(i=8;i<0x10;i++)
		mem_setwrite(i,mmc4_write);
	nes_setsramsize(2);
	LatchA[0] = LatchA[1] = 0;
	LatchB[0] = LatchB[1] = 0;
	LatchAState = LatchBState = 0;
	PRGBank = 0;
	Mirroring = 0;
	mmc4_sync();
}

void mmc4_write(u32 addr,u8 data)
{
	switch(addr & 0xF000) {
		case 0xA000:
			PRGBank = data & 0xF;
			break;
		case 0xB000:
			LatchA[0] = data & 0x1F;
			break;
		case 0xC000:
			LatchA[1] = data & 0x1F;
			break;
		case 0xD000:
			LatchB[0] = data & 0x1F;
			break;
		case 0xE000:
			LatchB[1] = data & 0x1F;
			break;
		case 0xF000:
			Mirroring = (data & 1) ^ 1;
			break;
	}
	mmc4_sync();
}

void mmc4_tile(u8 tile,int highpt)
{
	if(highpt == 0) {
		if(tile == 0xFD) {
			LatchAState = 0;
			mem_setchr4(0,LatchA[0]);
		}
		else if(tile == 0xFE) {
			LatchAState = 1;
			mem_setchr4(0,LatchA[1]);
		}
	}
	else {
		if(tile == 0xFD) {
			LatchBState = 0;
			mem_setchr4(4,LatchB[0]);
		}
		else if(tile == 0xFE) {
			LatchBState = 1;
			mem_setchr4(4,LatchB[1]);
		}
	}
	mmc4_sync();
}

void mmc4_state(int mode,u8 *data)
{
	STATE_U8(PRGBank);
	STATE_U8(Mirroring);
	STATE_ARRAY_U8(LatchA,2);
	STATE_ARRAY_U8(LatchB,2);
	STATE_U8(LatchAState);
	STATE_U8(LatchBState);
	mmc4_sync();
}
