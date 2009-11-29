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

#ifndef __rom_h__
#define __rom_h__

#include "defines.h"
#include "nes/ppu/cache.h"
#include "nes/nsf/nsfplayer.h"

#define MIRROR_H	0
#define MIRROR_V	1
#define MIRROR_1L	2
#define MIRROR_1H	4
#define MIRROR_4	8

//ines 2.0 ppu definitions
#define RP2C03B		0
#define RP2C03G		1
#define RP2C04_0001	2
#define RP2C04_0002	3
#define RP2C04_0003	4
#define RP2C04_0004	5
#define RC2C03B		6
#define RC2C03C		7
#define RC2C05_01		8
#define RC2C05_02		9
#define RC2C05_03		10
#define RC2C05_04		11
#define RC2C05_05		12

#define VS_NORMAL				0
#define VS_RBI_BASEBALL		1
#define VS_TKO_BOXING		2
#define VS_SUPER_XEVIOUS	3

typedef struct rom_s {

	//rom filename
	char		filename[512];

	//name of game (if not in database, it is the filename)
	char		name[512];

	//rom information
	u32	prgsize,chrsize;			//size of prg/chr in bytes
	u32	vramsize,svramsize;		//size of vram/svram
	u32	sramsize,wramsize;		//size of sram/wram
	u32	prgmask,chrmask;			//prg and chr masks
	u32	vrammask,svrammask;		//sram and vram masks
	u32	srammask,wrammask;		//wram (sram that is not battery backed)
	u8		*prg,*chr;					//actual prg/chr data
	u8		*vram,*svram;				//vram and svram data
	u8		*sram,*wram;				//sram and wram data

	//cache data
	cache_t	*cache,*cache_hflip;		//chr cache
	cache_t	*vcache,*vcache_hflip;	//vram cache

	//unif masks
	u32	prgbankmask,chrbankmask;	//masks created from unif info struct
	u32	vrambankmask,srambankmask;	//in lowest cpu page granularity for
													//easy conversion to higher bank sizes
	//fds information
	u8			*diskdata;					//disk data
	u8			*orig_diskdata;			//untouched disk data
	u8			disksides;					//number of sides for fds disk

	//ines mapper number, and ines 2.0 submapper
	int		mapper,submapper;

	//unif board name
	char		board[512];

	//mirroring
	int		mirroring;

	//television mode
	int		ntsc,pal;

	//vs unisystem ppu and vs unisystem mode
	int		vsppu,vsmode;

	//sprite0 hack :(
	int		sprite0hack;

	//does rom exist in our rom database?
	int		indatabase;

	//for nsf files
	nsfheader_t nsf;
} rom_t;

u32 rom_createmask(u32 size);
rom_t *rom_load(const char *filename);
void rom_unload(rom_t *r);
u32 rom_prgcrc32(rom_t *r);
u32 rom_chrcrc32(rom_t *r);
u32 rom_diskcrc32(rom_t *r);
void rom_setsramsize(rom_t *r,int banks);
void rom_setvramsize(rom_t *r,int banks);
int rom_checkdb(rom_t *ret,int whichdb);

#endif
