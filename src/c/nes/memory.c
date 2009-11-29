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

#include "defines.h"
#include "memory.h"
#include "nes.h"

//set read page function pointer
void mem_setread(int page,readfunc_t readfunc)
{
	nes->cpu.op6502[page] =
	nes->cpu.read6502[page] = readfunc;
}

//set read page data pointer
void mem_setreadptr(int page,u8 *ptr)
{
	nes->cpu.oppages[page] =
	nes->cpu.readpages[page] = ptr;
}

//set write page function pointer
void mem_setwrite(int page,writefunc_t writefunc)
{
	nes->cpu.write6502[page] = writefunc;
}

//set write page data pointer
void mem_setwriteptr(int page,u8 *ptr)
{
	nes->cpu.writepages[page] = ptr;
}

//retreive read page function pointer
readfunc_t mem_getread(int page)
{
	return(nes->cpu.read6502[page]);
}

//retreive read page data pointer
u8 *mem_getreadptr(int page)
{
	return(nes->cpu.readpages[page]);
}

//retreive write page function pointer
writefunc_t mem_getwrite(int page)
{
	return(nes->cpu.write6502[page]);
}

//retreive write page data pointer
u8 *mem_getwriteptr(int page)
{
	return(nes->cpu.writepages[page]);
}

void mem_unsetcpu4(int page)
{
	nes->cpu.oppages[page+0] = nes->cpu.readpages[page+0] = nes->cpu.writepages[page+0] = 0;
	nes->cpu.read6502[page+0] = 0;
	nes->cpu.write6502[page+0] = 0;
}

void mem_unsetcpu8(int page)
{
	nes->cpu.oppages[page+0] = nes->cpu.readpages[page+0] = nes->cpu.writepages[page+0] = 0;
	nes->cpu.oppages[page+1] = nes->cpu.readpages[page+1] = nes->cpu.writepages[page+0] = 0;
	nes->cpu.read6502[page+0] = 0;
	nes->cpu.write6502[page+0] = 0;
	nes->cpu.read6502[page+1] = 0;
	nes->cpu.write6502[page+1] = 0;
}

void mem_setsram4(int page,int rombank)
{
	u8 *sramptr = nes->rom->sram + ((rombank * 0x1000) & nes->rom->srammask);

	if(nes->rom->sramsize == 0)
		return;
	nes->cpu.oppages[page+0] = nes->cpu.readpages[page+0] = nes->cpu.writepages[page+0] = sramptr + 0x0000;
	nes->cpu.read6502[page+0] = nes_read_mem;
	nes->cpu.write6502[page+0] = nes_write_mem;
}

void mem_setsram8(int page,int rombank)
{
	u8 *sramptr = nes->rom->sram + ((rombank * 0x2000) & nes->rom->srammask);

	if(nes->rom->sramsize == 0)
		return;
	nes->cpu.oppages[page+0] = nes->cpu.readpages[page+0] = nes->cpu.writepages[page+0] = sramptr + 0x0000;
	nes->cpu.oppages[page+1] = nes->cpu.readpages[page+1] = nes->cpu.writepages[page+1] = sramptr + 0x1000;
	nes->cpu.read6502[page+0] = nes_read_mem;
	nes->cpu.write6502[page+0] = nes_write_mem;
	nes->cpu.read6502[page+1] = nes_read_mem;
	nes->cpu.write6502[page+1] = nes_write_mem;
}

void mem_setprg4(int page,int rombank)
{
	u8 *romptr = nes->rom->prg + ((rombank * 0x1000) & nes->rom->prgmask);

	nes->cpu.oppages[page+0] = nes->cpu.readpages[page+0] = romptr + 0x0000;
}

void mem_setprg8(int page,int rombank)
{
	u8 *romptr = nes->rom->prg + ((rombank * 0x2000) & nes->rom->prgmask);

	nes->cpu.oppages[page+0] = nes->cpu.readpages[page+0] = romptr + 0x0000;
	nes->cpu.oppages[page+1] = nes->cpu.readpages[page+1] = romptr + 0x1000;
}

