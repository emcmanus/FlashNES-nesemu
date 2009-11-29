#include <stdio.h>
#include <string.h>
#include "gui2/windows/paths.h"
#include "nesemu.h"

/*

gui2 paths object

-------------------


contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/
extern u8 genie_rom[];
extern u8 disksys_rom[];
extern u8 hle_fds_rom[];
extern u8 nsf_bios[];

void paths_draw(paths_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"paths Configuration");
	button_draw(&m->donebtn);
}

int paths_event(paths_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0) {
				m->isshowing = 0;
				config_save();
			}
			break;
	}
	return(0);
}

void paths_create(paths_t *m)
{
	int x,y;

	memset(m,0,sizeof(paths_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 20;
	y = m->info.y = 40;
	m->info.w = 200;
	m->info.h = 100;
	m->info.draw = (drawfunc_t)paths_draw;
	m->info.event = (eventfunc_t)paths_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6),124,0);

	x += 3;
	y += 2 + 11;
}
