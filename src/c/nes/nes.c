/***************************************************************************
 *   Copyright (C) 2006-2009 by Dead_Body   *
 *   jamesholodnak@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "system/config.h"
#include "nes.h"
#include "nes_io.h"
#include "nes/ppu/ppu.h"
#include "nes/ppu/io.h"
#include "nes/ppu/event.h"
#include "nes/ppu/palette.h"
#include "nes/apu/mattapu.h"
#include "nes/inputdev/joypad0.h"
#include "nes/inputdev/joypad1.h"
#include "nes/inputdev/joypad02.h"
#include "nes/inputdev/joypad13.h"
#include "nes/inputdev/zapper.h"
#include "nes/inputdev/powerpad.h"
#include "nes/inputdev/exp_arkanoid.h"
#include "nes/inputdev/exp_studykeyboard.h"
#include "nes/fds/nes_fds.h"
#include "nes/fds/hle_fds.h"

static u8 startup_pal[32] = {
	0x09,0x01,0x00,0x01,0x00,0x02,0x02,0x0D,0x08,0x10,0x08,0x24,0x00,0x00,0x04,0x2C,
	0x09,0x01,0x34,0x03,0x00,0x04,0x00,0x14,0x08,0x3A,0x00,0x02,0x00,0x20,0x2C,0x08
};

//kludge
static nes_t _nes;
nes_t *nes = &_nes;

//create a new nes object
void nes_init()
{
	//clear out the nes structure (but dont clear input device/bios pointers...HACK)
	memset(nes,0,sizeof(nes_t) - (sizeof(inputdev_t*) * 3 + sizeof(u8*) * 3));

	//palette info
	nes->hue = config.hue;
	nes->sat = config.sat;

	//initialize the 6502
	dead6502_init();

	//initialize the ppu event system
	ppu_event_init();

	//set up the apu
	nes->apu = apu_create(0,SOUND_HZ,60,16,dead6502_irq);

	//enable sound channels
	nes->apu->mix_enable = config.soundenabled ? config.soundchannels : 0;

	//set the 6502 data pointer
	dead6502_setdata(&nes->cpu);

	apu_setcontext(nes->apu);

	//generate a kevin horton palette for the video
	palette_generate(nes->hue,nes->sat);
}

void nes_kill()
{
	log_message("killing nes\n");
	apu_destroy(&nes->apu);
}

//try to load a rom into the nes
int nes_load(rom_t *r)
{
	//init the nes
	nes_init();

	//if this is a unif rom
	if(r->mapper == -1) {
		mapper_unif_t *mapper;

		if((mapper = mapper_init_unif(r->board)) == 0) {
			log_message("unif board '%s' not supported\n",r->board);
			return(1);
		}
		nes->mapper = (mapper_ines_t*)mapper;
		nes->rom = r;
		nes->rom->prgbankmask = rom_createmask(mapper->info.prg);
		nes->rom->chrbankmask = rom_createmask(mapper->info.chr);
		nes->rom->vrambankmask = rom_createmask(mapper->info.vram);
		nes->rom->srambankmask = rom_createmask(mapper->info.sram);
		if(mapper->info.vram)
			nes_setvramsize(mapper->info.vram / 8);
		if(mapper->info.sram)
			nes_setsramsize(mapper->info.sram / 4);
		log_message("unif mapper '%s' loaded\n",r->board);
	}
	else if(r->mapper == 20) {
		//disk should never be loaded if the bios is not found
		nes->mapper = (config.fdsbios ? &mapper_hle_fds : &mapper_fds);
		printf("%s mapper loaded\n",config.fdsbios ? "hle fds" : "fds");
	}
	else {
		if((nes->mapper = mapper_init_ines(r->mapper)) == 0) {
			log_message("ines mapper %d not supported\n",r->mapper);
			return(1);
		}
		log_message("ines mapper %d loaded\n",r->mapper);
	}
	nes->rom = r;
	ppu_setmirroring(nes->rom->mirroring);
	return(0);
}

void nes_unload()				//unload nes rom
{
	nes->rom = 0;
	nes->mapper = 0;
}

void nes_reset(int hard)
{
	int i;

	if(nes->rom == 0)
		return;

	//set default read/write handlers
	for(i=0;i<16;i++) {
		mem_setread(i,0);
		mem_setwrite(i,0);
	}

	//set rom read handlers (mappers might use these)
	for(i=8;i<16;i++)
		mem_setread(i,nes_read_rom);

	//ram
	mem_setreadptr(0,nes->ram);
	mem_setwriteptr(0,nes->ram);
	mem_setreadptr(1,nes->ram);
	mem_setwriteptr(1,nes->ram);

	//ppu
	mem_setread(2,ppu_read);
	mem_setwrite(2,ppu_write);
	mem_setread(3,ppu_read);
	mem_setwrite(3,ppu_write);

	//apu
	mem_setread(4,nes_read_4000);
	mem_setwrite(4,nes_write_4000);

	//mapper will set its read/write handlers
	nes->mapper->reset(hard);
	nes->frame_irqmode = 0;
	apu_reset(hard);
	ppu_reset();
	dead6502_reset();
	if(hard) {
		memset(nes->ram,0,0x800);
		for(i=0;i<32;i++)
			pal_write(i,startup_pal[i]);
	}
	log_message("nes reset ok\n");
}

int nes_frame()
{
	if(nes->inputdev[0]->update)
		nes->inputdev[0]->update();
	if(nes->inputdev[1]->update)
		nes->inputdev[1]->update();
	return(ppu_event_frame());
}

void nes_burn(u32 cycles)
{
	dead6502_burn(cycles);
}

//more kludges :(
void nes_setvramsize(int banks)
{
	rom_setvramsize(nes->rom,banks);
}

void nes_setsramsize(int banks)
{
	rom_setsramsize(nes->rom,banks);
}

static u8 read_none(u32 addr){return(0);}
static inputdev_t dev_none = {read_none,0,0,0};

void nes_setinput(int port,int type)
{
	log_message("device %d set to type %d\n",port,type);
	switch(type) {
		default:
		case 0: nes->inputdev[port] = &dev_none; break;
		case 1: nes->inputdev[port] = (port == 0) ? &dev_joypad0 : &dev_joypad1; break;
		case 2: nes->inputdev[port] = &dev_zapper; break;
		case 3: nes->inputdev[port] = &dev_powerpad; break;
//		case 4: nes->inputdev[port] = &dev_arkanoid; break;
		case 5: nes->inputdev[port] = (port == 0) ? &dev_joypad02 : &dev_joypad13; break;
	}
}

void nes_setexp(int type)
{
	log_message("expansion device set to type %d\n",type);
	switch(type) {
		default:
		case 0: nes->expdev = &dev_none; break;
		case 2: nes->expdev = &exp_arkanoid; break;
//		case 3: nes->expdev = &exp_basickeyboard; break;
		case 4: nes->expdev = &exp_studykeyboard; break;
	}
}
