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
#include "nes/nes.h"
#include "nes/nsf/nsfplayer.h"
#include "system/file.h"

rom_t *rom_load_nsf(int fd,rom_t *ret)
{
	nsfheader_t header;
	long size;

	//read header
	log_message("read %d bytes of header\n",file_read(fd,&header,sizeof(nsfheader_t)));

	for(size=0;file_eof(fd)==0;) {
		u8 s;

		file_read(fd,&s,1);
		size++;
	}

	file_seek(fd,128,SEEK_SET);
	//copy header to rom struct
	memcpy(&ret->nsf,&header,sizeof(nsfheader_t));

	ret->mapper = NSF_MAPPER;
	ret->prgsize = size;
	ret->prg = (u8*)malloc(size);

	log_message("read %d (of %ld) bytes prg data\n",file_read(fd,ret->prg,size),size);
	return(ret);
}
