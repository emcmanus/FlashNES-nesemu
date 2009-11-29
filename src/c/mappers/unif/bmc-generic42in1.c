#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"

static u8 reg[2];

static void sync()
{
	if(reg[0] & 0x40)
		ppu_setmirroring(MIRROR_V);
	else
		ppu_setmirroring(MIRROR_H);
	if(reg[0] & 0x20) {
		mem_setprg16(0x8,reg[0] | ((reg[0] & 0x80) >> 2));
		mem_setprg16(0xC,reg[0] | ((reg[0] & 0x80) >> 2));
	}
	else
		mem_setprg32(0x8,(reg[0] >> 1) | ((reg[0] & 0x80) >> 3));
}

static void write_upper(u32 addr,u8 data)
{
	reg[addr & 1] = data;
	sync();
}

static void reset(int hard)
{
	int i;

	for(i=8;i<16;i++)
		mem_setwrite(i,write_upper);
	reg[0] = 0;
	reg[1] = 0;
	sync();
}

static void state(int mode,u8 *data)
{

}

MAPPER_UNIF(bmc_generic42in1,"BMC-Generic42in1",reset,0,0,state,INFO(32 + 4,8,0,2,0,0));
