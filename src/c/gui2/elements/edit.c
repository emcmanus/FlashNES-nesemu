#include <stdio.h>
#include <string.h>
#include "gui2/elements/edit.h"

/*

gui2 edit object

-------------------

simple edit control

*/

void edit_draw(edit_t *m)
{
	int x,y;

	if(m->info.w == 0)
		return;
	x = m->info.x;
	y = m->info.y;
	gui_draw_borderpressed(GUI_EDITBACKGROUND,x,y,m->info.w,m->info.h);
	gui_draw_text(GUI_TEXT,x+2,y+2,m->text);
}

int edit_event(edit_t *m,int event,int data)
{
	switch(event) {
		//key pressed
		case E_KEYDOWN:
			return(1);
	}
	return(0);
}

void edit_create(edit_t *m,int x,int y,int w,int h,void (*change)(char*))
{
	memset(m,0,sizeof(edit_t));

	m->info.type = G_EDIT;
	m->info.x = x;
	m->info.y = y;
	m->info.w = w;
	m->info.h = 9;
	m->info.draw = (drawfunc_t)edit_draw;
	m->info.event = (eventfunc_t)edit_event;

	m->change = change;

	strcpy(m->text,"");
}
