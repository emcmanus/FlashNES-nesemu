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
#include <stdlib.h>
#include "defines.h"
#include "nes/nes.h"
#include "system/file.h"

static u32 ramtable[16] = {
	B(0),		B(128),	B(256),	B(512),
	KB(1),	KB(2),	KB(4),	KB(8),
	KB(16),	KB(32),	KB(64),	KB(128),
	KB(256),	KB(512),	MB(1),	(u32)-1,
};

static void load_ines20_header(rom_t *ret,u8 *header)
{
	//prg and chr sizes
	ret->prgsize = header[4] | ((header[9] & 0x0F) << 8);
	ret->chrsize = header[5] | ((header[9] & 0xF0) << 4);

	//work ram size
	ret->wramsize = ramtable[header[10] & 0xF];

	//battery backed ram size
	ret->sramsize = ramtable[(header[10] & 0xF0) >> 4];

	//vram size
	ret->vramsize = ramtable[header[11] & 0xF];

	//battery backed vram size
	ret->svramsize = ramtable[(header[11] & 0xF0) >> 4];

	//tv mode
	if(header[12] & 1)
		ret->pal = 1;
	else
		ret->ntsc = 1;

	//ntsc/pal compatable games
	if(header[12] & 2)
		ret->ntsc = ret->pal = 1;

	ret->vsppu = header[13] & 0xF;
	ret->vsmode = (header[13] & 0xF0) >> 4;

	//generate our masks
	ret->prgmask = rom_createmask(ret->prgsize);
	ret->chrmask = rom_createmask(ret->chrsize);
	ret->vrammask = rom_createmask(ret->vramsize);

	//assign mapper numbers
	ret->mapper = (header[6] & 0xF0) >> 4;
	ret->mapper |= (header[7] & 0xF0) << 0;
	ret->mapper |= (header[9] & 1) << 8;
	ret->submapper = header[9] >> 4;

	//find correct mirroring
	ret->mirroring = header[6] & 1;
	if(ret->mirroring & 4)
		ret->mirroring = 4;

	log_message("load_ines20_header:\n");
	log_message("   %dkb prg, %dkb chr, %dkb vram, %dkb wram, %dkbsram, %dkb bvram\n",
		ret->prgsize,ret->chrsize,ret->vramsize,ret->wramsize,ret->sramsize,ret->svramsize);
	log_message("   mapper %d.%d, %s mirroring, %s\n",
		ret->mapper,ret->submapper,
		(ret->mirroring == 4) ? "four screen" : ((ret->mirroring == 0) ? "horizontal" : "vertical"),
		(ret->ntsc && ret->pal) ? "ntsc + pal" : (ret->ntsc ? "ntsc" : "pal"));
}

rom_t *rom_load_ines20(int fd,rom_t *ret)
{
	u8 header[16];

	//read header (again...)
	file_read(fd,header,16);

	//load ines 2.0 header
	load_ines20_header(ret,header);

	//allocate memory for the prg rom
	ret->prg = (u8*)malloc(ret->prgsize);

	//read in the prg rom
	if(file_read(fd,ret->prg,ret->prgsize) != ret->prgsize) {
		log_message("rom_load: error reading prg data\n");
		rom_unload(ret);
		return(0);
	}

	//if chr exists...
	if(ret->chrsize) {

		//allocate memory for chr rom
		ret->chr = (u8*)malloc(ret->chrsize);

		//read in the chr rom data
		if(file_read(fd,ret->chr,ret->chrsize) != ret->chrsize) {
			log_message("rom_load: error reading chr data\n");
			rom_unload(ret);
			return(0);
		}

		//allocate memory for the tile cache
		ret->cache = (cache_t*)malloc(ret->chrsize);
		ret->cache_hflip = (cache_t*)malloc(ret->chrsize);

		//convert all chr tiles to cache tiles
		convert_tiles(ret->chr,ret->cache,ret->chrsize / 16,0);
		convert_tiles(ret->chr,ret->cache_hflip,ret->chrsize / 16,1);
	}

	//if no chr exists, go ahead and set default vram size of 8kb
	else
		rom_setvramsize(ret,1);

	//check if rom is in our database, and update its info
	rom_checkdb(ret,0);

	return(ret);
}
