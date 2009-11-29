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
/*
file config
saves data to config file
*/
#include <stdio.h>
#include "system/system.h"
#include "system/keys.h"
#include "version.h"

//config defaults
config_t defaults;

#define CFG_END	0
#define CFG_U32	1
#define CFG_STR	2

typedef struct configvar_s {
	void *data;
	int type;
	char *name;
	char *desc;
} configvar_t;

static configvar_t configvars[] = {
	{&config.path_rom,		CFG_STR,	"ROM Path",					"Current path to ROM images"},
	{&config.path_state,		CFG_STR,	"State Path",				"Path to state saves"},
	{&config.path_cheat,		CFG_STR,	"Cheat Path",				"Path to cheat data files"},
	{&config.path_shots,		CFG_STR,	"Screenshot Path",		"Path to screenshots"},
	{&config.fullscreen,		CFG_U32,	"Fullscreen",				"Fullscreen video mode (0=320x240,1=640x480,2=800x600,3=1024x768,4=1280x1024)"},
	{&config.windowed,		CFG_U32,	"Windowed",					"Windowed mode flag (1=windowed,0=fullscreen)"},
	{&config.windowscale,	CFG_U32,	"Window Scale",			"Windowed mode scale (1, 2, 3, or 4)"},
	{&config.filter,			CFG_U32,	"Filter",					"Video Filter"},
	{&config.fdsbios,			CFG_U32,	"FDS BIOS",					"FDS BIOS to use (0=Nintendo,1=HLE)"},
	{&config.showinfo,		CFG_U32,	"Show Info",				"Show ROM Name and FPS at top of screen during gameplay"},
	{&config.devices[0],		CFG_U32,	"Device0",					"Controller Port 1 Input Device"},
	{&config.devices[1],		CFG_U32,	"Device1",					"Controller Port 2 Input Device"},
	{&config.expdevice,		CFG_U32,	"Expansion Device",		"Famicom Expansion Port Input Device"},
	{&config.soundenabled,	CFG_U32,	"Sound Enabled",			"Sound enabled"},
	{&config.soundchannels,	CFG_U32,	"Sound Channels",			"NES Sound channels enabled (bit mask)"},
	{&config.hue,				CFG_U32,	"Palette Hue",				"Palette generator hue"},
	{&config.sat,				CFG_U32,	"Palette Saturation",	"Palette generator saturation"},
	{&config.shotformat,		CFG_U32,	"Screenshot Format",		"Screenshot Image Format (0=pcx,1=png)"},
	{&config.gui_keys[0],	CFG_U32,	"GUI Key Menu",			"GUI Key - Menu"},
	{&config.gui_keys[1],	CFG_U32,	"GUI Key Load State",	"GUI Key - Load State"},
	{&config.gui_keys[2],	CFG_U32,	"GUI Key Save State",	"GUI Key - Save State"},
	{&config.gui_keys[3],	CFG_U32,	"GUI Key Disk Flip",		"GUI Key - Disk Flip"},
	{&config.gui_keys[4],	CFG_U32,	"GUI Key Screenshot",	"GUI Key - Screenshot"},
	{&config.gui_keys[5],	CFG_U32,	"GUI Key Fast Forward",	"GUI Key - Fast Forward"},
	{&config.joy_keys[0][0],CFG_U32,	"Joypad 1 Key A",			"Joypad 1 Key - Button A"},
	{&config.joy_keys[0][1],CFG_U32,	"Joypad 1 Key B",			"Joypad 1 Key - Button B"},
	{&config.joy_keys[0][2],CFG_U32,	"Joypad 1 Key Select",	"Joypad 1 Key - Button Select"},
	{&config.joy_keys[0][3],CFG_U32,	"Joypad 1 Key Start",	"Joypad 1 Key - Button Start"},
	{&config.joy_keys[0][4],CFG_U32,	"Joypad 1 Key Up",		"Joypad 1 Key - Button Up"},
	{&config.joy_keys[0][5],CFG_U32,	"Joypad 1 Key Down",		"Joypad 1 Key - Button Down"},
	{&config.joy_keys[0][6],CFG_U32,	"Joypad 1 Key Left",		"Joypad 1 Key - Button Left"},
	{&config.joy_keys[0][7],CFG_U32,	"Joypad 1 Key Right",	"Joypad 1 Key - Button Right"},
	{&config.joy_keys[1][0],CFG_U32,	"Joypad 2 Key A",			"Joypad 2 Key - Button A"},
	{&config.joy_keys[1][1],CFG_U32,	"Joypad 2 Key B",			"Joypad 2 Key - Button B"},
	{&config.joy_keys[1][2],CFG_U32,	"Joypad 2 Key Select",	"Joypad 2 Key - Button Select"},
	{&config.joy_keys[1][3],CFG_U32,	"Joypad 2 Key Start",	"Joypad 2 Key - Button Start"},
	{&config.joy_keys[1][4],CFG_U32,	"Joypad 2 Key Up",		"Joypad 2 Key - Button Up"},
	{&config.joy_keys[1][5],CFG_U32,	"Joypad 2 Key Down",		"Joypad 2 Key - Button Down"},
	{&config.joy_keys[1][6],CFG_U32,	"Joypad 2 Key Left",		"Joypad 2 Key - Button Left"},
	{&config.joy_keys[1][7],CFG_U32,	"Joypad 2 Key Right",	"Joypad 2 Key - Button Right"},
	{&config.joy_keys[2][0],CFG_U32,	"Joypad 3 Key A",			"Joypad 3 Key - Button A"},
	{&config.joy_keys[2][1],CFG_U32,	"Joypad 3 Key B",			"Joypad 3 Key - Button B"},
	{&config.joy_keys[2][2],CFG_U32,	"Joypad 3 Key Select",	"Joypad 3 Key - Button Select"},
	{&config.joy_keys[2][3],CFG_U32,	"Joypad 3 Key Start",	"Joypad 3 Key - Button Start"},
	{&config.joy_keys[2][4],CFG_U32,	"Joypad 3 Key Up",		"Joypad 3 Key - Button Up"},
	{&config.joy_keys[2][5],CFG_U32,	"Joypad 3 Key Down",		"Joypad 3 Key - Button Down"},
	{&config.joy_keys[2][6],CFG_U32,	"Joypad 3 Key Left",		"Joypad 3 Key - Button Left"},
	{&config.joy_keys[2][7],CFG_U32,	"Joypad 3 Key Right",	"Joypad 3 Key - Button Right"},
	{&config.joy_keys[3][0],CFG_U32,	"Joypad 4 Key A",			"Joypad 4 Key - Button A"},
	{&config.joy_keys[3][1],CFG_U32,	"Joypad 4 Key B",			"Joypad 4 Key - Button B"},
	{&config.joy_keys[3][2],CFG_U32,	"Joypad 4 Key Select",	"Joypad 4 Key - Button Select"},
	{&config.joy_keys[3][3],CFG_U32,	"Joypad 4 Key Start",	"Joypad 4 Key - Button Start"},
	{&config.joy_keys[3][4],CFG_U32,	"Joypad 4 Key Up",		"Joypad 4 Key - Button Up"},
	{&config.joy_keys[3][5],CFG_U32,	"Joypad 4 Key Down",		"Joypad 4 Key - Button Down"},
	{&config.joy_keys[3][6],CFG_U32,	"Joypad 4 Key Left",		"Joypad 4 Key - Button Left"},
	{&config.joy_keys[3][7],CFG_U32,	"Joypad 4 Key Right",	"Joypad 4 Key - Button Right"},
	{&config.recent[0],CFG_STR,		"Recent File 0",			"Recent File 0"},
	{&config.recent[1],CFG_STR,		"Recent File 1",			"Recent File 1"},
	{&config.recent[2],CFG_STR,		"Recent File 2",			"Recent File 2"},
	{&config.recent[3],CFG_STR,		"Recent File 3",			"Recent File 3"},
	{&config.recent[4],CFG_STR,		"Recent File 4",			"Recent File 4"},
	{&config.recent[5],CFG_STR,		"Recent File 5",			"Recent File 5"},
	{&config.recent[6],CFG_STR,		"Recent File 6",			"Recent File 6"},
	{&config.recent[7],CFG_STR,		"Recent File 7",			"Recent File 7"},
	{&config.recent[8],CFG_STR,		"Recent File 8",			"Recent File 8"},
	{&config.recent[9],CFG_STR,		"Recent File 9",			"Recent File 9"},
	{0,0,0,0}
};

