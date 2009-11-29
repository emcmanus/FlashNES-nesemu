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

#include <windows.h>

#include <stdio.h>
#include <string.h>
//#include <dirent.h>
#include <sys/stat.h>

#include "defines.h"
#include "system/filesystem.h"

static file_info_static_t root[64];

static void filesystem_create_root()
{
	DWORD drives = GetLogicalDrives();
	int n,index = 0;

	for(n=0;n<64;n++)
		root[n].flags = 0;

	for(n=0;n<32;n++) {
		if(drives & (1 << n)) {
			root[index].flags = FI_FLAG_DIRECTORY;
			sprintf(root[index].name,"%c:",'A' + n);
			index++;
		}
	}
}

void filesystem_init()
{
	filesystem_create_root();
}

void filesystem_kill()
{
	log_message("killing filesystem...\n");
}

void filesystem_translate_path(char *in,char *out,int outsize)
{
	int i;

	if(in[0] == '/')
		strncpy(out,in + 1,outsize);
	else
		strncpy(out,in,outsize);
	for(i=0;out[i];i++) {
		if(out[i] == '/')
			out[i] = '\\';
	}
}

static HANDLE hfind;
static WIN32_FIND_DATAA wfd;
static int find_root = 0;
static file_info_t file_info;

file_info_t *filesystem_findfirst(char *path)
{
	char host_path[4096];

	//see if we are browsing the root directory
	printf("finding first in '%s'\n",path);
	if(strcmp("/",path) == 0) {
		find_root = 0;
		file_info.flags = FI_FLAG_DIRECTORY;
		file_info.name = root[find_root++].name;
	}
	else {
		//change into a real working path
		filesystem_translate_path(path,host_path,4096);

		//append search filter
		strcat(host_path,"\\*");
	printf("finding first in '%s'\n",host_path);

		hfind = FindFirstFileA(host_path,&wfd);
		if((hfind == NULL) || (strcmp(wfd.cFileName,"") == 0))
			return(0);
		file_info.flags = FI_FLAG_FILE;
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			file_info.flags = FI_FLAG_DIRECTORY;
		file_info.name = wfd.cFileName;
	}
	return(&file_info);
}

file_info_t *filesystem_findnext()
{
	if(find_root) {
		if(root[find_root].flags == 0)
			return(0);
		file_info.flags = FI_FLAG_DIRECTORY;
		file_info.name = root[find_root++].name;
	}
	else {
		if(FindNextFileA(hfind,&wfd) == 0)
			return(0);
		file_info.flags = FI_FLAG_FILE;
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			file_info.flags = FI_FLAG_DIRECTORY;
		file_info.name = wfd.cFileName;
	}
	return(&file_info);
}

void filesystem_findclose()
{
	if(find_root) {
		find_root = 0;
		return;
	}
	FindClose(hfind);
	hfind = 0;
}
