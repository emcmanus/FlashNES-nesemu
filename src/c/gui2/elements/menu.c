#include <stdio.h>
#include <string.h>
#include "gui2/elements/menu.h"
#include "gui2/elements/menuitem.h"

/*

gui2 menu object

-------------------

implements the dropdown menus
*/

void menu_draw(menu_t *m)
{
	int x,y;
	int i,j,maxwidth;

	x = m->info.x;
	y = m->info.y;
	if(m->ispressed) {
		for(maxwidth=0,i=0;m->items[i].info.x;i++) {
			if((j = strlen(m->items[i].caption)) > maxwidth)
				maxwidth = j;
		}
//		gui_draw_fillbox(GUI_COLOR_DARKGREY,x+5,y+13+5,maxwidth * 6 + 4,i * 8 + 1);
		gui_draw_buttonpressed(m->caption,x,y,m->info.w,m->info.h);
		gui_draw_border(GUI_COLOR_GREY,x,y+13,maxwidth * 6 + 4,i * 8 + 1);
		for(i=0;m->items[i].info.x;i++) {
			if(strcmp(m->items[i].caption,"") == 0)
				gui_draw_borderpressed(0,x+2,y+13+3+i*8,maxwidth*6,1);
			else
				menuitem_draw(&m->items[i]);
		}
	}
	else
		gui_draw_button(m->caption,x,y,m->info.w,m->info.h);
}

int menu_event(menu_t *m,int event,int data)
{
	int i;

	switch(event) {
		//mouse button pressed
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			//see if the click was on a menu
			m->ispressed ^= 1;
			if(m->click)
				m->click(m->user);
			//send event off to children
			for(i=0;i<24 && m->items[i].info.x;i++)
				if(menuitem_event(&m->items[i],event,data))
					m->ispressed = 0;
			return(1);
		case E_MOUSEUP:
			if(m->ispressed == 0)
				return(0);
//			m->ispressed = 0;
			//send event off to children
			for(i=0;i<24 && m->items[i].info.x;i++)
				if(menuitem_event(&m->items[i],event,data))
					m->ispressed = 0;
			break;
		case E_MOUSEMOVE:
			//send event off to children
			for(i=0;i<24 && m->items[i].info.x;i++)
				menuitem_event(&m->items[i],event,data);
			break;
	}
	return(0);
}

void menu_create(menu_t *m,char *cap,int x,menuitems_t *items)
{
	int i;

	memset(m,0,sizeof(menu_t));

	m->info.type = G_MENU;
	m->info.x = x;
	m->info.y = 3;
	m->info.w = strlen(cap) * 6 + 3;
	m->info.h = 9;
	m->info.draw = (drawfunc_t)menu_draw;
	m->info.event = (eventfunc_t)menu_event;

	strcpy(m->caption,cap);

	for(i=0;items[i].caption;i++) {
		menuitem_create(&m->items[i],items[i].caption,x+2,15+3+i*8,items[i].click);
	}
}
