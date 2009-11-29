//what is the name of this chip?  need to give this file a proper name.

#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "mappers/chips/mapper90.h"

static u8 prg[4],chrlo[8],chrhi[8],nt[4];
static u8 mirror,bankmode,mul[2],onebyte,chrcontrol;
static u8 irqenabled,irqmode,irqxor;
static u8 irqlo,irqhi;
static int revision;

static u8 reverse(u8 data)
{
	u8 ret = 0;

	if(data & 0x40)	ret |= 0x01;
	if(data & 0x20)	ret |= 0x02;
	if(data & 0x10)	ret |= 0x04;
	if(data & 0x08)	ret |= 0x08;
	if(data & 0x04)	ret |= 0x10;
	if(data & 0x02)	ret |= 0x20;
	if(data & 0x01)	ret |= 0x40;
	return(ret);
}

static void syncprg()
{
	if((bankmode & 0x80) == 0)	//dont map in rom at $6000-7FFF
		mem_unsetcpu8(6);
	switch(bankmode & 7) {
		case 0:
			mem_setprg8(0x6,(prg[3] << 2) + 3);
			mem_setprg32(0x8,0xFF);
			break;
		case 1:
			mem_setprg8(0x6,(prg[3] << 1) + 1);
			mem_setprg16(0x8,prg[1]);
			mem_setprg16(0xC,0xFF);
			break;
		case 2:
			mem_setprg8(0x6,prg[3]);
			mem_setprg8(0x8,prg[0]);
			mem_setprg8(0xA,prg[1]);
			mem_setprg8(0xC,prg[2]);
			mem_setprg8(0xE,0xFF);
			break;
		case 3:
			mem_setprg8(0x6,reverse(prg[3]));
			mem_setprg8(0x8,reverse(prg[0]));
			mem_setprg8(0xA,reverse(prg[1]));
			mem_setprg8(0xC,reverse(prg[2]));
			mem_setprg8(0xE,0xFF);
			break;
		case 4:
			mem_setprg8(0x6,(prg[3] << 2) + 3);
			mem_setprg32(0x8,prg[3]);
			break;
		case 5:
			mem_setprg8(0x6,(prg[3] << 1) + 1);
			mem_setprg16(0x8,prg[1]);
			mem_setprg16(0xC,prg[3]);
			break;
		case 6:
			mem_setprg8(0x6,prg[3]);
			mem_setprg8(0x8,prg[0]);
			mem_setprg8(0xA,prg[1]);
			mem_setprg8(0xC,prg[2]);
			mem_setprg8(0xE,prg[3]);
			break;
		case 7:
			mem_setprg8(0x6,reverse(prg[3]));
			mem_setprg8(0x8,reverse(prg[0]));
			mem_setprg8(0xA,reverse(prg[1]));
			mem_setprg8(0xC,reverse(prg[2]));
			mem_setprg8(0xE,reverse(prg[3]));
			break;
	}
}

