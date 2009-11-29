#include <stdio.h>
#include <string.h>
#include "gui2/elements/text.h"

/*

gui2 text object

-------------------

simple text control

*/

void text_draw(text_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	gui_draw_text(GUI_TEXT,x,y,m->text);
}

int text_event(text_t *m,int event,int data)
{
	switch(event) {
		//key pressed
		case E_KEYDOWN:
			return(1);
	}
	return(0);
}

void text_create(text_t *m,int x,int y,char *text)
{
	memset(m,0,sizeof(text_t));

	m->info.type = G_TEXT;
	m->info.x = x;
	m->info.y = y;
	m->info.w = strlen(text) * 8;
	m->info.h = 10;
	m->info.draw = (drawfunc_t)text_draw;
	m->info.event = (eventfunc_t)text_event;

	strcpy(m->text,text);
}


