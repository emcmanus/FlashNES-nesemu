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

#ifndef __blocks_h__
#define __blocks_h__

#include "defines.h"

#define MAKEID(c1,c2,c3,c4) \
	((c1 << 0) | (c2 << 8) | (c3 << 16) | (c4 << 24))

//if state blocks are all compressed together
#define STATE_FLAG_GZIP		0x8000

#define B_UNKNOWN	0
#define B_NES		1
#define B_CPU		2
#define B_PPU		3
#define B_APU		4
#define B_VRAM		5
#define B_SVRAM	6
#define B_SRAM		7
#define B_WRAM		8
#define B_MAPR		9
#define B_DISK		10
#define B_END		11

//state header, version 1.00
typedef struct stateheader_s {
	char	ident[4];		//state ident
	u16	version;			//state version
	u16	flags;			//flags
} stateheader_t;

//state header, version 1.01 (supported compressed data, useful on ps2)
typedef struct stateheader101_s {
	char	ident[4];		//state ident
	u16	version;			//state version
	u16	flags;			//flags
	u32	csize,usize;	//compressed/uncompressed blocks size
} stateheader101_t;

typedef struct blockheader_s {
	char	ident[4];		//block ident
	u32	size;				//block uncompressed size
} blockheader_t;

typedef struct blocknes_s {
	//nes internals
	u8 ram[0x800];				//ram
	u8 nametables[0x1000];	//four nametables
	u8 palette[32];			//palette ram
	u8 strobe;					//joypad strobe
} blocknes_t;

typedef struct blockcpu_s {
	//cpu info
	u8 a,x,y,s,f;		//registers and flags
	u16 pc;				//program counter
	u32 cycles;			//total number of cycles executed this frame
	u8 irq;				//if nmi/irq is needed
} blockcpu_t;

typedef struct blockppu_s {
	//ppu info
	u8 control1,control2,status;	//ppu control 1 and 2, ppu status
	u16 temp,scroll;					//ppu scroll, ppu temp scroll
	u8 finex,toggle;					//ppu fine x scroll, $2005/2006 toggle
	u8 buffer,latch;					//ppu vram buffer and read latch
	u8 spraddr;							//sprite address
	u8 spriteram[0x100];				//sprite ram
	int scanline;
	u32 cycles;							//number of cycles executed this frame
	u32 frames;							//number of frames executed so far
} blockppu_t;

typedef struct blockapu_s {
	//apu info
	u8 regs[0x16];			//sound registers
} blockapu_t;

typedef struct blockvram_s {
	u32 size;				//size of vram
	u8 data[0x4000];		//vram data
} blockvram_t;

typedef struct blocksvram_s {
	u32 size;				//size of svram
	u8 data[0x4000];		//svram data
} blocksvram_t;

typedef struct blocksram_s {
	u32 size;				//size of sram
	u8 data[0x8000];		//sram data
} blocksram_t;

typedef struct blockwram_s {
	u32 size;				//size of wram
	u8 data[0x8000];		//wram data
} blockwram_t;

typedef struct blockmapper_s {
	u8 size;				//size of mapper data
	u8 data[0x100];	//mapper data
} blockmapper_t;

typedef struct blockdisk_s {
	u8 sides;				//number of 65500 byte sides
	u8 data[65500 * 8];	//disk xor data, maximum of 8 sides (4 disks)
} blockdisk_t;

typedef struct blockunknown_s {
	u8 size;
	u8 *data;
} blockunknown_t;

typedef struct state_s {
	blocknes_t nes;
	blockcpu_t cpu;
	blockppu_t ppu;
	blockapu_t apu;
	blockvram_t vram;
	blocksvram_t svram;
	blocksram_t sram;
	blockwram_t wram;
	blockmapper_t mapper;
	blockdisk_t disk;
} state_t;

extern char *blockids[];

int blocks_loadheader(int fp,stateheader_t *header);
void *blocks_load(int fp,int *type);
void blocks_free(void *block);
void blocks_saveheader(int fp);
void blocks_save(int fp,int type,void *data);
void blocks_savestate(int fp,state_t *state);

#endif