void mem_setprg16(int page,int rombank)
{
	u8 *romptr = nes->rom->prg + ((rombank * 0x4000) & nes->rom->prgmask);

	nes->cpu.oppages[page+0] = nes->cpu.readpages[page+0] = romptr + 0x0000;
	nes->cpu.oppages[page+1] = nes->cpu.readpages[page+1] = romptr + 0x1000;
	nes->cpu.oppages[page+2] = nes->cpu.readpages[page+2] = romptr + 0x2000;
	nes->cpu.oppages[page+3] = nes->cpu.readpages[page+3] = romptr + 0x3000;
}

void mem_setprg32(int page,int rombank)
{
	u8 *romptr = nes->rom->prg + ((rombank * 0x8000) & nes->rom->prgmask);

	nes->cpu.oppages[page+0] = nes->cpu.readpages[page+0] = romptr + 0x0000;
	nes->cpu.oppages[page+1] = nes->cpu.readpages[page+1] = romptr + 0x1000;
	nes->cpu.oppages[page+2] = nes->cpu.readpages[page+2] = romptr + 0x2000;
	nes->cpu.oppages[page+3] = nes->cpu.readpages[page+3] = romptr + 0x3000;
	nes->cpu.oppages[page+4] = nes->cpu.readpages[page+4] = romptr + 0x4000;
	nes->cpu.oppages[page+5] = nes->cpu.readpages[page+5] = romptr + 0x5000;
	nes->cpu.oppages[page+6] = nes->cpu.readpages[page+6] = romptr + 0x6000;
	nes->cpu.oppages[page+7] = nes->cpu.readpages[page+7] = romptr + 0x7000;
}

void mem_setvram1(int page,int bank)
{
	int offset = (bank * 0x400) & nes->rom->vrammask;

	nes->ppu.readpages[page] =
	nes->ppu.writepages[page] = (u8*)nes->rom->vram + offset;
	nes->ppu.cachepages[page] = (cache_t*)((u8*)nes->rom->vcache + offset);
	nes->ppu.cachepages_hflip[page] = (cache_t*)((u8*)nes->rom->vcache_hflip + offset);
}

void mem_setvram2(int page,int bank)
{
	int i,offset = (bank * 0x800) & nes->rom->vrammask;

	for(i=0;i<2;i++) {
		nes->ppu.readpages[page+i] =
		nes->ppu.writepages[page+i] = (u8*)nes->rom->vram + offset + (i * 0x400);
		nes->ppu.cachepages[page+i] = (cache_t*)((u8*)nes->rom->vcache + offset + (i * 0x400));
		nes->ppu.cachepages_hflip[page+i] = (cache_t*)((u8*)nes->rom->vcache_hflip + offset + (i * 0x400));
	}
}

void mem_setvram4(int page,int bank)
{
	int i,offset = (bank * 0x1000) & nes->rom->vrammask;

	for(i=0;i<4;i++) {
		nes->ppu.readpages[page+i] =
		nes->ppu.writepages[page+i] = (u8*)nes->rom->vram + offset + (i * 0x400);
		nes->ppu.cachepages[page+i] = (cache_t*)((u8*)nes->rom->vcache + offset + (i * 0x400));
		nes->ppu.cachepages_hflip[page+i] = (cache_t*)((u8*)nes->rom->vcache_hflip + offset + (i * 0x400));
	}
}

void mem_setvram8(int page,int bank)
{
	int i,offset = (bank * 0x2000) & nes->rom->vrammask;

	for(i=0;i<8;i++) {
		nes->ppu.readpages[page+i] =
		nes->ppu.writepages[page+i] = (u8*)nes->rom->vram + offset + (i * 0x400);
		nes->ppu.cachepages[page+i] = (cache_t*)((u8*)nes->rom->vcache + offset + (i * 0x400));
		nes->ppu.cachepages_hflip[page+i] = (cache_t*)((u8*)nes->rom->vcache_hflip + offset + (i * 0x400));
	}
}

