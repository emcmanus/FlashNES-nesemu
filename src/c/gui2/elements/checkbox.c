#include <stdio.h>
#include <string.h>
#include "gui2/elements/checkbox.h"

/*

gui2 checkbox object

-------------------

simple checkbox control

*/

u8 checkbox_button[7*8] = {
	0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,
	1,1,1,1,1,0,0,
	1,1,1,1,1,3,0,
	1,1,1,1,1,3,0,
	1,1,1,1,1,3,0,
	1,1,1,1,1,3,0,
	0,3,3,3,3,3,0,
};

u8 checkbox_button_pressed[7*8] = {
	0,0,0,0,0,0,2,
	0,0,0,0,0,2,2,
	1,1,1,1,2,2,0,
	2,1,1,2,2,3,0,
	2,2,2,2,2,3,0,
	1,2,2,2,1,3,0,
	1,1,2,1,1,3,0,
	0,3,3,3,3,3,0,
};

void checkbox_draw(checkbox_t *m)
{
	int x,y;
	int i,j;
	u8 *b;

	x = m->info.x;
	y = m->info.y;
	gui_draw_text(GUI_TEXT,x+8,y+1,m->text);
	if(m->state == 0)
		b = checkbox_button;
	else
		b = checkbox_button_pressed;
	for(j=0;j<8;j++) {
		for(i=0;i<7;i++) {
			switch(b[i + (j * 7)]) {
				default: case 0: break;
				case 1: gui_draw_pixel(GUI_COLOR_LIGHTGREY,x+i,y+j); break;
				case 2: gui_draw_pixel(GUI_COLOR_RED,x+i,y+j); break;
				case 3: gui_draw_pixel(GUI_COLOR_DARKGREY,x+i,y+j); break;
			}
		}
	}
}

int checkbox_event(checkbox_t *m,int event,int data)
{
	switch(event) {
		//key pressed
		case E_KEYDOWN:
			return(1);
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			m->state ^= 1;
			if(m->click)
				m->click(m->user,m->state);
			return(1);
	}
	return(0);
}

void checkbox_create(checkbox_t *m,int x,int y,char *text,int state,void (*click)(void*,int))
{
	memset(m,0,sizeof(checkbox_t));

	m->info.type = G_TEXT;
	m->info.x = x;
	m->info.y = y;
	m->info.w = strlen(text) * 8;
	m->info.h = 10;
	m->info.draw = (drawfunc_t)checkbox_draw;
	m->info.event = (eventfunc_t)checkbox_event;

	m->click = click;
	strcpy(m->text,text);
	m->state = state ? 1 : 0;
}
