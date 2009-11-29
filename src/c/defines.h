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

#ifndef __defines_h__
#define __defines_h__

#include "version.h"

#define GZ_STATES

#ifdef WIN32
#define stricmp _stricmp
#include <direct.h>
#define mkdir _mkdir
#endif

#ifdef MEMWATCH
#include "memwatch.h"
#elif defined OSX && !defined(FLASH)
#include <memory.h>
#else
#include <malloc.h>
#endif

#ifdef PS2
#define SOUND_HZ	48000
#else
#define SOUND_HZ	44100
#endif

#ifdef __OSX__
int stricmp(char *,char *);
#endif

#define B(nn)	(nn)
#define KB(nn)	(B(nn) * 1024)
#define MB(nn)	(KB(nn) * 1024)

#include "log.h"

//includes types such as u8, s16, u64, etc...
#include "nes/cpu/dead6502.h"

#endif
