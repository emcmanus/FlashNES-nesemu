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
#include "system/system.h"
#include "gui2/gui2.h"
#include "nes/state/state.h"
#include "log.h"
#include "misc.h"
#include "nes/romdb/romdb.h"
#ifdef SDL
#include <SDL/SDL.h>
#endif

//must include the path on the target system, this variable
//provides the path used when loading the config file.
//without it, config files and logs end of everywhere, along
//with the directories created by config_load().
char *argv0;

//bios rom data
u8 genie_rom[0x6000];
u8 disksys_rom[0x2000];
u8 hle_fds_rom[0x2000];
u8 nsfbios_rom[0x1000];

//bios rom filenames
static char genie_path[1024] = "genie.rom";
static char disksys_path[1024] = "disksys.rom";
static char hle_fds_path[1024] = "hle_fds.bin";
static char nsfbios_path[1024] = "nsf_bios.bin";

int gui_active = 0;
char curdir[1024];		//base directory of nesemu.exe (or on ps2 it = "mc0:/nesemu")
char path_config[1024];	//path to configuration file (set by config_defaults() or
config_t config;			//some other system dependent init call)

u8 *nesscr;

//is the emulator currently executing the nes?
int running = 1;

//is it time to quit?
int quit = 0;

static char *lines_init[] = {"Loading...","NESEMU v"VERSION,"","http://ps2.fapexpert.com","http://code.google.com/p/nesemu",0};

void init()
{
	init_system();
	file_init();
	log_init();
	log_message("inited system, file and log ok\n");
	log_message("argv0 = '%s'\n",argv0);
	log_message("initing filesystem\n");
	filesystem_init();
	config_load();
	log_message("inited config\n");
	video_init();
	log_message("initing gui\n");
	gui2_init();
	gui_draw_setscreensize(256,240);
	gui_draw_setscreen((u8*)nesscr,256+8);
	splash_start(nesscr);
	splash_adddot();
	log_message("inited video\n");
	splash_adddot();
	input_init();
	splash_adddot();
	log_message("inited input\n");
	log_message("initing sound\n");
	sound_init();
	splash_adddot();
	log_message("init done, saving config\n");
	config_save();
	splash_adddot();
	if(config.soundenabled == 0)
		apu_disable();
	splash_adddot();
}

void nesemu_kill()
{
	log_message("killing system\n");
	config_save();
	unloadrom();
	filesystem_kill();
	gui2_kill();
	log_message("killing sound...\n");
	sound_kill();
	log_message("killing input...\n");
	input_kill();
	log_message("killing video...\n");
	video_kill();
	log_message("killing logger...\n");
	log_kill();
	file_kill();
	log_message("killing system dependent stuff...\n");
	kill_system();
}

#ifdef PS2
int system_setupsavelocation(int loc);
#endif


#ifdef FLASH

	#include "AS3.h"

	int f_setup(void *data, AS3_Val args)
	{
		char *p,*rom_filename = 0;
		int i,doloadstate = 0;

		log_message("nesemu - version "VERSION"\n\n");
			
		// set running state to starting up
		running = -1;
		
		// screen buffer (currently only used by the gui)
		nesscr = (u8*)malloc(256 * (256 + 16) * sizeof(u8));
		
		memset(nesscr,0,256 * (256 + 16) * sizeof(u8));
		
		//initialize system
		init();
		
		rom_filename = "nesrom";	// ROM File Alias
		
		// Initialize nes
		nes_init();
		splash_adddot();
		
		// Set input devices
		nes_setinput(0,config.devices[0]);
		nes_setinput(1,config.devices[1]);
		nes_setexp(config.expdevice);
		splash_adddot();
		
		// No nintendo bios, hle loaded, make sure config has correct selection
		if(nes->disksys_rom == 0 && nes->hle_fds_rom != 0) config.fdsbios = 1;
		// And the opposite effect, to ensure the fds bios is ok
		if(nes->disksys_rom != 0 && nes->hle_fds_rom == 0) config.fdsbios = 0;

		log_message("trying to load rom passed as parameter\n");

		// If a rom filename was passed and that rom is loaded ok
		if(rom_filename && (loadrom(rom_filename) == 0)) {
			splash_adddot();

			// Set gui to inactive
			gui_active = 0;

			//start playing sound
			sound_play();

			//if the load state command line parameter was passed
			if(doloadstate) {
				loadstate();
				splash_adddot();
			}
		}
		else
		{
			// else activate the gui and stop the sound
			gui_active = 1;
			sound_pause();
		}
		
		splash_adddot();
		
		//debug message
		log_message("everything initialized ok, exiting setup\n");
		
		splash_stop();
		
		return 0;
	}
	int f_loop(void *data, AS3_Val args)	// Main emulation loop
	{
		// Poll the input
		input_poll();
		
		// Check os events
		check_events();
		
		// Poll gui input
		gui2_poll();
		
		// If the gui is active, draw it
		if(gui_active) {
			gui2_draw(((u8*)nesscr),256+8);
			video_update(nesscr,256+8);
		}
		
		// Or if a rom is loaded and the running state is positive
		else if(nes->rom && running) {
			// Execute a nes frame, if error exit to the gui
			if(nes_frame() == 1) {
				unloadrom();
				gui_active = 1;
			}
			else
				// Update the sound output
				sound_update();
		}
		
		return(0);
	}
	int f_teardown(void *data, AS3_Val args)
	{
		log_message("exiting...\n");
		unloadrom();
		nes_kill();
		log_message("freeing screen buffer\n");
		free(nesscr);
		nesemu_kill();
		log_message("nesemu exiting...\n");
		return(0);
	}
	