void mem_setnt(int page,int bank)
{
	nes->ppu.readpages[page] = nes->nametables[bank];
	nes->ppu.writepages[page] = nes->nametables[bank];
}

void mem_setchr1(int page,int bank)
{
	int offset = (bank * 0x400) & nes->rom->chrmask;

	nes->ppu.readpages[page] = nes->rom->chr + offset;
	nes->ppu.writepages[page] = 0;
	nes->ppu.cachepages[page] = (cache_t*)((u8*)nes->rom->cache + offset);
	nes->ppu.cachepages_hflip[page] = (cache_t*)((u8*)nes->rom->cache_hflip + offset);
}

void mem_setchr2(int page,int bank)
{
	int offset = (bank * 0x800) & nes->rom->chrmask;

	nes->ppu.readpages[page+0] = nes->rom->chr + offset;
	nes->ppu.writepages[page+0] = 0;
	nes->ppu.cachepages[page+0] = (cache_t*)((u8*)nes->rom->cache + offset);
	nes->ppu.cachepages_hflip[page+0] = (cache_t*)((u8*)nes->rom->cache_hflip + offset);
	nes->ppu.readpages[page+1] = nes->ppu.readpages[page+0] + 0x400;
	nes->ppu.writepages[page+1] = 0;
	nes->ppu.cachepages[page+1] = nes->ppu.cachepages[page+0] + (0x400 / 8);
	nes->ppu.cachepages_hflip[page+1] = nes->ppu.cachepages_hflip[page+0] + (0x400 / 8);
}

void mem_setchr4(int page,int bank)
{
	int offset = (bank * 0x1000) & nes->rom->chrmask;

	nes->ppu.readpages[page+0] = nes->rom->chr + offset;
	nes->ppu.writepages[page+0] = 0;
	nes->ppu.cachepages[page+0] = (cache_t*)((u8*)nes->rom->cache + offset);
	nes->ppu.cachepages_hflip[page+0] = (cache_t*)((u8*)nes->rom->cache_hflip + offset);
	nes->ppu.readpages[page+1] = nes->ppu.readpages[page+0] + 0x400;
	nes->ppu.writepages[page+1] = 0;
	nes->ppu.cachepages[page+1] = nes->ppu.cachepages[page+0] + (0x400 / 8);
	nes->ppu.cachepages_hflip[page+1] = nes->ppu.cachepages_hflip[page+0] + (0x400 / 8);
	nes->ppu.readpages[page+2] = nes->ppu.readpages[page+1] + 0x400;
	nes->ppu.writepages[page+2] = 0;
	nes->ppu.cachepages[page+2] = nes->ppu.cachepages[page+1] + (0x400 / 8);
	nes->ppu.cachepages_hflip[page+2] = nes->ppu.cachepages_hflip[page+1] + (0x400 / 8);
	nes->ppu.readpages[page+3] = nes->ppu.readpages[page+2] + 0x400;
	nes->ppu.writepages[page+3] = 0;
	nes->ppu.cachepages[page+3] = nes->ppu.cachepages[page+2] + (0x400 / 8);
	nes->ppu.cachepages_hflip[page+3] = nes->ppu.cachepages_hflip[page+2] + (0x400 / 8);
}

void mem_setchr8(int page,int bank)
{
	int i,offset = (bank * 0x2000) & nes->rom->chrmask;

	for(i=0;i<8;i++) {
		nes->ppu.readpages[page+i] = nes->rom->chr + offset + (i * 0x400);
		nes->ppu.writepages[page+i] = 0;
		nes->ppu.cachepages[page+i] = (cache_t*)((u8*)nes->rom->cache + offset + (i * 0x400));
		nes->ppu.cachepages_hflip[page+i] = (cache_t*)((u8*)nes->rom->cache_hflip + offset + (i * 0x400));
	}
}
