#include "mappers/mapper.h"
#include "mappers/chips/mmc3.h"

static u8 reg[2];

static void sync()
{
	if(reg[0] & 0x40) {
		u8 bank = (reg[0] & 5) | ((reg[0] >> 2) & 2) | ((reg[0] >> 2) & 8);

		if(reg[0] & 2)
			mem_setprg32(8,bank >> 1);
		else {
			mem_setprg16(0x8,bank);
			mem_setprg16(0xC,bank);
		}
	}
	else
		mmc3_syncprg(0xFF,0);
	mmc3_syncchr(0xFF,0);
	mmc3_syncsram();
	mmc3_syncmirror();
}

static u8 read5(u32 addr)
{
//	log_message("h2288 protection read: $%04X\n",addr);
	return((addr >> 8 & 0xFE) | (((~addr & 1) & (addr >> 8 & 1)) ^ 1));
}

static void write5(u32 addr,u8 data)
{
	if(addr < 0x5800)
		return;
//	log_message("h2288 write: $%04X = $%02X\n",addr,data);
	if(reg[addr & 1] != data) {
		reg[addr & 1] = data;
		sync();
	}
}

static void write_security(u32 addr,u8 data)
{
	u8 security[] = {0,3,1,5,6,7,2,4};

	mmc3_write(addr,(data & 0xC0) | security[data & 7]);
}

static void reset(int hard)
{
	mem_setread(5,read5);
	mem_setwrite(5,write5);
	mmc3_init(mmc3_sync);
	mem_setwrite(8,write_security);
}

MAPPER_UNIF(unl_h2288,"UNL-H2288",
	reset,0,0,mmc3_state,
	INFO(256,256,0,0,0,0)
);

