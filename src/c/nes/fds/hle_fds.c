#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/fds/hle_fds.h"
#include "nes/fds/hle_fds_calls.h"
#include "nes/fds/nes_fds.h"

static readfunc_t read4;
static writefunc_t write4;
static u8 hle_command;

static u8 hle_read(u32 addr)
{
	if(addr > 0x4035)
		log_message("hle fds read: $%04X (pc = $%04X)\n",addr,nes->cpu.pc);
	switch(addr) {
		//fds add-on register (for hle bios)
		case 0x4039:
			log_message("hle command = %X\n",hle_command);
			return(0);
	}
	return(fds_read(addr));
}

static void hle_write(u32 addr,u8 data)
{
//	if((addr & ~1) == 0x4028 && data != 0x12 && data != 0xC && data != 0x10)
//		log_message("hle fds write: $%04X = $%02X\n",addr,data);
	switch(addr) {
		//hle command write
		case 0x4028:
			hle_command = data & 0x3F;
			return;

		//hle command execute
		case 0x4029:
			hle_calls[hle_command & 0x1F]();
			return;
	}
	fds_write(addr,data);
}

static void hle_fds_line(int line,int pcycles)
{
	fds_line(line,pcycles);
}

static void hle_fds_reset(int hard)
{
	fds_reset(hard);
	mem_setread(4,hle_read);
	mem_setwrite(4,hle_write);
	mem_setreadptr(0xE,nes->hle_fds_rom + 0x0000);
	mem_setreadptr(0xF,nes->hle_fds_rom + 0x1000);
	log_message("hle fds inited ok\n");
}

static void hle_fds_state(int mode,u8 *data)
{
	fds_state(mode,data);
}

mapper_ines_t mapper_hle_fds = {20,hle_fds_reset,0,hle_fds_line,hle_fds_state};
