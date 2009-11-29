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
#include "nes/nes.h"
#include "nes/ppu/ppu.h"
#include "nes/ppu/io.h"
#include "nes/ppu/event.h"
#include "nes/state/state.h"
#include "nes/state/blocks.h"
#include "system/file.h"

int nes_loadstate(int fp)
{
	stateheader_t header;
	blocknes_t *bnes;
	blockcpu_t *cpu;
	blockppu_t *ppu;
	blockapu_t *apu;
	blockvram_t *vram;
	blocksvram_t *svram;
	blocksram_t *sram;
	blockwram_t *wram;
	blockmapper_t *mapper;
	blockdisk_t *disk;
	int i;
	void *block;
	int blocktype;

	//load state header
	if(blocks_loadheader(fp,&header) != 0)
		return(1);

	//enumerate thru all the state blocks
	while((block = blocks_load(fp,&blocktype)) != 0) {
		switch(blocktype) {
			case B_NES:
				bnes = (blocknes_t*)block;
				memcpy(nes->ram,bnes->ram,0x800);
				memcpy(nes->nametables[0],bnes->nametables + 0x000,0x400);
				memcpy(nes->nametables[1],bnes->nametables + 0x400,0x400);
				memcpy(nes->nametables[2],bnes->nametables + 0x800,0x400);
				memcpy(nes->nametables[3],bnes->nametables + 0xC00,0x400);
				memcpy(nes->pal,bnes->palette,32);
				nes->strobe = bnes->strobe;
				for(i=0;i<32;i++)
					pal_write(i,nes->pal[i]);
				break;
			case B_CPU:
				cpu = (blockcpu_t*)block;
				nes->cpu.a = cpu->a;
				nes->cpu.x = cpu->x;
				nes->cpu.y = cpu->y;
				nes->cpu.s = cpu->s;
				nes->cpu.f = cpu->f;
				nes->cpu.pc = cpu->pc;
				nes->cpu.totalcycles = cpu->cycles;
				nes->cpu.needirq = cpu->irq;
				break;
			case B_PPU:
				ppu = (blockppu_t*)block;
				nes->scanline = ppu->scanline;
				nes->ppu.ctrl0 = ppu->control1;
				nes->ppu.ctrl1 = ppu->control2;
				nes->ppu.status = ppu->status;
				nes->ppu.temp = ppu->temp;
				nes->ppu.scroll = ppu->scroll;
				nes->ppu.scrollx = ppu->finex;
				nes->ppu.toggle = ppu->toggle;
				nes->ppu.buf = ppu->buffer;
				nes->ppu.latch = ppu->latch;
				nes->ppu.spraddr = ppu->spraddr;
				memcpy(nes->sprmem,ppu->spriteram,0x100);
				ppucycles = ppu->cycles;
				ppuframes = ppu->frames;
				break;
			case B_APU:
				apu = (blockapu_t*)block;
				for(i=0;i<0x16;i++)
					apu_write(0x4000 + i,apu->regs[i]);
				break;
			case B_VRAM:
				vram = (blockvram_t*)block;
				if(nes->rom->vram) {
					memcpy(nes->rom->vram,vram->data,nes->rom->vramsize);
					for(i=0;i<0x2000;i++)
						ppumem_write(i,ppumem_read(i));
				}
				break;
			case B_SVRAM:
				svram = (blocksvram_t*)block;
				if(nes->rom->svram)
					memcpy(nes->rom->svram,svram->data,nes->rom->svramsize);
				break;
			case B_SRAM:
				sram = (blocksram_t*)block;
				if(nes->rom->sram)
					memcpy(nes->rom->sram,sram->data,nes->rom->sramsize);
				break;
			case B_WRAM:
				wram = (blockwram_t*)block;
				if(nes->rom->wram)
					memcpy(nes->rom->wram,wram->data,nes->rom->wramsize);
				break;
			case B_MAPR:
				mapper = (blockmapper_t*)block;
				if(nes->mapper->state)
					nes->mapper->state(STATE_LOAD,mapper->data);
				break;
			case B_DISK:
				disk = (blockdisk_t*)block;
				for(i=0;i<(nes->rom->disksides * 65500);i++)
					nes->rom->diskdata[i] = nes->rom->orig_diskdata[i] ^ disk->data[i];
				break;
		}
		blocks_free(block);
		log_message("loading block '%s'\n",blockids[blocktype]);
	}

	log_message("loaded state, %ld bytes\n",file_tell(fp));

	return(0);
}

