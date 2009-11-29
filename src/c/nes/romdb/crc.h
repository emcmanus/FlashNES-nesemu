#ifndef __crc_h__
#define __crc_h__

#include "defines.h"


#define ROM_WRAM8K	0x0001		//game uses 8k of non-battery ram
#define ROM_SRAM8K	0x0010		//game uses 8k of battery ram
#define SPRITE0_HACK	0x0100
#define ROM_NOCHR		0x0800		//game has no chr
#define ROM_FDS		0x1000		//indicates this is an fds disk
#define ROM_NSF		0x2000		//indicates this is an nsf file

#define INPUT_JOYPAD		0x000000		//game uses joypads (default)
#define INPUT_ZAPPER		0x010000		//game uses zapper in socket 2
#define INPUT_POWERPAD	0x020000		//game uses powerpad in socket 2
#define INPUT_ARKANOID	0x040000		//game uses arkanoid in socket 2
#define INPUT_KEYBOARD	0x080000		//study 32-in-1 keyboard
#define INPUT_KEYBOARD2	0x100000		//family basic keyboard

typedef struct rom_crc_s {
	char *name;				//game name
//	char *year;				//game year
//	char *manufacturer;	//game manufacturer
	u32 prgcrc,chrcrc;	//prg and chr crc32
	char *board;			//unif board used by game
	u32 flags;				//flags
} rom_crc_t;

extern rom_crc_t rom_crcs[];
extern rom_crc_t fds_crcs[];

#endif
