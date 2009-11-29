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

#ifndef __nes_h__
#define __nes_h__

#define INPUT_A		0x01
#define INPUT_B		0x02
#define INPUT_SELECT	0x04
#define INPUT_START	0x08
#define INPUT_UP		0x10
#define INPUT_DOWN	0x20
#define INPUT_LEFT	0x40
#define INPUT_RIGHT	0x80

#include "defines.h"
#include "mappers/mapper.h"
#include "nes/rom/rom.h"
#include "nes/ppu/ppu.h"
#include "nes/apu/mattapu.h"
#include "nes/cpu/dead6502.h"
#include "nes/inputdev/inputdev.h"

#define nes_irq()		\
	dead6502_irq();

typedef struct sprite_s {
	u8 y,tile,attrib,x;
} sprite_t;

typedef struct nes_s
	{
	u8		sprmem[256];				//sprite memory
	u8		ram[0x800];					//system ram (2kb)
	u8		nametables[4][0x400];	//nametables
	u8		pal[32];						//palette

	int	scanline;			//current scanline

	dead6502data	cpu;			//cpu data
	mapper_ines_t	*mapper;		//mapper functions
	rom_t				*rom;			//ptr to current rom (specified by nes_load)
	apu_t				*apu;			//apu context
	ppu_t				ppu;			//ppu data

	u8		apuregs[0x20];			//apu register backups (for save states)

	u32	strobe;							//joy strobe
	int		frame_irqmode;					//frame irq mode

	//palette generator info
	int hue,sat;

	//bioses that are optionally loaded sometime during startup
	u8 *disksys_rom;
	u8 *hle_fds_rom;
	u8 *genie_rom;

	//input ports and expansion port
	inputdev_t *inputdev[2],*expdev;
	} nes_t;

extern nes_t *nes;

void nes_init();						//create nes object
void nes_kill();						//destroy nes object
int nes_load(rom_t *r);				//load nes rom
void nes_unload();					//unload nes rom
void nes_reset(int hard);			//reset nes
void nes_setsramsize(int banks);
void nes_setvramsize(int banks);
int nes_frame();						//process one frame
void nes_burn(u32 cycles);
void nes_setinput(int port,int type);	//set input type in port
void nes_setexp(int type);			//set expansion port input type

#endif