static void syncchr()
{
	if(chrcontrol & 0x20) {
		switch(bankmode & 0x18) {
			case 0x00:
				mem_setchr8(0,chrlo[0] | (chrhi[0] << 8));
				break;
			case 0x08:
				mem_setchr4(0,chrlo[0] | (chrhi[0] << 8));
				mem_setchr4(4,chrlo[4] | (chrhi[4] << 8));
				break;
			case 0x10:
				mem_setchr2(0,chrlo[0] | (chrhi[0] << 8));
				mem_setchr2(2,chrlo[2] | (chrhi[2] << 8));
				mem_setchr2(4,chrlo[4] | (chrhi[4] << 8));
				mem_setchr2(6,chrlo[6] | (chrhi[6] << 8));
				break;
			case 0x18:
				mem_setchr1(0,chrlo[0] | (chrhi[0] << 8));
				mem_setchr1(1,chrlo[1] | (chrhi[1] << 8));
				mem_setchr1(2,chrlo[2] | (chrhi[2] << 8));
				mem_setchr1(3,chrlo[3] | (chrhi[3] << 8));
				mem_setchr1(4,chrlo[4] | (chrhi[4] << 8));
				mem_setchr1(5,chrlo[5] | (chrhi[5] << 8));
				mem_setchr1(6,chrlo[6] | (chrhi[6] << 8));
				mem_setchr1(7,chrlo[7] | (chrhi[7] << 8));
				break;
		}
	}
	else {
		switch(bankmode & 0x18) {
			case 0x00:
				mem_setchr8(0,chrlo[0] | ((chrcontrol & 0x1F) << 8));
				break;
			case 0x08:
				mem_setchr4(0,chrlo[0] | ((chrcontrol & 0x1F) << 8));
				mem_setchr4(4,chrlo[4] | ((chrcontrol & 0x1F) << 8));
				break;
			case 0x10:
				mem_setchr2(0,chrlo[0] | ((chrcontrol & 0x1F) << 8));
				mem_setchr2(2,chrlo[2] | ((chrcontrol & 0x1F) << 8));
				mem_setchr2(4,chrlo[4] | ((chrcontrol & 0x1F) << 8));
				mem_setchr2(6,chrlo[6] | ((chrcontrol & 0x1F) << 8));
				break;
			case 0x18:
				mem_setchr1(0,chrlo[0] | ((chrcontrol & 0x1F) << 8));
				mem_setchr1(1,chrlo[1] | ((chrcontrol & 0x1F) << 8));
				mem_setchr1(2,chrlo[2] | ((chrcontrol & 0x1F) << 8));
				mem_setchr1(3,chrlo[3] | ((chrcontrol & 0x1F) << 8));
				mem_setchr1(4,chrlo[4] | ((chrcontrol & 0x1F) << 8));
				mem_setchr1(5,chrlo[5] | ((chrcontrol & 0x1F) << 8));
				mem_setchr1(6,chrlo[6] | ((chrcontrol & 0x1F) << 8));
				mem_setchr1(7,chrlo[7] | ((chrcontrol & 0x1F) << 8));
				break;
		}
	}
}

static void syncnt()
{
	if((bankmode & 0x20) && (revision == MAPPER90B)) {
		int i;

		for(i=0;i<4;i++) {
			if(bankmode & 0x40) {
			}
			else {
				mem_setchr1(0x8 + i,nt[i] & 1);
				mem_setchr1(0xC + i,nt[i] & 1);
			}
		}
	}
	else {
		switch(mirror & 3) {
			case 0: ppu_setmirroring(MIRROR_V); break;
			case 1: ppu_setmirroring(MIRROR_H); break;
			case 2: ppu_setmirroring(MIRROR_1L); break;
			case 3: ppu_setmirroring(MIRROR_1H); break;
		}
	}
}

u8 mapper90_read_mul(u32 addr)
{
	switch(addr & 0x803) {
		case 0x800:
			return((u8)(mul[0] * mul[1]));
		case 0x801:
			return((u8)((mul[0] * mul[1]) >> 8));
		case 0x803:
			return(onebyte);
	}
	return(0);
}

void mapper90_write_mul(u32 addr,u8 data)
{
	switch(addr & 0x803) {
		case 0x800:
			mul[0] = data;
			break;
		case 0x801:
			mul[1] = data;
			break;
		case 0x803:
			onebyte = data;
			break;
	}
}

void mapper90_write_prg(u32 addr,u8 data)
{
//	log_message("mapper90 write prg: $%04X = $%02X\n",addr,data);
//	if(addr >= 0x8008)
//		return;
	prg[addr & 3] = data;
	syncprg();
}

void mapper90_write_chrlo(u32 addr,u8 data)
{
	chrlo[addr & 7] = data;
	syncchr();
}

void mapper90_write_chrhi(u32 addr,u8 data)
{
	chrhi[addr & 7] = data;
	syncchr();
}

