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

#ifndef __ppu_h__
#define __ppu_h__

#include "defines.h"
#include "nes/rom/rom.h"

typedef struct ppu_s {
	u8		ctrl0,ctrl1;				//ppu ctrl regs
	u8		status;						//ppu status reg
	u8		toggle;						//ppu 2005/6 flipflop
	u32	temp,scroll;				//ppu temp scroll and scroll internal registers
	u8		scrollx;						//ppu x fine scroll
	u8		buf,latch;					//2007 buffer and latch
	u8		spraddr;						//sprite memory location
	u8		suppressvblflag;			//suppress vbl flag
	u8		nmi;							//nmi executed yet this frame?
	u8		*readpages[16];			//ppu read page pointers
	u8		*writepages[16];			//ppu write page pointers
	cache_t *cachepages[16];		//cache page pointers (read only)
	cache_t *cachepages_hflip[16];//hflipped cache page pointers (read only)
	readfunc_t	readfuncs[16];		//ppu read function pointers
	writefunc_t	writefuncs[16];	//ppu write function pointers
	u8		cacheattrib[4][32*32];	//attribute cache for four nametables
	u8		*attribpages[4];			//attribute cache pointers
} ppu_t;

u8 ppumem_read(u32 addr);
void ppumem_write(u32 addr,u8 data);
u8 ppu_read(u32 addr);
void ppu_write(u32 addr,u8 data);
void ppu_reset();
void ppu_line(void *dest);
void ppu_setmirroring(int m);
void ppu_setmirroring2(int n0,int n1,int n2,int n3);
void ppu_setnt(int n,int index);
void ppu_setnt_chr(int n,int index);

#endif
