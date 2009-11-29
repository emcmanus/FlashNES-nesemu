#include <stdio.h>
#include <string.h>
#include "gui2/elements/radio.h"

/*

gui2 radio object

-------------------

simple radio control

*/

u8 radio_button[6*6] = {
	0,0,2,2,0,0,
	0,2,1,1,2,0,
	2,1,1,1,1,2,
	2,1,1,1,1,2,
	0,2,1,1,2,0,
	0,0,2,2,0,0,
};

u8 radio_button_pressed[6*6] = {
	0,0,2,2,0,0,
	0,2,1,1,2,0,
	2,1,3,3,1,2,
	2,1,3,3,1,2,
	0,2,1,1,2,0,
	0,0,2,2,0,0,
};

void radio_draw(radio_t *m)
{
	int x,y;
	int i,j;
	u8 *b;

	x = m->info.x;
	y = m->info.y;
	gui_draw_text(GUI_TEXT,x+8,y+1,m->text);
	if(m->state == 0)
		b = radio_button;
	else
		b = radio_button_pressed;
	for(j=0;j<6;j++) {
		for(i=0;i<6;i++) {
			switch(b[i + (j * 6)]) {
				default:
				case 0: break;
				case 1: gui_draw_pixel(GUI_COLOR_LIGHTGREY,x+i,y+j); break;
				case 2: gui_draw_pixel(GUI_COLOR_GREY,x+i,y+j); break;
				case 3: gui_draw_pixel(GUI_COLOR_DARKGREY,x+i,y+j); break;
			}
		}
	}
}

int radio_event(radio_t *m,int event,int data)
{
	switch(event) {
		//key pressed
		case E_KEYDOWN:
			return(1);
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			if(m->click)
				m->click(m->user,1);
			return(1);
	}
	return(0);
}

void radio_create(radio_t *m,int x,int y,char *text,int state,void (*click)(void*,int))
{
	memset(m,0,sizeof(radio_t));

	m->info.type = G_TEXT;
	m->info.x = x;
	m->info.y = y;
	m->info.w = strlen(text) * 8;
	m->info.h = 10;
	m->info.draw = (drawfunc_t)radio_draw;
	m->info.event = (eventfunc_t)radio_event;

	m->click = click;
	strcpy(m->text,text);
	m->state = state;
}
