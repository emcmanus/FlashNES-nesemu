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

#ifndef __mapper_h__
#define __mapper_h__

#include "defines.h"
#include "nes/memory.h"
#include "nes/nes_io.h"

//ines mapper
typedef struct mapper_ines_s {
	int num;							//mapper number
	void (*reset)(int);			//mapper reset function
	void (*tile)(u8,int);		//tile read handler
	void (*line)(int,int);		//scanline handler
	void (*state)(int,u8*);		//load/save state information
	} mapper_ines_t;

//board information
typedef struct board_info_s {
	u16	prg,chr;		//prg/chr sizes in kilobytes
	u16	sram;			//battery backed ram
	u16	wram;			//work ram
	u16	vram;			//vram
	u8		mirror;		//default mirroring
} board_info_t;

//unif mapper
typedef struct mapper_unif_s {
	int num;							//always -1
	void (*reset)(int);			//mapper reset function
	void (*tile)(u8,int);		//tile read handler
	void (*line)(int,int);		//scanline handler
	void (*state)(int,u8*);		//load/save state information
	char *board;					//board name
	board_info_t info;			//board info
	} mapper_unif_t;

#define MAPPER_INES(num,reset,tile,line,state) \
	mapper_ines_t mapper##num = {num,reset,tile,line,state}

#define INFO(prg,chr,sram,wram,vram,mirror) \
	{prg,chr,sram,wram,vram,mirror}

#define MAPPER_UNIF(sname,board,reset,tile,line,state,info) \
	mapper_unif_t mapper_##sname = {-1,reset,tile,line,state,board,info}

#define STATE_LOAD		0	//load state info into mapper
#define STATE_SAVE		1	//get state info from mapper
#define STATE_LOADCFG	2	//load config info into mapper
#define STATE_SAVECFG	3	//get config info from mapper

#define CFG_U8(dat)		\
	if(mode == STATE_LOADCFG)	\
		{(dat) = *data++;}	\
	else if(mode == STATE_SAVECFG) \
		{*data++ = (dat);}

#define STATE_U8(dat)		\
	if(mode == STATE_LOAD)	\
		{(dat) = *data++;}	\
	else if(mode == STATE_SAVE) \
		{*data++ = (dat);}

#define STATE_U16(dat)			\
	if(mode == STATE_LOAD) {	\
		(dat) = *data++;			\
		(dat) |= *data++ << 8;	\
	}									\
	else if(mode == STATE_SAVE) {					\
		*data++ = (u8)((dat) & 0xFF);				\
		*data++ = (u8)(((dat) >> 8) & 0xFF);	\
	}

#define STATE_U32(dat)			\
	if(mode == STATE_LOAD) {	\
		(dat) = *data++;			\
		(dat) |= *data++ << 8;	\
		(dat) |= *data++ << 16;	\
		(dat) |= *data++ << 24;	\
	}									\
	else if(mode == STATE_SAVE) {					\
		*data++ = (u8)((dat) & 0xFF);				\
		*data++ = (u8)(((dat) >> 8) & 0xFF);	\
		*data++ = (u8)(((dat) >> 16) & 0xFF);	\
		*data++ = (u8)(((dat) >> 24) & 0xFF);	\
	}

#define STATE_ARRAY_U8(arr,siz)	\
	{int i; for(i=0;i<(siz);i++)	\
		STATE_U8((arr)[i]); }

#define UNIF_PRGMASK4	(nes->rom->prgbankmask / 4)
#define UNIF_PRGMASK8	(nes->rom->prgbankmask / 8)
#define UNIF_PRGMASK16	(nes->rom->prgbankmask / 16)
#define UNIF_PRGMASK32	(nes->rom->prgbankmask / 32)

#define UNIF_CHRMASK1	(nes->rom->chrbankmask / 1)
#define UNIF_CHRMASK2	(nes->rom->chrbankmask / 2)
#define UNIF_CHRMASK4	(nes->rom->chrbankmask / 4)
#define UNIF_CHRMASK8	(nes->rom->chrbankmask / 8)

#include "nes/nes.h"

#define NSF_MAPPER	0x101D00D5

#ifdef __cplusplus
extern "C" {
#endif
mapper_ines_t *mapper_init_ines(int n);
mapper_unif_t *mapper_init_unif(char *board);
#ifdef __cplusplus
}
#endif

#endif
