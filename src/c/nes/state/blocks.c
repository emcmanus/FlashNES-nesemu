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
#include "nes/state/blocks.h"
#include "system/file.h"

#define ID MAKEID

static char stateident[] = "NST\0";
static u16 stateversion = 0x0100;

char *blockids[12] = {
	"???\0",	"NES\0",	"CPU\0",	"PPU\0",
	"APU\0",	"VRAM",	"BRAM",	"SRAM",
	"WRAM",	"MAPR",	"DISK",	"END\0"
};

static void *readblock(int fp,int size)
{
	void *ret = malloc(size);

	if(file_read(fp,ret,size) != size) {
		free(ret);
		return(0);
	}
	return(ret);
}

int blocks_loadheader(int fp,stateheader_t *header)
{
	if(file_read(fp,header,sizeof(stateheader_t)) != sizeof(stateheader_t)) {
		log_message("nes_load: error reading header data\n");
		return(1);
	}
	if(memcmp(header->ident,stateident,4) != 0) {
		log_message("nes_load: bad state ident\n");
		return(1);
	}
	if(header->version != stateversion) {
		log_message("nes_load: state version is unsupported (version = %d)\n",header->version);
		return(1);
	}
	return(0);
}

//returns block data, sets type loaded, B_UNKNOWN for unknown block, B_ERROR for errors
void *blocks_load(int fp,int *type)
{
	blockheader_t header;
	void *ret;

	//read block header
	if(file_read(fp,&header,sizeof(blockheader_t)) != sizeof(blockheader_t))
		return(0);

	switch(*((u32*)header.ident)) {
		case ID('N','E','S','\0'):
			*type = B_NES;
			if((ret = readblock(fp,sizeof(blocknes_t))) == 0)
				return(0);
			break;
		case ID('C','P','U','\0'):
			*type = B_CPU;
			if((ret = readblock(fp,sizeof(blockcpu_t))) == 0)
				return(0);
			break;
		case ID('P','P','U','\0'):
			*type = B_PPU;
			if((ret = readblock(fp,sizeof(blockppu_t))) == 0)
				return(0);
			break;
		case ID('A','P','U','\0'):
			*type = B_APU;
			if((ret = readblock(fp,sizeof(blockapu_t))) == 0)
				return(0);
			break;
		case ID('V','R','A','M'):
			*type = B_VRAM;
			if((ret = readblock(fp,sizeof(blockvram_t))) == 0)
				return(0);
			break;
		case ID('S','R','A','M'):
			*type = B_SRAM;
			if((ret = readblock(fp,sizeof(blocksram_t))) == 0)
				return(0);
			break;
		case ID('M','A','P','R'):
			*type = B_MAPR;
			if((ret = readblock(fp,sizeof(blockmapper_t))) == 0)
				return(0);
			break;
		case ID('D','I','S','K'):
			*type = B_DISK;
			if((ret = readblock(fp,sizeof(blockdisk_t))) == 0)
				return(0);
			break;
		case ID('E','N','D','\0'):
			*type = B_END;
			return(0);
		//skip unknown blocks
		default:
			file_seek(fp,header.size,SEEK_CUR);
			return(blocks_load(fp,type));
	}
	return(ret);
}

void blocks_free(void *block)
{
	if(block) {
		free(block);
	}
}

void blocks_saveheader(int fp)
{
	stateheader_t header;

	memcpy(header.ident,stateident,4);
	header.version = stateversion;
	file_write(fp,&header,sizeof(stateheader_t));
}

void blocks_save(int fp,int type,void *data)
{
	blockheader_t header;

	switch(type) {
		case B_NES: header.size = sizeof(blocknes_t); break;
		case B_CPU: header.size = sizeof(blockcpu_t); break;
		case B_PPU: header.size = sizeof(blockppu_t); break;
		case B_APU: header.size = sizeof(blockapu_t); break;
		case B_SRAM: header.size = sizeof(blocksram_t); break;
		case B_VRAM: header.size = sizeof(blockvram_t); break;
		case B_SVRAM:header.size = sizeof(blocksvram_t); break;
		case B_WRAM: header.size = sizeof(blockwram_t); break;
		case B_MAPR: header.size = sizeof(blockmapper_t); break;
		case B_DISK: header.size = sizeof(blockdisk_t); break;
		case B_END: header.size = 0; break;
		default:
			log_message("saveblock: invalid blocktype passed: %d\n",type);
			return;
	}
	//optional blocks
	switch(type) {
		case B_SRAM: if(((blocksram_t*)data)->size == 0) return; break;
		case B_VRAM: if(((blockvram_t*)data)->size == 0) return; break;
		case B_SVRAM:if(((blocksvram_t*)data)->size == 0)return; break;
		case B_WRAM: if(((blockwram_t*)data)->size == 0) return; break;
		case B_DISK: if(((blockdisk_t*)data)->sides == 0)return; break;
	}
	memcpy(header.ident,blockids[type],4);
	file_write(fp,&header,sizeof(blockheader_t));
	if(data && header.size)
		file_write(fp,data,header.size);
}

void blocks_savestate(int fp,state_t *state)
{
	blocks_save(fp,B_NES,&state->nes);
	blocks_save(fp,B_CPU,&state->cpu);
	blocks_save(fp,B_PPU,&state->ppu);
	blocks_save(fp,B_APU,&state->apu);
	blocks_save(fp,B_MAPR,&state->mapper);
	blocks_save(fp,B_VRAM,&state->vram);
	blocks_save(fp,B_SVRAM,&state->svram);
	blocks_save(fp,B_SRAM,&state->sram);
	blocks_save(fp,B_WRAM,&state->wram);
	blocks_save(fp,B_DISK,&state->disk);
	blocks_save(fp,B_END,0);
}
