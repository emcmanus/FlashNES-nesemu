/*
gui2 - a zsnes lookalike gui for nesemu

uses mouse and keyboard for input
*/

#include <string.h>
#include "gui2/elements/menubar.h"
#include "gui2/elements/scrollbar.h"
#include "misc.h"
#ifndef PS2
//#include "screenshot.h"
#endif

u32 gui_palette[] = {
	//0x00
	0x000000,
	0x404040,
	0x808080,
	0xC0C0C0,
	0xFFFFFF,

	0x800000,
	0x008000,
	0x000080,

	0xC00000,
	0x00C000,
	0x0000C0,

	0xFF0000,
	0x00FF00,
	0x0000FF,

	0xFFFF00,
	0xC0C000,
	//0x10
	0xFF8000,
	0xE07000,
	0xFF9010,
	0xFF00FF,

	0x00FFFF,
	0x000000,
	0x000000,
	0x000000,

	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,
	0x000000,

	//0x20
	0x000080,
	0x000090,
	0x0000A0,
	0x0000B0,
	0x0000C0,
	0x0000D0,
	0x0000E0,
	0x0000F0,
	0x0000FF,

	//0x29

	0xFFFFFFFF
};

#ifdef B
#undef B
#endif
#ifdef R
#undef R
#endif
#define B GUI_COLOR_LIGHTGREY + GUI_COLOR_OFFSET
#define R GUI_COLOR_BLUE + GUI_COLOR_OFFSET
u8 disk_read[24][24] = {
	{B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B,0},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B,0,0},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B,0,0,0},
	{B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,0,0,0,0},
};
#undef R
#define R GUI_COLOR_RED + GUI_COLOR_OFFSET
u8 disk_write[24][24] = {
	{B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B,0},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B,0,0},
	{B,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,R,B,0,0,0},
	{B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,0,0,0,0},
};
#undef B
#undef R

//the menubar is the root gui object
menubar_t *root;

int mousex,mousey;

int mouseinrange(gui2_obj_t *obj,int data)
{
	mousemove_u mm;
	int mx,my;

	mm.data = data;
	mx = mm.info.x;
	my = mm.info.y;

	if(mx >= obj->x && mx < (obj->x + obj->w))
		if(my >= obj->y && my < (obj->y + obj->h))
			return(1);
	return(0);
}

void gui2_init()
{
	if(root == 0)
		root = (menubar_t*)malloc(sizeof(menubar_t));
	menubar_create(root);

}

void gui2_kill()
{
	log_message("killing gui2...\n");
	if(root)
		free(root);
	root = 0;
}

extern u32 gui_palette[];
extern int gui_active;
extern u8 *nesscr;

void gui2_draw(u8 *scr,int pitch)
{
	int i;

	gui_draw_setscreen(scr,pitch);
	for(i=0;gui_palette[i] != 0xFFFFFFFF;i++) {
		u32 c = gui_palette[i];
		video_setpalentry(i + GUI_COLOR_OFFSET,(c >> 16) & 0xFF,(c >> 8) & 0xFF,c & 0xFF);
	}
	for(i=0;i<pitch*240;i++)
		scr[i] = 0;
	menubar_draw(root);
	gui_draw_cursor();
}

/*
gui2_event(int id,int event,int data);

id = id of item in gui that receives the message
event = event to send
data = event data
*/
void gui2_event(int event,int data)
{
	if(event == E_MOUSEMOVE) {
		mousemove_u mm;

		mm.data = data;
		mousex = mm.info.x;
		mousey = mm.info.y;
	}
	menubar_event(root,event,data);
}

//poll input for gui2
void gui2_poll()
{
	static u32 old_joy1 = 0;
	u32 new_joy1,joy1 = 0;

	//hack to make sure key isnt held down
	if(joykeys[config.gui_keys[0]])	joy1 |= INPUT_MENU;
	if(joykeys[config.gui_keys[1]])	joy1 |= INPUT_LOADSTATE;
	if(joykeys[config.gui_keys[2]])	joy1 |= INPUT_SAVESTATE;
	if(joykeys[config.gui_keys[4]])	joy1 |= INPUT_SCREENSHOT;
	new_joy1 = joy1 & ~old_joy1;
	old_joy1 = joy1;
	//end hack
	if(new_joy1 & INPUT_MENU) {
		//dont let user exit gui if rom is not loaded
		if((nes == 0) || (nes->rom == 0))
			return;
		gui_active ^= 1;
		memset(nesscr,GUI_COLOR_OFFSET,(256+8)*240);
		if(gui_active == 0)
			sound_play();
		else {
			sound_pause();
			video_copyscreen();
		}
	}
	else if(gui_active) return;
	else if(new_joy1 & INPUT_LOADSTATE)	loadstate();
	else if(new_joy1 & INPUT_SAVESTATE)	savestate();
#ifdef SCREENSHOTS
	else if(new_joy1 & INPUT_SCREENSHOT)screenshot();
#endif
}

//control types
#define G_BUTTON		1				//a button
#define G_EDIT			2				//single line edit control
#define G_TEXT			3				//some text
#define G_MENU			4				//one menu, containing menu items
#define G_MENUITEM	5				//menu item
#define G_MENUBAR		6				//menu bar, contains menus and special buttons
#define G_LIST			7				//list/dirlist/filelist control all in one
#define G_SCROLLBAR	8				//a horizontal or vertical scrollbar

//create object
void gui_obj_create(int type,gui2_obj_t *obj,int x,int y,int w,int h)
{
	obj->type = type;
	switch(type) {
		case G_BUTTON:    memset(obj,0,sizeof(button_t));    break;
		case G_EDIT:      memset(obj,0,sizeof(edit_t));      break;
		case G_TEXT:      memset(obj,0,sizeof(text_t));      break;
		case G_MENU:      memset(obj,0,sizeof(menu_t));      break;
		case G_MENUITEM:  memset(obj,0,sizeof(menuitem_t));  break;
		case G_MENUBAR:   memset(obj,0,sizeof(menubar_t));   break;
		case G_LIST:      memset(obj,0,sizeof(list_t));      break;
		case G_SCROLLBAR: memset(obj,0,sizeof(scrollbar_t)); break;
	}
	obj->x = x;
	obj->y = y;
	obj->w = w;
	obj->h = h;
}

//object event
void gui_obj_event(gui2_obj_t *obj,int event,int data)
{
	obj->event(obj,event,data);
}

//draw object
void gui_obj_draw(gui2_obj_t *obj)
{
	obj->draw(obj);
}
