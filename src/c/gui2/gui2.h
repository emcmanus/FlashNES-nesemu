#ifndef __gui2_h__
#define __gui2_h__

#include "gui2/colors.h"
#include "gui2/gui2_draw.h"
#include "system/system.h"

#ifdef PS2
#define MAX_FILES		4096				//4k files
#else
#define MAX_FILES		(1024 * 24)		//24k files
#endif

#define E_KEYDOWN		0x0001		//key down
#define E_KEYUP		0x0002		//key up
#define E_MOUSEDOWN2	0x0008		//double click
#define E_MOUSEDOWN	0x0010		//MOUSE BUTTON PRESSED
#define E_MOUSEUP		0x0020		//mouse button released
#define E_MOUSEMOVE	0x0040		//mouse changed position
#define E_REFRESH		0x0100		//refresh

//control types
#define G_BUTTON		1				//a button
#define G_EDIT			2				//single line edit control
#define G_TEXT			3				//some text
#define G_MENU			4				//one menu, containing menu items
#define G_MENUITEM	5				//menu item
#define G_MENUBAR		6				//menu bar, contains menus and special buttons
#define G_LIST			7				//list/dirlist/filelist control all in one
#define G_SCROLLBAR	8				//a horizontal or vertical scrollbar
#define G_WINDOW		9				//a window

typedef void (*drawfunc_t)(void*);
typedef int (*eventfunc_t)(void*,int,int);

typedef union {
	int data;
	struct {
		u8 x,y;			//mouse position
		s8 xrel,yrel;	//mouse relative postion
	} info;
} mousemove_u;

typedef union {
	int data;
	struct {
		u8 x,y;		//mouse position
		u8 btn;		//mouse buttons down
		u8 unused;	//unused byte
	} info;
} mousebutton_u;

typedef struct gui2_obj_s {
	//object type
	int type;

	//objects flags
	int flags;

	//object position and size
	int x,y;
	int w,h;

	//objects functions
	drawfunc_t draw;
	eventfunc_t event;
} gui2_obj_t;

u8 *gui_draw_getscreen();
int gui_draw_getscreenpitch();
int mouseinrange(gui2_obj_t *obj,int data);
void gui2_init();
void gui2_kill();
void gui2_draw(u8 *scr,int pitch);
void gui2_event(int event,int data);
void gui2_show();
void gui2_hide();
void gui2_poll();

//create object
void gui_obj_create(int type,gui2_obj_t *obj,int x,int y,int w,int h);

//object event
void gui_obj_event(gui2_obj_t *obj,int event,int data);

//draw object
void gui_obj_draw(gui2_obj_t *obj);

#endif