#else	// Normal main
	
	int main(int argc,char *argv[])
	{
		char *p,*rom_filename = 0;
		int i,doloadstate = 0;

		log_message("nesemu - version "VERSION"\n\n");

	/*************/{

		/*romdb_t *romdb = 0;

		char fn[2][256] = {
			"romdb/nesemu.romdb",
			"../romdb/romdb.xml",
		};
		if((romdb = romdb_load(fn[1])) == 0)
			log_error("failed to load rom database\n");
		*/

	/*************/}

		//set running state to starting up
		running = -1;

		//set our base filename
		argv0 = argv[0];

		//zero out curdir
		memset(curdir,0,1024);

		//determine current directory
	#ifdef PS2
		strcpy(curdir,"mc0:/nesemu");
	#elif defined(WII)
		strcpy(curdir,"sd:/nesemu");
	#elif (defined(LINUX) || defined(OSX))
		{
		char *homedir = getenv("HOME");

		if(homedir) {
			sprintf(curdir,"%s/.nesemu/",homedir);
			mkdir(curdir);
		}
		}
	#else
	//TODO: win32 target can use documents and settings folder, to be like
	//the cool kids, if it fails to exist, it can default on the argv0 method.
		strcpy(curdir,argv0);
	#ifdef WIN32
		//convert windows path to unix path
		for(i=0;curdir[i];i++) {
			if(curdir[i] == '\\')
				curdir[i] = '/';
		}
	#endif
	#endif
		log_message("curdir = '%s'\n",curdir);

		//safety check
		if((p = strrchr(curdir,'/')) == 0) {
			log_error("platform must include path in argv[0]\n");
			exit(0);
		}
		*p = 0;
		log_message("determined executable directory to be '%s'\n",curdir);

		//parse command line arguments (if they are given)
		if(argc > 1) {
			for(i=1;i<argc;i++) {
				if(strcmp("--mappers",argv[i]) == 0) {
					showmappers();
					return(0);
				}
			}
		}

		//screen buffer (currently only used by the gui)
		nesscr = (u8*)malloc(256 * (256 + 16) * sizeof(u8));

		memset(nesscr,0,256 * (256 + 16) * sizeof(u8));
		//initialize system
		init();

		//parse command line arguments (if they are given)
		if(argc > 1) {
			for(i=1;i<argc;i++) {
				if(strcmp("--loadstate",argv[i]) == 0)
					doloadstate = 1;
	#ifndef PS2
				else if(strcmp("--windowed",argv[i]) == 0)
					config.windowed = 1;
	#else
				else if(strcmp("--setupmc0",argv[i]) == 0)
					system_setupsavelocation(0);
				else if(strcmp("--setupmc1",argv[i]) == 0)
					system_setupsavelocation(1);
	#endif
				else
					rom_filename = argv[i];
			}
		}

		//initialize nes
		nes_init();
		splash_adddot();

		//set input devices
		nes_setinput(0,config.devices[0]);
		nes_setinput(1,config.devices[1]);
		nes_setexp(config.expdevice);
		splash_adddot();

		//try to load game genie bios
		if(load_bios(genie_path,genie_rom,0x6000) == 0)
			nes->genie_rom = genie_rom;
		splash_adddot();

		//try to load real nintendo fds bios
		if(load_bios(disksys_path,disksys_rom,0x2000) == 0)
			nes->disksys_rom = disksys_rom;
		splash_adddot();

		//try to load the hle fds bios
		if(load_bios(hle_fds_path,hle_fds_rom,0x2000) == 0)
			nes->hle_fds_rom = hle_fds_rom;
		splash_adddot();

		//try to load the nsf player bios
		if(load_bios(nsfbios_path,nsfbios_rom,0x1000) == 0)
			{}//nes->nsfbios_rom = nsfbios_rom;
		splash_adddot();

		//no nintendo bios, hle loaded, make sure config has correct selection
		if(nes->disksys_rom == 0 && nes->hle_fds_rom != 0) config.fdsbios = 1;
		//and the opposite effect, to ensure the fds bios is ok
		if(nes->disksys_rom != 0 && nes->hle_fds_rom == 0) config.fdsbios = 0;

		log_message("trying to load rom passed as parameter\n");

		//if a rom filename was passed and that rom is loaded ok
		if(rom_filename && (loadrom(rom_filename) == 0)) {
			splash_adddot();

			//set gui to inactive
			gui_active = 0;

			//start playing sound
			sound_play();

			//if the load state command line parameter was passed
			if(doloadstate) {
				loadstate();
			splash_adddot();
			}
		}

		//else activate the gui and stop the sound
		else {
			gui_active = 1;
			sound_pause();
		}
		splash_adddot();

		//debug message
		log_message("everything initialized ok, beginning main loop\n");

		splash_stop();

		//start the main loop
		while(quit == 0) {
			//poll the input
			input_poll();

			//check os events
			check_events();

			//poll gui input
			gui2_poll();

			//if the gui is active, draw it
			if(gui_active) {
				gui2_draw(((u8*)nesscr),256+8);
				video_update(nesscr,256+8);
			}

			//or if a rom is loaded and the running state is positive
			else if(nes->rom && running) {
				//execute a nes frame, if error exit to the gui
				if(nes_frame() == 1) {
					unloadrom();
					gui_active = 1;
				}
				else
					//update the sound output
					sound_update();
			}
		}
		log_message("exiting...\n");
		unloadrom();
		nes_kill();
		log_message("freeing screen buffer\n");
		free(nesscr);
		nesemu_kill();
		log_message("nesemu exiting...\n");
		return(0);
	}
#endif