//load configuration file
int config_load()
{
	int fd;

	//set defaults
	config_defaults();

	//try to open config file
	if((fd = file_open(path_config,"rt")) == -1) {
		log_warning("cannot open config file '%s'\n",path_config);
		return(1);
	}
	while(file_eof(fd) == 0) {
		char line[512],*p,*oldp;
		char name[256],data[256];
		int i;

		file_gets(fd,line,512);
//		log_message("read line: '%s'\n",line);
		//eat whitespace from beginning of var name
		for(p = line;*p == ' ' || *p == '\t';p++);
		//comments
		if(*p == ';' || *p == '#' || *p == 0)
			continue;
		if((oldp = p = strchr(p,'=')) == 0) {
			log_warning("malformed configuration line, skipping\n");
			continue;
		}
		//eat whitespace end of var name
		for(*p--=0;*p == ' ' || *p == '\t';)
			*p-- = 0;
		//eat whitespace from var data
		for(oldp++;*oldp == ' ' || *oldp == '\t';oldp++);
		strcpy(name,line);
		strcpy(data,oldp);
		for(i=0;configvars[i].type != 0;i++) {
			//variable match
			if(strcmp(name,configvars[i].name) == 0) {
				if(configvars[i].type == CFG_U32)
					*((u32*)configvars[i].data) = atoi(data);
				else
					strcpy(((char*)configvars[i].data),data);
//				log_message("config var '%s' set to '%s'\n",name,data);
			}
		}
	}
	file_close(fd);
#ifndef PS2
	log_message("creating directories\n");
	mkdir(config.path_state);
	mkdir(config.path_cheat);
	mkdir(config.path_shots);
#endif
	return(0);
}

int config_save()
{
	int fd,i;
	char str[512];

	log_message("saving config (%s)...\n",path_config);
	if((fd = file_open(path_config,"uwt")) == -1) {
		log_error("cannot open/create config file\n");
		return(1);
	}
	sprintf(str,"; nesemu v"VERSION" configuration file\n");
	file_write(fd,str,strlen(str));
	sprintf(str,"; WARNING: the variable names are CASE SENSITIVE! (for now)\n");
	file_write(fd,str,strlen(str));
	for(i=0;configvars[i].type != 0;i++) {
		sprintf(str,"\n; %s\n",configvars[i].desc);
		file_write(fd,str,strlen(str));
		if(configvars[i].type == CFG_U32)
			sprintf(str,"%s = %d\n",configvars[i].name,*((u32*)configvars[i].data));
		else if(configvars[i].type == CFG_STR)
			sprintf(str,"%s = %s\n",configvars[i].name,((char*)configvars[i].data));
		else {
			log_error("bad config value type, skipping\n");
			continue;
		}
		file_write(fd,str,strlen(str));
	}
	file_close(fd);
 	return(0);
}
