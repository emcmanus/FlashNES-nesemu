#include <stdio.h>
#include <string.h>
#include "gui2/elements/menubar.h"
#include "nesemu.h"

/*

gui2 menubar object

-------------------

contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void menubar_draw(menubar_t *m)
{
	int i,x,y;

	x = m->info.x;
	y = m->info.y;
	/*
	HACK HACK HACK

	keeps the recent files menu updated
	*/
	for(i=0;i<10;i++) {
		char str[1024];
		char *fn;

		if((fn = strrchr(config.recent[i],'\\') + 1) == (char*)1)
			if((fn = strrchr(config.recent[i],'/') + 1) == (char*)1)
				fn = config.recent[i];
		sprintf(str,"%d. %s",i+1%10,fn);
		strcpy(m->menus[0].items[i].caption,str);
		m->menus[0].items[i].info.w = strlen(str) * 8;
	}
	/*
	END HACK
	*/
	load_draw(&m->load);
	video_draw(&m->video);
	input_draw(&m->input);
	gui_input_draw(&m->guiinput);
	sound_draw(&m->sound);
	devices_draw(&m->devices);
	palette_draw(&m->palette);
	options_draw(&m->options);
	mappers_draw(&m->mappers);
	paths_draw(&m->paths);
	supported_mappers_draw(&m->supported_mappers);
	tracer_draw(&m->tracer);
	rom_info_draw(&m->rom_info);
	memory_viewer_draw(&m->memory_viewer);
	nt_draw(&m->nametable_viewer);
	pt_draw(&m->patterntable_viewer);
	about_draw(&m->about);
	for(i=0;i<(m->info.h/2);i++)
		gui_draw_hline(GUI_TITLEBARBACKGROUND+i,x,y++,256);
	for(i++;i;i--)
		gui_draw_hline(GUI_TITLEBARBACKGROUND+i,x,y++,256);
	for(i=0;i<MAX_MENUS && m->menus[i].info.w;i++)
		menu_draw(&m->menus[i]);
#ifdef PS2
	i = 3;
#elif defined(WII)
	i = 2;
#else
	i = 0;
#endif
	for(;i<3;i++)
		button_draw(&m->buttons[i]);
}

menuitems_t recentitems[];
int posx = 0;
int posy = 0;
int newposx = 0;
int newposy = 0;

int menubar_event(menubar_t *m,int event,int data)
{
	int i,processed;
#ifndef PS2
	static int msx,msy;
#endif

	load_event(&m->load,event,data);
	video_event(&m->video,event,data);
	input_event(&m->input,event,data);
	sound_event(&m->sound,event,data);
	gui_input_event(&m->guiinput,event,data);
	devices_event(&m->devices,event,data);
	palette_event(&m->palette,event,data);
	options_event(&m->options,event,data);
	mappers_event(&m->mappers,event,data);
	paths_event(&m->paths,event,data);
	supported_mappers_event(&m->supported_mappers,event,data);
	rom_info_event(&m->rom_info,event,data);
	tracer_event(&m->tracer,event,data);
	memory_viewer_event(&m->memory_viewer,event,data);
	nt_event(&m->nametable_viewer,event,data);
	pt_event(&m->patterntable_viewer,event,data);
	about_event(&m->about,event,data);
	switch(event) {
		//mouse button pressed
		case E_MOUSEDOWN:
		case E_MOUSEDOWN2:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			//see if the click was on a menu
			for(processed=0,i=0;i<MAX_MENUS && m->menus[i].info.w;i++) {
				//send event to the menu and see if it processes it
				if(menu_event(&m->menus[i],event,data) > 0)
					processed = 1;
			}
			for(i=0;i<3;i++) {
				if(button_event(&m->buttons[i],event,data) > 0)
					processed = 1;
			}
			//if event was not processed by a menu or button, then move the window
			if(processed == 0 && config.windowed != 0) {
#ifndef PS2
				int x,y;
				mousemove_u mm;

				m->ismoving = 1;
				mm.data = data;
				video_getwindowpos(&x,&y);
//					break;
				log_message("window moving: ms x = %d, y = %d\n",msx,msy);
				log_message("window moving: window x = %d, y = %d\n",x,y);
				log_message("window moving: mouse x = %d, y = %d\n",mm.info.x,mm.info.y);
				log_message("window moving: mouse xrel = %d, yrel = %d\n",mm.info.xrel,mm.info.yrel);
				x += mm.info.xrel;
				y += mm.info.yrel;
//				if(x == msx && y == msy)
//					video_setwindowpos(x,y);
//				video_setwindowpos(x,y);
#endif
			}
			break;
		case E_MOUSEUP:
			m->ismoving = 0;
			posx = newposx;
			posy = newposy;
			//send event off to children
			for(i=0;i<MAX_MENUS && m->menus[i].info.w;i++)
				menu_event(&m->menus[i],event,data);
#ifndef PS2
			for(i=0;i<3;i++)
				button_event(&m->buttons[i],event,data);
#endif
			break;
		case E_MOUSEMOVE:
#ifndef PS2
			if(m->ismoving && config.windowed) {
				int x,y;
				mousemove_u mm;

				mm.data = data;
				video_getwindowpos(&x,&y);
//					break;
				log_message("window moving: ms x = %d, y = %d\n",msx,msy);
				log_message("window moving: window x = %d, y = %d\n",x,y);
				log_message("window moving: mouse x = %d, y = %d\n",mm.info.x,mm.info.y);
				log_message("window moving: mouse xrel = %d, yrel = %d\n",mm.info.xrel,mm.info.yrel);
				x += mm.info.xrel;
				y += mm.info.yrel;
//				SDL_GetRelativeMouseState(&newposx,&newposy);
				video_setwindowpos(x,y);
				break;
			}
#endif
			//send event off to children
			for(i=0;i<MAX_MENUS && m->menus[i].info.w;i++)
				menu_event(&m->menus[i],event,data);
			break;
	}
	return(0);
}

