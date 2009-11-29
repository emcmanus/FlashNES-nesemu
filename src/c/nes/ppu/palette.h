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

#ifndef __palette_h__
#define __palette_h__

#include "defines.h"

typedef struct palentry_s
	{
	u8 r,g,b;
	} palentry_t;

extern palentry_t ronipalette[];
extern palentry_t warmpalette[];
extern palentry_t *palette;

//palette generator from nintendulator
void GenerateNTSC(int hue,int sat);

void palette_set(palentry_t *p);  //set palette from array
void palette_generate(int hue,int sat);
void palette_setactive(int n);

#endif
