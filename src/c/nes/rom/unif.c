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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defines.h"
#include "nes/rom/unif.h"
#include "nes/nes.h"
#include "nes/state/blocks.h"
#include "system/file.h"

#define makeindex(n)	\
((n >= '0' && n <= '9') ? (n - '0') :	\
	((n >= 'A' && n <= 'F') ? (n - 'A' + 10) : -1))

rom_t *rom_load_unif(int fd,rom_t *ret)
{
	u8 blockname[6] = "\0\0\0\0\0";
	u32 blocksize;
	int i,j;
	u32 prgsize[16];
	u8 *prgdata[16];
	u32 prgcrc32[16];
	u32 chrsize[16];
	u8 *chrdata[16];
	u32 chrcrc32[16];
	u8 n,header[32];
	char *wrtr;

	//clear prg/chr info
	memset(prgsize,0,sizeof(u32) * 16);
	memset(prgdata,0,sizeof(u8*) * 16);
	memset(chrsize,0,sizeof(u32) * 16);
	memset(chrdata,0,sizeof(u8*) * 16);

	//read in unif header
	file_read(fd,header,32);

	//parse each block
	while(file_eof(fd) == 0) {

		//read in block name and block size
		file_read(fd,blockname,4);
		file_read(fd,&blocksize,4);

		if(file_eof(fd) != 0)
			break;

		switch(*((u32*)&blockname[0])) {

			//unknown block
			default:
				log_message("block '%s' skipped (%d bytes)\n",blockname,blocksize);
				for(i=0;(u32)i<blocksize;i++)
					file_read(fd,&n,1);
//				file_seek(fd,blocksize,SEEK_CUR);
				break;

			//thing that wrote this unif
			case ID_WRTR:
				wrtr = (char*)malloc(blocksize);
				file_read(fd,wrtr,blocksize);
				log_message("unif file wrote by '%s'\n",wrtr);
				free(wrtr);
				break;

			//board name
			case ID_MAPR:
				if(blocksize >= 512) {
					log_message("error: board name is way too long, rom might be corrupted\n");
					rom_unload(ret);
					return(0);
				}
				ret->mapper = -1;
				file_read(fd,ret->board,blocksize);
				break;

			//mirroring flag
			case ID_MIRR:
				file_read(fd,&ret->mirroring,1);
				break;

			//prg crc32
			case ID_PCK0:case ID_PCK1:case ID_PCK2:case ID_PCK3:
			case ID_PCK4:case ID_PCK5:case ID_PCK6:case ID_PCK7:
			case ID_PCK8:case ID_PCK9:case ID_PCKA:case ID_PCKB:
			case ID_PCKC:case ID_PCKD:case ID_PCKE:case ID_PCKF:
				i = makeindex(blockname[3]);
				if(i == -1) {
					log_message("internal error\n");
					exit(0);
				}
				file_read(fd,&prgcrc32[i],blocksize);
				log_message("prg bank %d crc32 = %08X\n",i,prgcrc32[i]);
				break;

			//chr crc32
			case ID_CCK0:case ID_CCK1:case ID_CCK2:case ID_CCK3:
			case ID_CCK4:case ID_CCK5:case ID_CCK6:case ID_CCK7:
			case ID_CCK8:case ID_CCK9:case ID_CCKA:case ID_CCKB:
			case ID_CCKC:case ID_CCKD:case ID_CCKE:case ID_CCKF:
				i = makeindex(blockname[3]);
				if(i == -1) {
					log_message("internal error\n");
					exit(0);
				}
				file_read(fd,&chrcrc32[i],blocksize);
				log_message("chr bank %d crc32 = %08X\n",i,chrcrc32[i]);
				break;

			//prg data
			case ID_PRG0:case ID_PRG1:case ID_PRG2:case ID_PRG3:
			case ID_PRG4:case ID_PRG5:case ID_PRG6:case ID_PRG7:
			case ID_PRG8:case ID_PRG9:case ID_PRGA:case ID_PRGB:
			case ID_PRGC:case ID_PRGD:case ID_PRGE:case ID_PRGF:
				i = makeindex(blockname[3]);
				if(i == -1) {
					log_message("internal error\n");
					exit(0);
				}
				else if(prgsize[i]) {
					log_message("unif file error: block used twice '%s', skipping\n",blockname);
					file_seek(fd,blocksize,SEEK_CUR);
				}
				else {
					prgsize[i] = blocksize;
					prgdata[i] = (u8*)malloc(blocksize);
					file_read(fd,prgdata[i],blocksize);
				}
				break;

			//chr data
			case ID_CHR0:case ID_CHR1:case ID_CHR2:case ID_CHR3:
			case ID_CHR4:case ID_CHR5:case ID_CHR6:case ID_CHR7:
			case ID_CHR8:case ID_CHR9:case ID_CHRA:case ID_CHRB:
			case ID_CHRC:case ID_CHRD:case ID_CHRE:case ID_CHRF:
				i = makeindex(blockname[3]);
				if(i == -1) {
					log_message("internal error\n");
					exit(0);
				}
				else if(chrsize[i]) {
					log_message("unif file error: block used twice '%s', skipping\n",blockname);
					file_seek(fd,blocksize,SEEK_CUR);
				}
				else {
					chrsize[i] = blocksize;
					chrdata[i] = (u8*)malloc(blocksize);
					file_read(fd,chrdata[i],blocksize);
				}
				break;
		}
	}

	//calculate total prg size
	for(j=0,i=0;i<16;i++)
		j += prgsize[i];

	//allocate prg memory
	ret->prg = (u8*)malloc(j);
	ret->prgsize = j;
	ret->prgmask = rom_createmask(j);

	//copy prg memory and free temp data
	for(j=0,i=0;i<16;i++) {
		if(prgdata[i]) {
			memcpy(ret->prg + j,prgdata[i],prgsize[i]);
			j += prgsize[i];
			free(prgdata[i]);
			prgdata[i] = 0;
		}
	}

	//calculate total chr size
	for(j=0,i=0;i<16;i++)
		j += chrsize[i];

	//if there is chr
	if(j) {
		//allocate chr memory
		ret->chr = (u8*)malloc(j);
		ret->chrsize = j;
		ret->chrmask = rom_createmask(j);

		//copy chr memory and free temp data
		for(j=0,i=0;i<16;i++) {
			if(chrdata[i]) {
				memcpy(ret->chr + j,chrdata[i],chrsize[i]);
				j += chrsize[i];
				free(chrdata[i]);
				chrdata[i] = 0;
			}
		}

		//allocate memory for the tile cache
		ret->cache = (cache_t*)malloc(ret->chrsize);
		ret->cache_hflip = (cache_t*)malloc(ret->chrsize);

		//convert all chr tiles to cache tiles
		convert_tiles(ret->chr,ret->cache,ret->chrsize / 16,0);
		convert_tiles(ret->chr,ret->cache_hflip,ret->chrsize / 16,1);
	}

	//look for rom in database and update its mapper info
	rom_checkdb(ret,0);

	return(ret);
}
