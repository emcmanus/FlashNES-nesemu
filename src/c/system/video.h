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

#ifndef __video_h__
#define __video_h__

#include "defines.h"

void video_init();
void video_reinit();
void video_kill();
void video_setpalentry(int i,u8 r,u8 g,u8 b);
void video_updateline(int line,u8 *s);
void video_endframe();
void video_update(u8 *s,int spitch);
//new functions added for zsnes-like gui
void video_copyscreen();
u32 video_getpixel(int x,int y);
u32 video_getpalette(int idx);
u32 *video_getscreen();
u8 *video_getnesscreen();
void video_togglefullscreen();
void video_minimize();
void video_setwindowpos(int x,int y);
void video_getwindowpos(int *x,int *y);

#endif
