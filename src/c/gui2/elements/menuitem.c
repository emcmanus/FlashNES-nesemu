#include <string.h>
#include "gui2/elements/menuitem.h"

/*

gui2 menu item object

-------------------

implements the actual menu items in the dropdown menus
*/

void menuitem_draw(menuitem_t *m)
{
	int x,y;
	u8 color = GUI_TEXT;

	x = m->info.x;
	y = m->info.y;
	if(m->click == 0)
		color = GUI_COLOR_DARKGREY;
	if(m->ishighlight && m->click)
		gui_draw_text_highlight(color,GUI_HIGHLIGHTTEXTBG,x,y,m->caption);
	else
		gui_draw_text(color,x,y,m->caption);
}

int menuitem_event(menuitem_t *m,int event,int data)
{
	switch(event) {
		//mouse button pressed
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			return(1);
		case E_MOUSEUP:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			if(m->click)
				m->click();
			return(1);
		case E_MOUSEMOVE:
			m->ishighlight = mouseinrange(&m->info,data);
			break;
	}
	return(0);
}

void menuitem_create(menuitem_t *m,char *cap,int x,int y,void (*click)())
{
	memset(m,0,sizeof(menuitem_t));

	m->info.type = G_MENUITEM;
	m->info.x = x;
	m->info.y = y;
	m->info.w = strlen(cap) * 8 + 2;
	m->info.h = 8;
	m->info.draw = (drawfunc_t)menuitem_draw;
	m->info.event = (eventfunc_t)menuitem_event;

	m->click = click;

	strcpy(m->caption,cap);
}
