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

#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"
#include "nes/rom/rom.h"
#include "nes/rom/ines.h"
#include "nes/rom/ines20.h"
#include "nes/rom/nsf.h"
#include "nes/rom/unif.h"
#include "nes/rom/fds.h"
#include "system/file.h"
#include "nes/nes.h"
#include "nes/romdb/crc.h"

static u8 nstident[] = "NST\0";
static u8 inesident[] = "NES\x1a";
static u8 fdsident[] = "FDS\x1a";
static u8 unifident[] = "UNIF";
static u8 nsfident[] = "NESM\x1a";

//thanks tnse for this, long ago
u32 rom_createmask(u32 size)
{
	u32 x,y,result;

	result = size - 1;
	for(x=0;x<32;x++) {
		y = 1 << x;
		if(y == size)
			return(result);
		if(y > size) {
			result = y - 1;
			return(result);
		}
	}
	return(0xFFFFFF);
}

rom_t *rom_load(const char *filename)
{
	int fd;						//file handle
	rom_t *ret = 0;			//rom data struct
	u8 header[16];
	char *p;

	//parse out the rom filename
	if((p = strrchr(filename,'/')) == 0) {
		if((p = strrchr(filename,'\\')) == 0) {
			p = (char*)filename;
		}
		else
			p++;
	}
	else
		p++;

	//allocate memory for rom struct
	ret = (rom_t*)malloc(sizeof(rom_t));

	//zero the rom struct
	memset(ret,0,sizeof(rom_t));

	//copy the rom filename
	strcpy(ret->filename,p);

	//try to open the file
	if((fd = file_open((char*)filename,"rb")) == -1) {
		log_message("rom_load:  cannot open rom image\n");
		rom_unload(ret);
		return(0);
	}

	log_message("loading '%s'...\n",ret->filename);

	//read in the file header
	if(file_read(fd,(u8*)header,16) != 16) {
		log_message("rom_load:  error reading file header\n");
		rom_unload(ret);
		file_close(fd);
		return(0);
	}

//hack for ps2
#ifdef PS2
	file_close(fd);
	fd = file_open((char*)filename,"rb");
#else
	//return to beginning of file
	file_seek(fd,0,SEEK_SET);
#endif

	//if the file is a state save, then try to find the rom
	if(memcmp(header,nstident,5) == 0) {
//		if(rom_load_state(fd,ret) == 0) {
//			rom_unload(ret);
			log_message("loading states as roms not implemented...yet\n");
			file_close(fd);
			return(0);
//		}
	}

	//see if it is an ines rom image
	else if(memcmp(header,inesident,4) == 0) {
		//check header for ines 2.0 or old ines format
		if((((header[7] & 0xC) == 8) && (rom_load_ines20(fd,ret) == 0)) ||
			(rom_load_ines(fd,ret) == 0)) {
			file_close(fd);
			return(0);
		}
	}

	//if the file is an fds disk image
	else if(memcmp(header,fdsident,4) == 0) {
		if(rom_load_fds(fd,ret) == 0) {
			file_close(fd);
			return(0);
		}
	}

	//if the file is an unif image
	else if(memcmp(header,unifident,4) == 0) {
		if(rom_load_unif(fd,ret) == 0) {
			file_close(fd);
			return(0);
		}
	}

	//if the file is a nsf music file
	else if(memcmp(header,nsfident,5) == 0) {
		if(rom_load_nsf(fd,ret) == 0) {
			file_close(fd);
			return(0);
		}
	}

	//unknown file type
	else {
		log_message("bad image\n");
		file_close(fd);
		return(0);
	}

	log_message("nes image '%s' loaded ok\n",ret->filename);

	//close the file and return
	file_close(fd);
	return(ret);
}

void rom_unload(rom_t *r)
{
	if(r) {
		if(r->prg)				free(r->prg);
		if(r->chr)				free(r->chr);
		if(r->cache)			free(r->cache);
		if(r->cache_hflip)	free(r->cache_hflip);
		if(r->vcache)			free(r->vcache);
		if(r->vcache_hflip)	free(r->vcache_hflip);
		if(r->vram)				free(r->vram);
		if(r->sram)				free(r->sram);
		if(r->diskdata)		free(r->diskdata);
		if(r->orig_diskdata)	free(r->orig_diskdata);
		memset(r,0,sizeof(rom_t));
		free(r);
	}
}

