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
#include "nes/nes.h"
#include "nes/rom/rom.h"
#include "nes/ppu/palette.h"
#include "system/system.h"
#include "gui2/gui2.h"
#include "nes/state/state.h"
#include "log.h"
#include "misc.h"

#ifdef PS2
#define LINES2 \
	"Do not remove any memory", \
	"cards or controllers."
#else
#define LINES2 \
	"Please wait."
#endif

static char *lines_load[] = {"Loading state...",LINES2,0};
static char *lines_save[] = {"Saving state...",LINES2,0};
static char *lines_loaddiskstate[] = {"Loading disk changes...",LINES2,0};
static char *lines_savediskstate[] = {"Saving disk changes...",LINES2,0};

static rom_t *rom;

void showmappers()
{
	int i,n1,n2;
	mapper_unif_t *unif;

	log_message("supported ines mappers: 0");
	for(n1=0,i=1;i<256;i++) {
		if(mapper_init_ines(i)) {
			log_message(", %d",i);
			n1++;
		}
	}

	log_message("\nsupported unif mappers:\n");
	for(n2=0,i=0;i<1000;i++) {
		unif = mapper_init_unif(0);
		if(unif == 0)
			break;
		log_message("   %s\n",unif->board);
		n2++;
	}
	log_message("\n%d ines mappers, %d unif mappers\n",n1,n2);
}

extern u8 *nesscr;

void drawbox(char **lines)
{
	int x = 28;
	int y = 64;
	int i,numlines = 0;

	//count number of lines
	for(i=0;lines[i+1];i++)
		numlines++;

	//clear screen
	memset(nesscr,0,256*240);

	//draw window
	gui_draw_window(lines[0],x,y,256-60,11 + (numlines * 6));

	//draw text
	for(i=1;lines[i];i++)
		gui_draw_text(GUI_TEXT,x + 2,y + 4 + (i * 6),lines[i]);

	//copy old screen for neat background
	video_copyscreen();

	//update screen
	video_update(nesscr,256+8);

#ifdef PS2
	//double buffered?
	video_update(nesscr,256+8);
#endif
}

static int is_ext(char *filename,char *ext)
{
	if(stricmp(filename + strlen(filename) - strlen(ext),ext) == 0)
		return(1);
	return(0);
}

static void makestatefilename(char *filename,int type)
{
	int i;
	char *exts[] = {
		".nes", ".nes.gz",
		".unf", ".unf.gz",
		".unif",".unif.gz",
		".fds", ".fds.gz",
		".nsf", ".nsf.gz",
		0,0
	};
	char *types[] = {
		".state",
		".diskstate",
		".sramstate",
		".laststate",
		0
	};
	
	strcpy(filename,config.path_state);
	log_message("rom->filename = '%s'\n",rom->filename);
	strcat(filename,rom->filename);
	for(i=0;exts[i];i++) {
		if(is_ext(filename,exts[i])) {
			strcpy(filename + strlen(filename) - strlen(exts[i]),types[type]);
			i = -1;
			break;
		}
	}
	if(i != -1)
		strcat(filename,types[type]);
}

void loadstate()
{
	int fp;
	char filename[1024];

   sound_pause();
	drawbox(lines_load);
	makestatefilename(filename,0);
	log_message("load state: filename = '%s'\n",filename);
	if((fp = file_open(filename,"rb")) >= 0) {
		nes_loadstate(fp);
		file_close(fp);
	}
	else
		log_error("load state: error opening file '%s'\n");
   sound_play();
}

void savestate()
{
	int fp;
	char filename[1024];

	if(nes == 0 || nes->rom == 0)
		return;
   sound_pause();
	drawbox(lines_save);
	makestatefilename(filename,0);
	log_message("save state: filename = '%s'\n",filename);
	if((fp = file_open(filename,"uwb")) >= 0) {
		nes_savestate(fp);
		file_close(fp);
	}
	else
		log_error("save state: error opening/creating file '%s', please check disk space and state path\n");
   sound_play();
}

void loaddiskstate()
{
	int fp;
	char filename[1024];

	if(nes == 0 || nes->rom == 0 || nes->rom->disksides == 0)
		return;
   sound_pause();
	drawbox(lines_loaddiskstate);
	if(nes->rom->disksides == 0)
		return;
	makestatefilename(filename,1);
	strcat(filename,"d");
	log_message("load disk state: filename = '%s'\n",filename);
	if((fp = file_open(filename,"rb")) >= 0) {
		nes_loadstate(fp);
		file_close(fp);
	}
   sound_play();
}

void savediskstate()
{
	int fp;
	char filename[1024];

	if(nes == 0 || nes->rom == 0 || nes->rom->disksides == 0)
		return;
   sound_pause();
	drawbox(lines_savediskstate);
	makestatefilename(filename,1);
	log_message("save state: filename = '%s'\n",filename);
	if((fp = file_open(filename,"uwb")) >= 0) {
		nes_savediskstate(fp);
		file_close(fp);
	}
   sound_play();
}

/////////////////////////////////////////////

static void add_recent(char *fn)
{
//	int i;
	char str[1024];

	if(config.recentfreeze & 1)
		return;
	strcpy(str,fn);
	strcpy(config.recent[9],config.recent[8]);
	strcpy(config.recent[8],config.recent[7]);
	strcpy(config.recent[7],config.recent[6]);
	strcpy(config.recent[6],config.recent[5]);
	strcpy(config.recent[5],config.recent[4]);
	strcpy(config.recent[4],config.recent[3]);
	strcpy(config.recent[3],config.recent[2]);
	strcpy(config.recent[2],config.recent[1]);
	strcpy(config.recent[1],config.recent[0]);
	strcpy(config.recent[0],str);
	config_save();
}

int loadrom(char *fn)
{
	log_message("loadrom: loading rom '%s'\n",fn);
	unloadrom();
	if(((rom = rom_load(fn)) == 0) || (nes_load(rom) != 0)) {
		log_message("loadrom: error loading rom '%s'\n",fn);
		return(1);
	}
	if(rom->diskdata)
		loaddiskstate();
//	if(config.autostates)
//		loadstate();
	nes_reset(1);	//perform hard reset
	add_recent(fn);
	return(0);
}

void unloadrom()
{
//	if(config.autostates)
//		savestate();
	log_message("unloadrom: unloading rom\n");
	nes_unload();
	if(rom) {
		if(rom->diskdata)
			savediskstate();
		rom_unload(rom);
		rom = 0;
	}
}

extern char curdir[];

int load_bios(char *fn,u8 *dest,int size)
{
	int fp;
	char tmp[1024];

	sprintf(tmp,"%s/%s",curdir,fn);
	if((fp = file_open(tmp,"rb")) == -1) {
		strcat(fn,".gz");
		if((fp = file_open(tmp,"rb")) == -1) {
			log_message("failed to load bios '%s'\n",fn);
			return(1);
		}
	}
	if(file_read(fp,dest,size) != (size)) {
		log_message("failed to load '%s', rom is too small\n",fn);
		return(2);
	}
	file_close(fp);
	log_message("found '%s'\n",fn);
	return(0);
}