extern int quit;

void click_minimize(void *m)
{
	video_minimize();
}

extern menubar_t *root;

void click_togglefullscreen(void *m)
{
	root->video.isshowing = 0;
	video_togglefullscreen();
}

static void click_about()
{
	root->about.isshowing = 1;
}

static void click_palette()
{
	root->palette.isshowing = 1;
}

static void click_load()
{
	root->load.isshowing = 1;
}

static void click_mappers()
{
	root->mappers.isshowing = 1;
}

static void click_options()
{
	root->options.isshowing = 1;
}

static void click_paths()
{
	root->paths.isshowing = 1;
}

static void click_supported_mappers()
{
	root->supported_mappers.isshowing = 1;
}

static void click_video()
{
	int i;

	if(config.windowed == 0)
		i = config.fullscreen + 4;
	else
		i = config.windowscale - 1;
	radiogroup_event(&root->video.videomode,E_REFRESH,i);
	root->video.isshowing = 1;
}

static void click_input()
{
	root->input.isshowing = 1;
}

static void click_sound()
{
	root->sound.isshowing = 1;
}

static void click_guiinput()
{
	root->guiinput.isshowing = 1;
}

static void click_quit(void *m)
{
	quit++;
}

static void click_devices()
{
	root->devices.isshowing = 1;
}

static void click_rom_info()
{
	if((nes == 0) || (nes->rom == 0))
		return;
	root->rom_info.isshowing = 1;
	root->rom_info.info.event(&root->rom_info,E_REFRESH,0);
}

static void click_tracer()
{
	if((nes == 0) || (nes->rom == 0))
		return;
	root->tracer.isshowing = 1;
	root->tracer.pc = nes->cpu.pc - 0x1D;
	root->tracer.info.event(&root->tracer,E_REFRESH,0);
}

static void click_memory_viewer()
{
	if((nes == 0) || (nes->rom == 0))
		return;
	root->memory_viewer.isshowing = 1;
	root->memory_viewer.info.event(&root->memory_viewer,E_REFRESH,0);
}

static void click_patterntables()
{
	if((nes == 0) || (nes->rom == 0))
		return;
	root->patterntable_viewer.isshowing = 1;
	root->patterntable_viewer.info.event(&root->patterntable_viewer,E_REFRESH,0);
}

static void click_nametables()
{
	if((nes == 0) || (nes->rom == 0))
		return;
	root->nametable_viewer.isshowing = 1;
	root->nametable_viewer.info.event(&root->nametable_viewer,E_REFRESH,0);
}

