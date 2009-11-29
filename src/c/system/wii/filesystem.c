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
#include <dirent.h>
#include <sys/stat.h>

#include "defines.h"
#include "system/filesystem.h"

static DIR *dir;
static file_info_t file_info;

void filesystem_init()
{
}

void filesystem_kill()
{

}

void filesystem_translate_path(char *in,char *out,int outsize)
{
	strncpy(out,in,outsize);
}

static char host_path[4096];

file_info_t *filesystem_findfirst(char *path)
{
	//change into a real working path
	filesystem_translate_path(path,host_path,4096);

	if(dir) {
		closedir(dir);
		printf("directory not closed from last time used!\n");
	}
	dir = opendir(host_path);
	if(dir == 0) {
		printf("error: directory '%s' couldnt be opened\n",host_path);
		return(0);
	}

	return(filesystem_findnext());
}

file_info_t *filesystem_findnext()
{
	struct dirent *dent;
	struct stat statbuf;
	char fn[1024];

	dent = readdir(dir);

	//successful directory read
	if(dent != 0) {
		sprintf(fn,"%s%s",host_path,dent->d_name);
		if(stat(fn,&statbuf) == -1)
			return;
		file_info.name = dent->d_name;
		file_info.flags = FI_FLAG_FILE;
		if(S_ISDIR(statbuf.st_mode))
			file_info.flags = FI_FLAG_DIRECTORY;
		return(&file_info);
	}

	return(0);
}

void filesystem_findclose()
{
	closedir(dir);
	dir = 0;
}
