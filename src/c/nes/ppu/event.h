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

#ifndef __ppu_event_h__
#define __ppu_event_h__

#include "defines.h"

typedef struct ppu_event_s {
	u32   timestamp;
	void  (*func)();
} ppu_event_t;

extern int ppucycles;
extern u32 ppuframes;

void ppu_event_init();
void ppu_event_kill();
int ppu_event_step();
int ppu_event_frame();

//ppu_draw.c
void ppu_draw(u8 *dest);
void ppu_drawline(u8 *dest);
void ppu_getsprites();

#endif
