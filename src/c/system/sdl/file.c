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

#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include "system/file.h"

typedef struct file_s {
	gzFile gzfd;
	FILE *fd;
	int flags;
} file_t;

static file_t files[16];
static int numfiles = 0;

void file_init()
{
	memset(files,0,sizeof(file_t) * 16);
	numfiles = 0;
}

void file_kill()
{
}

int file_open(char *filename,char *mode)
{
	int fd = 0;
	int uncompressed = 0;

	if(*mode == 'u') {
		uncompressed = 1;
		mode++;
	}
	if(*mode == 'z') {
		uncompressed = 0;
		mode++;
	}
	for(fd=0;fd<16;fd++) {
		if(files[fd].gzfd == 0 && files[fd].fd == 0) {
			files[fd].flags = uncompressed;
			if(uncompressed == 0) {
				if((files[fd].gzfd = gzopen(filename,mode)))
					return(fd);
			}
			else {
				if((files[fd].fd = fopen(filename,mode)))
					return(fd);
			}
		}
	}
	return(-1);
}

void file_close(int fd)
{
	if(files[fd].flags == 0)
		gzclose(files[fd].gzfd);
	else
		fclose(files[fd].fd);
	memset(&files[fd],0,sizeof(file_t));
}

int file_gets(int fd,char *buf,int maxlen)
{
	char ch;

	*buf = 0;
	for(;;) {
		int read = file_read(fd,&ch,1);
		if(read != 1 || ch == '\n' || ch == 0) {
			*buf = 0;
			return(strlen(buf));
		}
		*buf++ = ch;
	}
	return(0);
}

int file_read(int fd,void *buf,int len)
{
	if(files[fd].flags == 0)
		return(gzread(files[fd].gzfd,buf,len));
	return(fread(buf,1,len,files[fd].fd));
}

int file_write(int fd,void *buf,int len)
{
	if(files[fd].flags == 0)
		return(gzwrite(files[fd].gzfd,buf,len));
	return(fwrite(buf,1,len,files[fd].fd));
}

long file_seek(int fd,long pos,int whence)
{
	if(files[fd].flags == 0)
		return(gzseek(files[fd].gzfd,pos,whence));
	return(fseek(files[fd].fd,pos,whence));
}

long file_tell(int fd)
{
	if(files[fd].flags == 0)
		return(gztell(files[fd].gzfd));
	return(ftell(files[fd].fd));
}

int file_eof(int fd)
{
	if(files[fd].flags == 0)
		return(gzeof(files[fd].gzfd));
	return(feof(files[fd].fd));
}
