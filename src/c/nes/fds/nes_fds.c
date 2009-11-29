#include <time.h>
#include "mappers/mapper.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"
#include "mappers/sound/s_FDS.h"
#include "system/system.h"

static apuext_t fdssound = {
	FDSsound_Load,
	FDSsound_Unload,
	FDSsound_Reset,
	FDSsound_Get,
	0,
	0
};

int fdsdrive;			//disk status indicator
u8 disknum;				//current inserted disk
static u8 writeskip;
static u8 status,control,enable,mirror;
static int diskirq;
static int diskaddr;
static int irqlatch,irqcounter;
static u8 irqenabled;
static readfunc_t read4;
static writefunc_t write4;

static u8 hle_readwrite = 1;		//default to read mode

u8 fds_read(u32 addr)
{
	u8 ret;

	if(addr < 0x4020)
		return(read4(addr));
	switch(addr) {

		//status register
		case 0x4030:
			ret = status;
			status &= ~3;
			return(ret);

		//read disk data register
		case 0x4031:
			//if there is no disk inserted, return zero
			if(disknum == 0xFF)
				return(0);

			//retreive byte to read
			ret = nes->rom->diskdata[(disknum * 65500) + diskaddr];

			//fds disk icon indicator
			fdsdrive |= 1;

			//if disk is in a valid range, increment data pointer
			if(diskaddr < 64999)
				diskaddr++;

			//setup disk irq
			diskirq = 100;

			//clear irq status bit
			status &= ~2;

			//return byte read
			return(ret);

		case 0x4032:
			ret = 0x40;

			//disk ejected bit
			if(disknum == 0xFF)
				ret |= 5;

			//disk not inserted
			if((disknum == 0xFF) || ((control & 1) == 0) || (control & 2))
				ret |= 2;

			return(ret);

		//battery status
		case 0x4033:
			return(0x80);
	}

	//else return fds sound register read
	return(FDSsound_Read(addr));
}

void fds_write(u32 addr,u8 data)
{
	if(addr < 0x4020) {
		write4(addr,data);
		return;
	}
	switch(addr) {
		default:
			FDSsound_Write(addr,data);
			break;
		case 0x4020:
			irqlatch &= 0xFF00;
			irqlatch |= data;
			status &= ~1;
			break;
		case 0x4021:
			irqlatch &= 0x00FF;
			irqlatch |= data << 8;
			status &= ~1;
			break;
		case 0x4022:
			irqenabled = data & 3;
			irqcounter = irqlatch;
			status &= ~1;
			break;
		case 0x4023:
			enable = data;
			break;
		case 0x4024:
			if((disknum != 0xFF) && ((control & 4) == 0) && (enable & 1)) {
				if((diskaddr >= 0) && (diskaddr < 65000)) {
					if(writeskip)
						writeskip--;
					else if(diskaddr >= 2) {
						nes->rom->diskdata[(disknum * 65500) + (diskaddr - 2)] = data;
						fdsdrive |= 2;
					}
				}
			}
			break;
		case 0x4025:
			status &= ~2;
			mirror = data & 8;
			if(mirror)
				ppu_setmirroring(MIRROR_H);
			else
				ppu_setmirroring(MIRROR_V);
			if(disknum == 0xFF)
				return;
			if((data & 0x40) == 0) {	//drive motor on
				if((control & 0x40) && (data & 0x10) == 0) {
					diskirq = 200;
					diskaddr -= 2;
//					log_message("diskaddr - 2 = %X (%d): $4025 write = $%02X\n",diskaddr,diskaddr,data);
				}
				if(diskaddr < 0)
					diskaddr = 0;
			}
			if((data & 4) == 0) {
				writeskip = 2;
//				log_message("writeskip set to %d\n",writeskip);
			}
			if(data & 2) {
//				log_message("diskaddr = 0: $4025 write = $%02X\n",data);
				diskaddr = 0;
				diskirq = 200;
			}
			if(data & 0x40)
				diskirq = 200;
			control = data;
			break;
	}
}

#include "system/input.h"

void fds_line(int line,int pcycles)
{
	//hacked in disk flipping code
	static int newdisknum = 0;
	static int diskflipwait = 0;

	if((line == 0)) {
		if(diskflipwait <= 0) {
			if(joykeys[config.gui_keys[3]]) {
				newdisknum = (disknum & ~1) | ((disknum ^ 1) & 1);
//				log_message("(disk ejected) disknum changed to %d\n",disknum);
				disknum = 0xFF;
				diskflipwait = 60 * 2;	//delay
			}
			else {
				disknum = newdisknum;
//				log_message("(disk inserted) disknum changed to %d\n",disknum);
			}
		}
		else
			diskflipwait--;
	}
	//end hack

	if((irqenabled & 0x2) && (irqcounter) && ((irqcounter -= 114) <= 0)) {
		if(irqenabled & 0x1)
			irqcounter = irqlatch;
		else
			irqenabled &= 1;
		status |= 1;
		dead6502_irq();
//		log_message("irq triggered, line = %d\n",line);
	}
	if((diskirq) && ((diskirq -= 114) <= 0) && (control & 0x80)) {
//		log_message("disk irq triggered, line = %d\n",line);
		status |= 2;
		dead6502_irq();
	}
}

void fds_reset(int hard)
{
	read4 = mem_getread(4);
	write4 = mem_getwrite(4);
	apu_setext(nes->apu,&fdssound);
	nes_setvramsize(1);		//8k vram
	nes_setsramsize(8);		//32k prg ram
	mem_setsram8(0x6,0);
	mem_setsram8(0x8,1);
	mem_setsram8(0xA,2);
	mem_setsram8(0xC,3);
	mem_setvram8(0,0);
	mem_setread(4,fds_read);
	mem_setwrite(4,fds_write);
	mem_setreadptr(0xE,nes->disksys_rom + 0x0000);
	mem_setreadptr(0xF,nes->disksys_rom + 0x1000);
	log_message("fds inited ok\n");
	if(hard == 0)
		return;
	disknum = 0;
	diskaddr = 0;
	writeskip = 0;
	status = 0x80;
	control = 0;
	enable = 0;
	mirror = 0;
	diskirq = 0;
	irqlatch = 0;
	irqcounter = 0;
	irqenabled = 0;
	fdsdrive = 0;
	srand((u32)time(0));
}

void fds_state(int mode,u8 *data)
{
	STATE_U8(disknum);
	STATE_U8(writeskip);
	STATE_U8(status);
	STATE_U8(control);
	STATE_U8(enable);
	STATE_U8(mirror);
	STATE_U8(irqenabled);
	STATE_U8(diskirq);
	STATE_U16(diskaddr);
	STATE_U16(irqcounter);
	STATE_U16(irqlatch);
}

mapper_ines_t mapper_fds = {20,fds_reset,0,fds_line,fds_state};
