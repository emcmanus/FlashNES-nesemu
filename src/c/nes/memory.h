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

#ifndef __memory_h__
#define __memory_h__

//page read pointers/functions
void mem_setread(int page,readfunc_t readfunc);
void mem_setreadptr(int page,u8 *ptr);
readfunc_t mem_getread(int page);
u8 *mem_getreadptr(int page);

//page write pointers/functions
void mem_setwrite(int page,writefunc_t writefunc);
void mem_setwriteptr(int page,u8 *ptr);
writefunc_t mem_getwrite(int page);
u8 *mem_getwriteptr(int page);

//unset memory page
void mem_unsetcpu4(int page);
void mem_unsetcpu8(int page);

//set memory from sram inside rom struct
void mem_setsram4(int page,int rombank);
void mem_setsram8(int page,int rombank);

//set memory banks from prg
void mem_setprg4(int page,int rombank);
void mem_setprg8(int page,int rombank);
void mem_setprg16(int page,int rombank);
void mem_setprg32(int page,int rombank);

//set nametable banks
void mem_setnt(int page,int bank);

//set chr banks from chr inside rom struct
void mem_setchr1(int page,int bank);
void mem_setchr2(int page,int bank);
void mem_setchr4(int page,int bank);
void mem_setchr8(int page,int bank);

//set vram banks from vram inside rom struct
void mem_setvram1(int page,int vrambank);
void mem_setvram2(int page,int vrambank);
void mem_setvram4(int page,int vrambank);
void mem_setvram8(int page,int vrambank);

#endif