static void click_softreset()
{
	if((nes == 0) || (nes->rom == 0))
		return;
	nes_reset(0);
	//deactivate the gui
	joykeys[config.gui_keys[0]] = 1;
}

static void click_hardreset()
{
	if((nes == 0) || (nes->rom == 0))
		return;
	nes_reset(1);
	//deactivate the gui
	joykeys[config.gui_keys[0]] = 1;
}

static void click_flipdisk()
{
	joykeys[config.gui_keys[0]] = 1;	//menu key
	joykeys[config.gui_keys[3]] = 1;	//disk flip key
}

static void click_savestate()
{
	savestate();
	joykeys[config.gui_keys[0]] = 1;
}

static void click_loadstate()
{
	loadstate();
	joykeys[config.gui_keys[0]] = 1;
}

static void click_savediskstate()
{
	savediskstate();
	joykeys[config.gui_keys[0]] = 1;
}

static void click_loaddiskstate()
{
	loaddiskstate();
	joykeys[config.gui_keys[0]] = 1;
}

static void loadrecent(int n)
{
	loadrom(config.recent[n]);
}

static void exit_gui()
{
	joykeys[config.gui_keys[0]] = 1;
}

extern int running;

void screenshot();

static void click_screenshot()
{
#if (!defined(PS2) && !defined(WII))
	screenshot();
#endif
}

static void click_pauseresume()
{
	running ^= 1;
}

static void click_recent0()	{	loadrecent(0);	exit_gui();	}
static void click_recent1()	{	loadrecent(1);	exit_gui();	}
static void click_recent2()	{	loadrecent(2);	exit_gui();	}
static void click_recent3()	{	loadrecent(3);	exit_gui();	}
static void click_recent4()	{	loadrecent(4);	exit_gui();	}
static void click_recent5()	{	loadrecent(5);	exit_gui();	}
static void click_recent6()	{	loadrecent(6);	exit_gui();	}
static void click_recent7()	{	loadrecent(7);	exit_gui();	}
static void click_recent8()	{	loadrecent(8);	exit_gui();	}
static void click_recent9()	{	loadrecent(9);	exit_gui();	}

static void click_freezedata()
{
	static char *freezecaptions[2] = {"Freeze Data: Off","Freeze Data: On"};

	config.recentfreeze ^= 1;
	strcpy(root->menus[0].items[11].caption,freezecaptions[config.recentfreeze & 1]);
}

static void click_clearrecent()
{
	int i;

	for(i=0;i<10;i++)
		config.recent[i][0] = 0;
}

menuitems_t recentitems[] = {
	{"1. ",click_recent0},
	{"2. ",click_recent1},
	{"3. ",click_recent2},
	{"4. ",click_recent3},
	{"5. ",click_recent4},
	{"6. ",click_recent5},
	{"7. ",click_recent6},
	{"8. ",click_recent7},
	{"9. ",click_recent8},
	{"0. ",click_recent9},
	{"",0},
	{"Freeze Data",click_freezedata},
	{"Clear Data",click_clearrecent},
	{0,0}
};

menuitems_t gameitems[] = {
	{"Load",click_load},
	{"Soft reset",click_softreset},
	{"Hard reset",click_hardreset},
	{"Flip disk",click_flipdisk},
	{"",0},
	{"Save state",click_savestate},
	{"Load state",click_loadstate},
//	{"Select save slot",0},
	{"",0},
	{"Save disk state",click_savediskstate},
	{"Load disk state",click_loaddiskstate},
#ifndef PS2
	{"",0},
	{"Quit",click_quit},
#endif
	{0,0}
};

menuitems_t configitems[] = {
	{"Devices",click_devices},
	{"Mapper",click_mappers},
	{"Palette",click_palette},
#if (defined(PS2) || defined(WII))
	{"Sound",click_sound},
	{"Options",click_options},
#else
	{"",0},
	{"Options",click_options},
	{"Paths",click_paths},
	{"",0},
	{"Video",click_video},
	{"Input",click_input},
	{"Sound",click_sound},
	{"",0},
	{"GUI Input",click_guiinput},
#endif
	{0,0}
};

