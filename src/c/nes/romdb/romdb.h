#ifndef __romdb_h__
#define __romdb_h__

#include "defines.h"

enum regions_e {
	REGION_UNKNOWN	= 0x00,
	REGION_USA		= 0x01,
	REGION_CANADA	= 0x02,
	REGION_JAPAN	= 0x04,
	REGION_EUROPE	= 0x08,
};

enum tvformat_e {
	TV_NTSC	= 0,
	TV_PAL	= 1,
};

typedef struct romdb_rominfo_s {
	//info
	struct {
		u32	prgcrc,chrcrc;			//prg and chr crcs
		u32	flags;					//rom flags
		char	*name;					//name of the rom
	} info;

	//memory mapper information
	struct {
		struct {
			char	*board;				//board name
			u32	vramsize;			//vram size
			u32	sramsize,wramsize;//battery backed ram, work ram sizes
		} unif;
		struct {
			u16	mapper,submapper;	//mapper and submapper numbers
			u32	flags;				//ines flags (mirroring, sram)
		} ines;
	} mapper;

} romdb_rominfo_t;

typedef struct romdb_s {
	char	name[64];		//name of rom database
	char	version[64];	//romdb version
	char	author[64];		//romdb author

	int	numitems;		//number of items in db
	int	maxitems;		//max number of items in db
	romdb_rominfo_t *items;
} romdb_t;

//load rom database from file
romdb_t *romdb_load(char *fn);

//extras for inserting items/rewriting/outputting romdb
int romdb_save(char *fn,romdb_t *db);
int romdb_add();

#endif

