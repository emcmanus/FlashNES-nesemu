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
#include "nes/nes.h"
#include "nes/ppu/ppu.h"

void ppu_reset()
{
	nes->ppu.spraddr = 0;
	nes->ppu.status = 0x80;
}

void ppu_setmirroring(int m)
{
	switch(m) {
		default:
		case MIRROR_H: ppu_setmirroring2(0,0,1,1); break;
		case MIRROR_V: ppu_setmirroring2(0,1,0,1); break;
		case MIRROR_1L:ppu_setmirroring2(0,0,0,0); break;
		case MIRROR_1H:ppu_setmirroring2(1,1,1,1); break;
		case MIRROR_4:	ppu_setmirroring2(0,1,2,3); break;
	}
}

void ppu_setmirroring2(int n0,int n1,int n2,int n3)
{
	ppu_setnt(0,n0);
	ppu_setnt(1,n1);
	ppu_setnt(2,n2);
	ppu_setnt(3,n3);
}

void ppu_setnt(int nt,int index)
{
	nes->ppu.readpages[nt + 0x8] = nes->ppu.writepages[nt + 0x8] =
	nes->ppu.readpages[nt + 0xC] = nes->ppu.writepages[nt + 0xC] = nes->nametables[index];
}

//ick...
void ppu_setnt_chr(int nt,int index)
{
	int offset = (index * 0x400) & nes->rom->chrmask;

	nes->ppu.readpages[nt + 0x8] = nes->ppu.readpages[nt + 0xC] = nes->rom->chr + offset;
	nes->ppu.writepages[nt + 0x8] = nes->ppu.writepages[nt + 0xC] = 0;
}
