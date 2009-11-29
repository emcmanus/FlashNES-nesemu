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

#include "nes/nes.h"
#include "nes/ppu/io.h"
#include "nes/ppu/event.h"
#include "nes/ppu/palette.h"
#include "nes/ppu/cache.h"
#include "system/video.h"

u8 ppumem_read(u32 addr)
{
	if(nes->ppu.readpages[addr >> 10])
		return(nes->ppu.readpages[addr >> 10][addr & 0x3FF]);
	else if(nes->ppu.readfuncs[addr >> 10])
		return(nes->ppu.readfuncs[addr >> 10](addr));
	log_error("ppumem_read: read from unmapped memory at $%04X\n",addr);
	return(0);
}

void ppumem_write(u32 addr,u8 data)
{
	u8 page = (addr >> 10) & 0xF;

	//check if mapped to memory pointer
	if(nes->ppu.writepages[page])
		nes->ppu.writepages[page][addr & 0x3FF] = data;

	//see if write function is mapped
	else if(nes->ppu.writefuncs[page])
		nes->ppu.writefuncs[page](addr,data);

	//not mapped, report error
	else
		log_error("ppumem_write: write to unmapped memory at $%04X\n",addr);

	//if address is in chr ram, update the cache
	//TODO: make a 'dirty buffer' system
	if(addr < 0x2000) {
		if((addr & 0xF) == 0xF) {
			cache_t *cache = nes->ppu.cachepages[page];
			cache_t *cache_hflip = nes->ppu.cachepages_hflip[page];
			u8 *chr = nes->ppu.readpages[page];
			u32 a = addr & 0x3F0;

			convert_tile(chr + a,cache + (a / 8));
			convert_tile_hflip(chr + a,cache_hflip + (a / 8));
		}
	}
/*
	//check if attribute write
	else if((addr & 0x3FF) >= 0x3C0) {
		u8 nt = page & 3;
		u32 a = addr & 0x3FF;
		u8 x = (a & 7) << 2;
		u8 y = ((a >> 3) << 2) * 32;
		u8 v = (data & 3);

		nes->ppu.cacheattrib[nt][y+0+x+0] = v;
		nes->ppu.cacheattrib[nt][y+0+x+1] = v;
		nes->ppu.cacheattrib[nt][y+32+x+0] = v;
		nes->ppu.cacheattrib[nt][y+32+x+1] = v;
		v = (data & 0xc) >> 2;
		nes->ppu.cacheattrib[nt][y+0+x+2] = v;
		nes->ppu.cacheattrib[nt][y+0+x+3] = v;
		nes->ppu.cacheattrib[nt][y+32+x+2] = v;
		nes->ppu.cacheattrib[nt][y+32+x+3] = v;
		v = (data & 0x30) >> 4;
		nes->ppu.cacheattrib[nt][y+64+x+0] = v;
		nes->ppu.cacheattrib[nt][y+64+x+1] = v;
		nes->ppu.cacheattrib[nt][y+96+x+0] = v;
		nes->ppu.cacheattrib[nt][y+96+x+1] = v;
		v = (data & 0xc0) >> 6;
		nes->ppu.cacheattrib[nt][y+64+x+2] = v;
		nes->ppu.cacheattrib[nt][y+64+x+3] = v;
		nes->ppu.cacheattrib[nt][y+96+x+2] = v;
		nes->ppu.cacheattrib[nt][y+96+x+3] = v;
	}
*/
}

u8 pal_read(u32 addr)
{
	return(nes->pal[addr]);
}

void pal_write(u32 addr,u8 data)
{
	nes->pal[addr] = data;
	video_setpalentry(addr,palette[data].r,palette[data].g,palette[data].b);
}

//extern int ppulaststatusread;

