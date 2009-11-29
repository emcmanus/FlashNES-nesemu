#include <string.h>
#include "gui2/elements/button.h"

/*

gui2 menu item object

-------------------

implements the actual menu items in the dropdown menus
*/

void button_draw(button_t *m)
{
	int x,y;

	if(m->info.w == 0)
		return;
	x = m->info.x;
	y = m->info.y;
	if(m->ispressed)
		gui_draw_buttonpressed(m->caption,x,y,m->info.w,m->info.h);
	else
		gui_draw_button(m->caption,x,y,m->info.w,m->info.h);
}

int button_event(button_t *m,int event,int data)
{
	switch(event) {
		//mouse button pressed
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) > 0) {
				m->ispressed = 1;
				return(1);
			}
			break;
		case E_MOUSEUP:
			m->ispressed = 0;
			if(mouseinrange(&m->info,data) > 0) {
				if(m->click)
					m->click(m->user);
				return(1);
			}
			break;
	}
	return(0);
}

void button_create(button_t *m,char *cap,int x,int y,void (*click)(void*))
{
	memset(m,0,sizeof(button_t));

	m->info.type = G_BUTTON;
	m->info.x = x;
	m->info.y = y;
	m->info.w = strlen(cap) * 7 + 2;
	m->info.h = 9;
	m->info.draw = (drawfunc_t)button_draw;
	m->info.event = (eventfunc_t)button_event;

	m->click = click;

	strcpy(m->caption,cap);
}

