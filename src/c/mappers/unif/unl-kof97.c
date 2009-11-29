#include "mappers/chips/mmc3.h"
#include "mappers/mapper.h"
#include "nes/memory.h"

static void write_upper(u32 addr,u8 data)
{
	if(addr >= 0x8000 && addr < 0xA000) {
		data = (data & 0xD8) |
				((data & 0x20) >> 4) |
				((data & 4) << 3) |
				((data & 2) >> 1) |
				((data & 1) << 2);
		if(addr == 0x9000)
			addr = 0x8001;
		mmc3_write(addr,data);
	}
	else if(addr >= 0xC000 && addr < 0xF000) {
		data = (data & 0xD8) |
				((data & 0x20) >> 4) |
				((data & 4) << 3) |
				((data & 2) >> 1) |
				((data & 1) << 2);
		if(addr == 0xD000)
			addr = 0xC001;
		else if(addr == 0xF000)
			addr = 0xE001;
		mmc3_write(addr,data);
	}
	else
		mmc3_write(addr,data);
}

static void reset(int hard)
{
	int i;

	mmc3_init(mmc3_sync);
	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	mmc3_sync();
}

MAPPER_UNIF(unl_kof97,"UNL-KOF97",
	reset,0,0,mmc3_state,
	INFO(128,256,0,0,0,0)
);
