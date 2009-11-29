#ifndef __gui2_menubar_h__
#define __gui2_menubar_h__

#include "gui2/gui2.h"
#include "gui2/elements/menu.h"
#include "gui2/windows/load.h"
#include "gui2/windows/videocfg.h"
#include "gui2/windows/inputcfg.h"
#include "gui2/windows/soundcfg.h"
#include "gui2/windows/gui_input.h"
#include "gui2/windows/devices.h"
#include "gui2/windows/palettecfg.h"
#include "gui2/windows/options.h"
#include "gui2/windows/mappers.h"
#include "gui2/windows/paths.h"
#include "gui2/windows/supported_mappers.h"
#include "gui2/windows/rom_info.h"
#include "gui2/windows/tracer.h"
#include "gui2/windows/memory_viewer.h"
#include "gui2/windows/nametable_viewer.h"
#include "gui2/windows/patterntable_viewer.h"
#include "gui2/windows/about.h"

#define MAX_MENUS	24

typedef struct menubar_s {
	//basic object information
	gui2_obj_t info;

	//menu information
	menu_t menus[MAX_MENUS];

	//os buttons (minimize, quit, fullscreen toggle)
	button_t buttons[3];

	//file loaded
	load_t load;

	//file loaded
	video_t video;

	//file loaded
	input_t input;

	//file loaded
	sound_t sound;

	//file loaded
	gui_input_t guiinput;

	//file loaded
	devices_t devices;

	//file loaded
	palette_t palette;

	//file loaded                  memory_viewer
	options_t options;

	//file loaded
	mappers_t mappers;

	//file loaded
	paths_t paths;

	//file loaded
	supported_mappers_t supported_mappers;

	//rom info
	rom_info_t rom_info;

	//tracer
	tracer_t tracer;

	//memory viewer
	memory_viewer_t memory_viewer;

	//nametable viewer
	nt_t nametable_viewer;

	//patterntable viewer
	pt_t patterntable_viewer;

	//file loaded
	about_t about;

	//flag, is the window being moved by the user?
	int ismoving;
} menubar_t;

void menubar_draw(menubar_t *m);
int menubar_event(menubar_t *m,int event,int data);
void menubar_create(menubar_t *m);

#endif