menuitems_t debugitems[] = {
	{"ROM Info",click_rom_info},
	{"Tracer",click_tracer},
	{"Memory Viewer",click_memory_viewer},
	{"",0},
	{"Pattern Tables",click_patterntables},
	{"Name Tables",click_nametables},
	{"",0},
//	{"Pause/Resume",click_pauseresume},
//	{"",0},
//#ifndef PS2
//	{"Take Screenshot",click_screenshot},
//#endif
	{"Supported Mappers",click_supported_mappers},
	{"About",click_about},
	{0,0}
};

menuitems_t miscitems[] = {
	{"Codes",0},
	{"",0},
	{"Browse",0},
	{"Search",0},
	{0,0}
};

static void click_recent(void *u)
{
	menubar_t *m = (menubar_t*)u;

	m->menus[1].ispressed = 0;
	m->menus[2].ispressed = 0;
	m->menus[3].ispressed = 0;
	m->menus[4].ispressed = 0;
}

static void click_game(void *u)
{
	menubar_t *m = (menubar_t*)u;

	m->menus[0].ispressed = 0;
	m->menus[2].ispressed = 0;
	m->menus[3].ispressed = 0;
	m->menus[4].ispressed = 0;
}

static void click_config(void *u)
{
	menubar_t *m = (menubar_t*)u;

	m->menus[0].ispressed = 0;
	m->menus[1].ispressed = 0;
	m->menus[3].ispressed = 0;
	m->menus[4].ispressed = 0;
}

static void click_debug(void *u)
{
	menubar_t *m = (menubar_t*)u;

	m->menus[0].ispressed = 0;
	m->menus[1].ispressed = 0;
	m->menus[2].ispressed = 0;
	m->menus[4].ispressed = 0;
}

static void click_misc(void *u)
{
	menubar_t *m = (menubar_t*)u;

	m->menus[0].ispressed = 0;
	m->menus[1].ispressed = 0;
	m->menus[2].ispressed = 0;
	m->menus[3].ispressed = 0;
}

void menubar_create(menubar_t *m)
{
	memset(m,0,sizeof(menubar_t));

	m->info.type = G_MENUBAR;
	m->info.x = 0;
	m->info.y = 0;
	m->info.w = 256;
	m->info.h = 14;
	m->info.draw = (drawfunc_t)menubar_draw;
	m->info.event = (eventfunc_t)menubar_event;

	//this needs to be configurable, passed thru in a struct
	menu_create(&m->menus[0],"\x1",4,recentitems);
	menu_create(&m->menus[1],"Game",m->menus[0].info.x + m->menus[0].info.w + 4,gameitems);
	menu_create(&m->menus[2],"Config",m->menus[1].info.x + m->menus[1].info.w + 4,configitems);
	menu_create(&m->menus[3],"Cheat",m->menus[2].info.x + m->menus[2].info.w + 4,miscitems);
	menu_create(&m->menus[4],"Debug",m->menus[3].info.x + m->menus[3].info.w + 4,debugitems);
	button_create(&m->buttons[0],"x",(256 - 9) - 3,3,click_quit);
	button_create(&m->buttons[1],"\x9",(256 - 33) - 1,3,click_minimize);
	button_create(&m->buttons[2],"\x8",(256 - 21) - 3,3,click_togglefullscreen);
	load_create(&m->load);
	video_create(&m->video);
	input_create(&m->input);
	gui_input_create(&m->guiinput);
	sound_create(&m->sound);
	devices_create(&m->devices);
	palette_create(&m->palette);
	options_create(&m->options);
	mappers_create(&m->mappers);
	paths_create(&m->paths);
	supported_mappers_create(&m->supported_mappers);
	rom_info_create(&m->rom_info);
	tracer_create(&m->tracer);
	memory_viewer_create(&m->memory_viewer);
	nt_create(&m->nametable_viewer);
	pt_create(&m->patterntable_viewer);
	about_create(&m->about);

	m->menus[0].click = click_recent;
	m->menus[1].click = click_game;
	m->menus[2].click = click_config;
	m->menus[3].click = click_debug;
	m->menus[4].click = click_misc;
	m->menus[0].user = m;
	m->menus[1].user = m;
	m->menus[2].user = m;
	m->menus[3].user = m;
	m->menus[4].user = m;

	//'hack' to update the 'freeze data' caption
	click_freezedata();
	click_freezedata();
}