u32 rom_prgcrc32(rom_t *r)
{
	return(crc32(crc32(0L, 0, 0),r->prg,r->prgsize));
}

u32 rom_chrcrc32(rom_t *r)
{
	return(crc32(0,r->chr,r->chrsize));
}

u32 rom_diskcrc32(rom_t *r)
{
	return(crc32(0,r->diskdata,r->disksides * 65500));
}

#define ram_alloc(size,ptr)	(u8*)(ptr ? realloc(ptr,size) : malloc(size))

void rom_setsramsize(rom_t *r,int banks)
{
	banks *= 0x1000;

	//if sram size hasnt changed, return
	if(r->sramsize == banks)
		return;

	//set sram size and create mask
	r->sramsize = banks;
	r->srammask = rom_createmask(banks);

	//allocate (or reallocate) memory
	r->sram = ram_alloc(r->sramsize,r->sram);

	log_message("sram size set to %dk\n",banks / 1024);
}

void rom_setwramsize(rom_t *r,int banks)
{
	banks *= 0x1000;

	//if wram size hasnt changed, return
	if(r->wramsize == banks)
		return;

	//set wram size and create mask
	r->wramsize = banks;
	r->wrammask = rom_createmask(banks);

	//allocate (or reallocate) memory
	r->wram = ram_alloc(r->wramsize,r->wram);

	log_message("wram size set to %dk\n",banks);
}

void rom_setvramsize(rom_t *r,int banks)
{
	banks *= 0x2000;

	//if vram size hasnt changed, return
	if(r->vramsize == banks)
		return;

	//set vram size and create mask
	r->vramsize = banks;
	r->vrammask = rom_createmask(banks);

	//allocate (or reallocate) memory
	r->vram = ram_alloc(r->vramsize,r->vram);
	r->vcache = (cache_t*)ram_alloc(r->vramsize,r->vcache);
	r->vcache_hflip = (cache_t*)ram_alloc(r->vramsize,r->vcache_hflip);

	log_message("vram size set to %dk\n",banks);
}

int rom_checkdb(rom_t *ret,int whichdb)
{
	int i;
	u32 prgcrc,chrcrc,diskcrc;

	ret->sprite0hack = 0;

	//check rom db
	if(whichdb == 0) {
		prgcrc = rom_prgcrc32(ret);
		chrcrc = rom_chrcrc32(ret);
		for(i=0;rom_crcs[i].name;i++) {
			rom_crc_t *rc = &rom_crcs[i];

			if((rc->prgcrc == prgcrc) && ((rc->flags & ROM_NOCHR) || (rc->chrcrc == chrcrc))) {
				log_message("rom found in database:\n");
				log_message("  name: %s\n",rc->name);
				log_message("  board: %s\n",rc->board);
				strcpy(ret->name,rc->name);
				if(rc->flags & SPRITE0_HACK) {
					ret->sprite0hack = 8;
					log_message("  sprite0 hack enabled\n");
				}
				if(strcmp(rc->board,"?") != 0) {
					strcpy(ret->board,rc->board);
					ret->mapper = -1;
				}
				ret->indatabase = 1;
				return(1);
			}
		}
	}

	//check fds db
	else if(whichdb == 1) {
		diskcrc = rom_diskcrc32(ret);
		for(i=0;fds_crcs[i].name;i++) {
			rom_crc_t *rc = &fds_crcs[i];
		//look for fds disk
			if(ret->diskdata && rc->prgcrc == diskcrc) {
				log_message("disk found in database:\n");
				log_message("  name: %s\n",rc->name);
//				log_message("  year: %s\n",rc->board);
//				log_message("  manufacturer: %s\n",rc->board);
				strcpy(ret->name,rc->name);
				if(rc->flags & SPRITE0_HACK) {
					ret->sprite0hack = 8;
					log_message("  sprite0 hack enabled\n");
				}
				ret->indatabase = 2;
				return(1);				
			}
		}
	}
	strcpy(ret->name,ret->filename);
	return(0);
}