u8 ppu_read(u32 addr)
{
	static u16 lastaddr = 0;
	u8 ret = 0;

	switch(addr & 7) {
		case 2:
//			ppulaststatusread = ppucycles;
//			log_message("ppulaststatusread = %d\n",ppulaststatusread);
			//vblank and nmi suppression
			if(ppucycles == 89341) {
				nes->ppu.suppressvblflag = 1;
				nes->ppu.nmi = 0;
			}
			//nmi suppress
			if(ppucycles == 0 || ppucycles == 1)
				nes->ppu.nmi = 0;
			ret = (nes->ppu.status & 0xE0) | (nes->ppu.buf & 0x1F);
			if(ret & 0x80)
				nes->ppu.status &= 0x60;	//clear vblank flag
			nes->ppu.toggle = 0;			//reset 2005/6 flipflop
			return(ret);
		case 4:
			return(nes->sprmem[nes->ppu.spraddr]);
		case 7:
//			log_message("2007 read: ppuscroll = $%04X, scanline = %d\n",nes->ppu.scroll,nes->scanline);
			nes->ppu.buf = nes->ppu.latch;
			nes->ppu.scroll &= 0x3FFF;

			//bootleg mmc3 ppu addr check
			if(((lastaddr & 0x1000) == 0) && (nes->ppu.scroll & 0x1000)) {
//				log_message("ppu read: a12 toggled! line %d, lastaddr = %04X, addr = %04X\n",nes->scanline,lastaddr,nes->ppu.scroll);
				if(nes->mapper->line)
					nes->mapper->line(1000,0);
			}
			lastaddr = nes->ppu.scroll;

			nes->ppu.latch = ppumem_read(nes->ppu.scroll);
			if((nes->ppu.scroll & 0x3F00) == 0x3F00)
				nes->ppu.buf = pal_read(nes->ppu.scroll & 0x1F);
			if(nes->ppu.ctrl0 & 4)
				nes->ppu.scroll += 32;
			else
				nes->ppu.scroll += 1;
			return(nes->ppu.buf);
	}

	return(nes->ppu.buf);
}

void ppu_write(u32 addr,u8 data)
{
	static u16 lastaddr = 0;

	switch(addr & 7) {
		case 0:				//ctrl0
			nes->ppu.ctrl0 = data;
			nes->ppu.nmi = data & 0x80;	//nmi enable flag is updated immediately
			nes->ppu.temp = (nes->ppu.temp & 0x73ff) | ((data & 3) << 10);
			return;
		case 1:				//ctrl1
			palette_setactive((data >> 5) & 7);
			nes->ppu.ctrl1 = data;
			return;
		case 3:				//spr addr
			nes->ppu.spraddr = data;
			return;
		case 4:				//spr data
			nes->sprmem[nes->ppu.spraddr++] = data;
			return;
		case 5:				//scroll
			if(nes->ppu.toggle == 0) { //first write
				nes->ppu.temp = (nes->ppu.temp & ~0x001F) | (data >> 3);
				nes->ppu.scrollx = data & 7;
				nes->ppu.toggle = 1;
			}
			else { //second write
				nes->ppu.temp &= ~0x73E0;
				nes->ppu.temp |= ((data & 0xF8) << 2) | ((data & 7) << 12);
				nes->ppu.toggle = 0;
			}
			return;
		case 6:				//vram addr
			if(nes->ppu.toggle == 0) { //first write
				nes->ppu.temp = (nes->ppu.temp & ~0xFF00) | ((data & 0x3F) << 8);
				nes->ppu.toggle = 1;
			}
			else { //second write
				nes->ppu.scroll = nes->ppu.temp = (nes->ppu.temp & ~0x00FF) | data;
//				log_message("2006 write.2: ppuscroll = $%04X, scanline = %d\n",nes->ppu.scroll,nes->scanline);
				nes->ppu.toggle = 0;
			}
			return;
		case 7:				//vram data
//			log_message("2007 write: ppuscroll = $%04X, scanline = %d\n",nes->ppu.scroll,nes->scanline);

			//bootleg mmc3 ppu addr check
			if(((lastaddr & 0x1000) == 0) && (nes->ppu.scroll & 0x1000)) {
//				log_message("ppu write: a12 toggled! line %d, lastaddr = %04X, addr = %04X\n",nes->scanline,lastaddr,nes->ppu.scroll);
				if(nes->mapper->line)
					nes->mapper->line(1000,0);
			}
			lastaddr = nes->ppu.scroll;

			if(nes->ppu.scroll < 0x3F00)	//palette write
				ppumem_write(nes->ppu.scroll,data);
			else {				//palette write
				if((nes->ppu.scroll & 0x0F) == 0) {
					int i;

					for(i=0;i<8;i++)
						pal_write(i * 4,data);
				}
				else if(nes->ppu.scroll & 3)
					pal_write(nes->ppu.scroll & 0x1F,data);
			}
			if(nes->ppu.ctrl0 & 4)
				nes->ppu.scroll += 32;
			else
				nes->ppu.scroll += 1;
			nes->ppu.scroll &= 0x3FFF;
			return;
	}
}