int nes_savestate(int fp)
{
	int i;
	state_t state;

	memset(&state,0,sizeof(state_t));

	memcpy(state.nes.ram,nes->ram,0x800);
	memcpy(state.nes.nametables + 0x000,nes->nametables[0],0x400);
	memcpy(state.nes.nametables + 0x400,nes->nametables[1],0x400);
	memcpy(state.nes.nametables + 0x800,nes->nametables[2],0x400);
	memcpy(state.nes.nametables + 0xC00,nes->nametables[3],0x400);
	memcpy(state.nes.palette,nes->pal,32);
	state.nes.strobe = nes->strobe;

	state.cpu.a = nes->cpu.a;
	state.cpu.x = nes->cpu.x;
	state.cpu.y = nes->cpu.y;
	state.cpu.s = nes->cpu.s;
	state.cpu.f = nes->cpu.f;
	state.cpu.pc = nes->cpu.pc;
	state.cpu.cycles = nes->cpu.totalcycles;
	state.cpu.irq = nes->cpu.needirq;

	state.ppu.scanline = nes->scanline;
	state.ppu.control1 = nes->ppu.ctrl0;
	state.ppu.control2 = nes->ppu.ctrl1;
	state.ppu.status = nes->ppu.status;
	state.ppu.temp = nes->ppu.temp;
	state.ppu.scroll = nes->ppu.scroll;
	state.ppu.finex = nes->ppu.scrollx;
	state.ppu.toggle = nes->ppu.toggle;
	state.ppu.buffer = nes->ppu.buf;
	state.ppu.latch = nes->ppu.latch;
	state.ppu.spraddr = nes->ppu.spraddr;
	memcpy(state.ppu.spriteram,nes->sprmem,0x100);
	state.ppu.cycles = ppucycles;
	state.ppu.frames = ppuframes;

	for(i=0;i<0x16;i++)
		state.apu.regs[i] = nes->apuregs[i];

	if(nes->rom->vramsize) {
		state.vram.size = nes->rom->vramsize;
		memcpy(state.vram.data,nes->rom->vram,state.vram.size);
	}

	if(nes->rom->sramsize) {
		state.sram.size = nes->rom->sramsize;
		memcpy(state.sram.data,nes->rom->sram,state.sram.size);
	}

	if(nes->rom->diskdata) {
		state.disk.sides = nes->rom->disksides;
		for(i=0;i<(nes->rom->disksides * 65500);i++)
			state.disk.data[i] = nes->rom->diskdata[i] ^ nes->rom->orig_diskdata[i];
	}

	if(nes->mapper->state)
		nes->mapper->state(STATE_SAVE,state.mapper.data);

	//write out the save state header
	blocks_saveheader(fp);

	//write save state data
	blocks_savestate(fp,&state);

	log_message("saved state, %ld bytes\n",file_tell(fp));

	return(0);
}

int nes_savediskstate(int fp)
{
	blockdisk_t disk;
	int i;

	blocks_saveheader(fp);
	disk.sides = nes->rom->disksides;
	for(i=0;i<(nes->rom->disksides * 65500);i++)
		disk.data[i] = nes->rom->diskdata[i] ^ nes->rom->orig_diskdata[i];
	blocks_save(fp,B_DISK,&disk);
	blocks_save(fp,B_END,0);
	return(0);
}

int nes_savesramstate(int fp)
{
	blocksram_t sram;

	if((sram.size = nes->rom->sramsize) == 0)
		return(0);
	blocks_saveheader(fp);
	memcpy(sram.data,nes->rom->sram,nes->rom->sramsize);
	blocks_save(fp,B_SRAM,&sram);
	blocks_save(fp,B_END,0);
	return(0);
}

int nes_savesvramstate(int fp)
{
	blocksvram_t svram;

	if((svram.size = nes->rom->svramsize) == 0)
		return(0);
	blocks_saveheader(fp);
	memcpy(svram.data,nes->rom->svram,nes->rom->svramsize);
	blocks_save(fp,B_SVRAM,&svram);
	blocks_save(fp,B_END,0);
	return(0);
}