void mapper90_write_nt(u32 addr,u8 data)
{
	nt[addr & 7] = data;
	syncnt();
}

void mapper90_write_irq(u32 addr,u8 data)
{
	switch(addr & 7) {
		case 0:
			break;
		case 1:
			irqmode = data;
			break;
		case 2:
			irqenabled = 0;
			break;
		case 3:
			irqenabled = 1;
			break;
		case 4:
			irqlo = data ^ irqxor;
			break;
		case 5:
			irqhi = data ^ irqxor;
			break;
		case 6:
			irqxor = data;
			break;
		case 7:
			break;
	}
}

void mapper90_write_control(u32 addr,u8 data)
{
	switch(addr & 3) {
		case 0:
			bankmode = data;
			syncprg();
			syncchr();
			syncnt();
			break;
		case 1:
			mirror = data;
			syncnt();
			break;
		case 2:
			syncnt();
			break;
		case 3:
			chrcontrol = data;
			syncchr();
			break;
	}
}

static void irqtick()
{
	u8 irqmask;

	//determine prescaler
	if(irqmode & 4)
		irqmask = 7;
	else
		irqmask = 0xFF;

	//count down
	if(irqmode & 0x80) {
		irqlo--;
		if((irqlo & irqmask) == irqmask) {
			irqhi--;
			if(irqhi == 0xFF)
				dead6502_irq();
		}
	}

	//count up
	if(irqmode & 0x40) {
		irqlo++;
		if((irqlo & irqmask) == 0) {
			irqhi++;
			if(irqhi == 0)
				dead6502_irq();
		}
	}
}

void mapper90_line(int line,int pcycles)
{
	int i;

	if(irqenabled == 0)
		return;
	if((irqmode & 3) == 0) {
		for(i=0;i<(pcycles/3);i++)
			irqtick();
	}
	else if((irqmode & 3) == 1) {
		if((nes->ppu.ctrl1 & 0x18) == 0 || (nes->ppu.ctrl1 & 0x18) != 0x18)
			return;
		for(i=0;i<8;i++)
			irqtick();
	}
	else
		log_message("unsupported irq mode\n");
}

void mapper90_init(int r)
{
	int i;

	mem_setread(0x5,mapper90_read_mul);
	mem_setwrite(0x5,mapper90_write_mul);
	mem_setwrite(0x8,mapper90_write_prg);
	mem_setwrite(0x9,mapper90_write_chrlo);
	mem_setwrite(0xA,mapper90_write_chrhi);
	mem_setwrite(0xB,mapper90_write_nt);
	mem_setwrite(0xC,mapper90_write_irq);
	mem_setwrite(0xD,mapper90_write_control);
	for(i=0;i<8;i++) {
		prg[i & 3] = 0;
		chrlo[i] = 0;
		chrhi[i] = 0;
		nt[i & 3] = 0;
	}
	bankmode = 0;
	mul[0] = mul[1] = 0;
	onebyte = 0;
	irqenabled = 0;
	irqmode = 0;
	irqxor = 0;
	irqlo = irqhi = 0;
	mirror = 0;
	revision = r;
	syncprg();
	syncchr();
	syncnt();
}

void mapper90_state(int mode,u8 *data)
{
	STATE_ARRAY_U8(prg,4);
	STATE_ARRAY_U8(chrlo,8);
	STATE_ARRAY_U8(chrhi,8);
	STATE_ARRAY_U8(nt,4);
	STATE_U8(mirror);
	STATE_U8(bankmode);
	STATE_ARRAY_U8(mul,2);
	STATE_U8(onebyte);
	STATE_U8(chrcontrol);
	STATE_U8(irqenabled);
	STATE_U8(irqmode);
	STATE_U8(irqxor);
	STATE_U8(irqlo);
	STATE_U8(irqhi);
	syncprg();
	syncchr();
	syncnt();
}
