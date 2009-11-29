#include "defines.h"
#include "mappers/mapper.h"
#include "mappers/chips/sachen_8259.h"
#include "nes/rom/rom.h"
#include "nes/ppu/ppu.h"

static u8 cmd,reg[8];
static writefunc_t write4;
static void (*syncchr)();

void sachen_8259_sync()
{
	mem_setprg32(8,reg[5] & 7);
	if(reg[7]&1)
		ppu_setmirroring(MIRROR_V);
	else {
		switch(reg[7] & 6) {
			case 0: ppu_setmirroring(MIRROR_V); break;
			case 2: ppu_setmirroring(MIRROR_H); break;
			case 4: ppu_setmirroring2(0,1,1,1); break;
			case 6: ppu_setmirroring(MIRROR_1L); break;
		}
	}
	if(nes->rom->chrsize)
		syncchr();
}

void sachen_8259a_syncchr()
{
	int i,j;

	for(i=0;i<4;i++) {
		if(reg[7] & 1)
			j = (reg[0] & 7) | ((reg[4] & 7) << 3);
		else
			j = (reg[i] & 7) | ((reg[4] & 7) << 3);
		mem_setchr2(i * 2,(j << 1) | (i & 1));
	}
}

void sachen_8259b_syncchr()
{
	int i;

	for(i=0;i<4;i++) {
		if(reg[7] & 1)
			mem_setchr2(i * 2,(reg[0] & 7) | ((reg[4] & 7) << 3));
		else
			mem_setchr2(i * 2,(reg[i] & 7) | ((reg[4] & 7) << 3));
	}
}

void sachen_8259c_syncchr()
{
	int i,j;

	for(i=0;i<4;i++) {
		if(reg[7] & 1)
			j = (reg[0] & 7) | ((reg[4] & 7) << 3);
		else
			j = (reg[i] & 7) | ((reg[4] & 7) << 3);
		mem_setchr2(i * 2,(j << 2) | (i & 3));
	}
}

void sachen_8259d_syncchr()
{
	int i,j;

	for(i=0;i<4;i++) {
		j = reg[i] & 7;
		switch(i) {
			case 1: j |= (reg[4] & 1) << 4; break;
			case 2: j |= (reg[4] & 2) << 3; break;
			case 3: j |= ((reg[4] & 4) << 2) | ((reg[6] & 1) << 3); break;
		}
		mem_setchr1(i,j);
	}
	mem_setchr4(4,~0);
}

void sachen_8259_write(u32 addr,u8 data)
{
	if(addr < 0x4020) {
		write4(addr,data);
		return;
	}
	addr &= 0x101;
	if(addr == 0x100)
		cmd = data & 7;
	else {
		reg[cmd] = data;
		sachen_8259_sync();
	}
}

void sachen_8259_init(int revision)
{
	int i;

	switch(revision) {
		default:
		case SACHEN_8259A: syncchr = sachen_8259a_syncchr; break;
		case SACHEN_8259B: syncchr = sachen_8259b_syncchr; break;
		case SACHEN_8259C: syncchr = sachen_8259c_syncchr; break;
		case SACHEN_8259D: syncchr = sachen_8259d_syncchr; break;
	}
	write4 = mem_getwrite(4);
	for(i=4;i<8;i++)
		mem_setwrite(i,sachen_8259_write);
	for(i=0;i<8;i++)
		reg[i] = 0;
	cmd = 0;
	sachen_8259_sync();
}

void sachen_8259_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(reg,8);
	STATE_U8(cmd);
	sachen_8259_sync();
}
