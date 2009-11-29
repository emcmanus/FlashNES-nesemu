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
#include <libmc.h>
#include <libhdd.h>
#include <fileio.h>
#include <fileXio_rpc.h>
#include <sys/fcntl.h>
#include "defines.h"
#include "system/filesystem.h"

#define PARTITION_NAME_ROMS	"+NES"
#define PARTITION_NAME_STATE	"+state"

static file_info_static_t root[16];

static void filesystem_create_root()
{
	int i = 0;

	memset(root,0,sizeof(file_info_static_t) * 16);

	root[i].flags = FI_FLAG_DIRECTORY;
	strcpy(root[i++].name,"mc0:");

	root[i].flags = FI_FLAG_DIRECTORY;
	strcpy(root[i++].name,"mc1:");

	root[i].flags = FI_FLAG_DIRECTORY;
	strcpy(root[i++].name,"hdd:");

	root[i].flags = FI_FLAG_DIRECTORY;
	strcpy(root[i++].name,"usb:");

	root[i].flags = FI_FLAG_DIRECTORY;
	strcpy(root[i++].name,"cd:");
}

void filesystem_init()
{
	filesystem_create_root();
}

void filesystem_kill()
{

}

void filesystem_translate_path(char *in,char *out,int outsize)
{
	printf("translate_path: in = '%s'\n",in);
	if(strncmp(in,"/hdd:",5) == 0) {
		strcpy(out,"pfs0:/");
		strcpy(out+6,in+6);
	}
	else if(strncmp(in,"/usb:",5) == 0) {
		strcpy(out,"mass:");
		strcpy(out+5,in+5);
	}
	else if(in[0] == '/')
		strcpy(out,in+1);
	else
		strcpy(out,in);
	printf("translate_path: out = '%s'\n",out);
}

static int find_root = 0;
static int find_type = 0;	//0 = root, 1 = mc, 2 = hdd, 3 = cd/dvd, 4 = host
static file_info_t file_info;
static file_info_static_t dirlist[2048];
static int dirlistnum,dirlistindex;

#define MCDIR_ENTRIES	256
static mcTable mcDir[MCDIR_ENTRIES] __attribute__((aligned(64)));
static int mc_Type, mc_Free, mc_Format, ret;
static int mcindex,mcnumfiles;

static int dirfd;

file_info_t *filesystem_findfirst(char *path)
{
	char host_path[1024],tmp[1024];

	printf("findfirst: path = '%s'\n",path);

	//see if we are browsing the root directory
	if(strcmp("/",path) == 0) {
		find_root = 0;
		find_type = 0;
		printf("findfirst: returning '%s'\n",root[find_root].name);
		file_info.flags = FI_FLAG_DIRECTORY;
		file_info.name = root[find_root++].name;
	}

	else if(strncmp("/mc",path,3) == 0) {		//memory card
		int slot = path[3] - '0';

		find_type = 1;
		printf("findfirst: memcard slot = %d\n",slot);
		printf("findfirst: memcard path = '%s'\n",path+5);
//we need to detect if it is a formatted memory card
		mcGetInfo(slot,0,&mc_Type,&mc_Free,&mc_Format);
		mcSync(0,NULL,&ret);
//		printf("mcGetInfo returned %d\n",ret);
		printf("Type: %d Free: %d Format: %d\n\n", mc_Type, mc_Free, mc_Format);

		strcpy(host_path,path+5);
		sprintf(tmp,"%s*",host_path);
		mcGetDir(slot,0,tmp,0,MCDIR_ENTRIES - 10, mcDir);
		mcSync(0,NULL,&ret);
//		printf("mcGetDir returned %d\n\nListing of root directory on memory card:\n\n", ret);

		mcindex = 0;
		mcnumfiles = ret;
		if(mcindex >= mcnumfiles)
			return(0);
		file_info.flags = (mcDir[mcindex].attrFile & MC_ATTR_SUBDIR) ? FI_FLAG_DIRECTORY : FI_FLAG_FILE;
		file_info.name = mcDir[mcindex++].name;
	}

	else if(strncmp("/hdd:",path,5) == 0) {		//hard disk partition NES
		char *partitionName = "pfs0:/";
		char dir[1024];
		iox_dirent_t infoDirEnt;
		int rv;

		printf("finding in hard drive partition '"PARTITION_NAME_ROMS"': path = '%s'\n",path+5);

		find_type = 2;
		dirlistnum = 0;
		memset(dirlist,0,sizeof(file_info_static_t) * 2048);
		rv = fileXioMount( partitionName, "hdd0:"PARTITION_NAME_ROMS, FILEXIO_MOUNT);
		if( rv == -16) {	//already mounted?
		}
		else if( rv < 0 ) {
			printf( "Mount Error: '%s' = error %d\n", "hdd0:"PARTITION_NAME_ROMS, rv );
			return(0);
		}
		printf("mounted ok\n");

		//open the directory and save the file descriptor
		sprintf(dir,"pfs0:%s",path+5);
		dirfd = fileXioDopen(dir);
//		dirfd = fileXioDopen(partitionName);
		if( dirfd < 0 ) {
			printf( "Change Directory Error: %d\n", dirfd );
			return(0);
		}
//		printf("dirlist: '%s' START\n",dir);
		while( fileXioDread( dirfd, &infoDirEnt ) > 0 ) {
			if(infoDirEnt.stat.mode == FS_TYPE_EMPTY)
				continue;
			if(strcmp(infoDirEnt.name,".") == 0)
				continue;
			if(infoDirEnt.stat.mode & 0x2000)
				dirlist[dirlistnum].flags = FI_FLAG_FILE;
			if(infoDirEnt.stat.mode & 0x1000)
				dirlist[dirlistnum].flags = FI_FLAG_DIRECTORY;
			strcpy(dirlist[dirlistnum++].name,infoDirEnt.name);
//			printf("dirlist++: (mode %X) - %s\n", infoDirEnt.stat.mode, infoDirEnt.name);
		}
//		printf("dirlist: END\n");
		fileXioDclose( dirfd );
		dirlistindex = 0;
	}

	else {
		return(0);
	}
	return(&file_info);
}

file_info_t *filesystem_findnext()
{
	if(find_root) {
		if(root[find_root].flags == 0)
			return(0);
		printf("findnext: returning '%s'\n",root[find_root].name);
		file_info.flags = FI_FLAG_DIRECTORY;
		file_info.name = root[find_root++].name;
	}

	else if(find_type == 1) {
		if(mcindex >= mcnumfiles)
			return(0);
		file_info.flags = (mcDir[mcindex].attrFile & MC_ATTR_SUBDIR) ? FI_FLAG_DIRECTORY : FI_FLAG_FILE;
		file_info.name = mcDir[mcindex++].name;
	}

	else if(find_type == 2) {
		if(dirlistindex >= dirlistnum)
			return(0);
		file_info.flags = dirlist[dirlistindex].flags;
		file_info.name = dirlist[dirlistindex++].name;
	}

	return(&file_info);
}

void filesystem_findclose()
{
	if(find_root) {
		find_root = 0;
		return;
	}
	fileXioDclose(dirfd);
}

