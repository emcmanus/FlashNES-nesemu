#include <string.h>
#include "gui2/elements/scrollbar.h"

/*

gui2 menu item object

-------------------

implements the actual menu items in the dropdown menus
*/

void scrollbar_draw(scrollbar_t *m)
{
	int x,y,p;

	x = m->info.x;
	y = m->info.y;
	if(m->orient == 0) {	//vertical
		gui_draw_border(GUI_COLOR_GREY,x,y,8,m->size);
		gui_draw_border(GUI_COLOR_GREY,x,y,8,8);
		gui_draw_border(GUI_COLOR_GREY,x,y+m->size-8,8,8);
		gui_draw_char(GUI_TEXT,x+2,y+2,'\x2');
		gui_draw_char(GUI_TEXT,x+2,y+m->size+2-8,'\x3');

		if(m->max == 0)
			return;
		p = (u32)((double)((double)m->position / (double)m->max) * (double)(m->size - 20));
//		log_message("m->size,max,position - p = %d, %d, %d, %d\n",m->size,m->max,m->position,p);
		gui_draw_border(GUI_COLOR_GREY,x,y+p+9,8,8);
	}
	else {	//horizontal
		gui_draw_border(GUI_COLOR_GREY,x,y,m->size,8);
		gui_draw_border(GUI_COLOR_GREY,x,y,8,8);
		gui_draw_border(GUI_COLOR_GREY,x+m->size-8,y,8,8);
		gui_draw_char(GUI_TEXT,x+2,y+2,'\x2');
		gui_draw_char(GUI_TEXT,x+m->size+2-8,y+2,'\x3');

		if(m->max == 0)
			return;
		p = (u32)((double)((double)m->position / (double)m->max) * (double)(m->size - 20));
//		log_message("m->size,max,position - p = %d, %d, %d, %d\n",m->size,m->max,m->position,p);
		gui_draw_border(GUI_COLOR_GREY,x+p+9,y,8,8);
	}
}

int scrollbar_event(scrollbar_t *m,int event,int data)
{
	mousebutton_u mb;
	mousemove_u mm;
	gui2_obj_t *obj;
	int mx,my,p;
	gui2_obj_t tmp1,tmp2;

	switch(event) {
		//mouse scrollbar pressed
		case E_MOUSEDOWN:
		case E_MOUSEDOWN2:
			mb.data = data;

			//wheel up
			if(mb.info.btn == 3) {
				if(m->click)
					m->position = m->click(m->user,0,3);
				return(1);
			}
			//wheel down
			if(mb.info.btn == 4) {
				if(m->click)
					m->position = m->click(m->user,1,3);
				if(m->position < 0)
					m->position = 0;
				return(1);
			}
			mm.data = data;
			mx = mm.info.x;
			my = mm.info.y;
			obj = &m->info;
			//up button
			if(mx >= obj->x && mx < (obj->x + obj->w)) {
				if(my >= obj->y && my < (obj->y + 8)) {
					if(m->click)
						m->position = m->click(m->user,0,1);
					return(1);
				}
			}
			//down button
			if(mx >= obj->x && mx < (obj->x + obj->w)) {
				if(my >= (obj->y + obj->h - 8) && my < (obj->y + obj->h)) {
					if(m->click)
						m->position = m->click(m->user,1,1);
					if(m->position < 0)
						m->position = 0;
					return(1);
				}
			}
			p = (u32)((double)((double)m->position / (double)m->max) * (double)(m->size - 20));
			tmp1.x = tmp2.x = m->info.x;
			tmp1.y = tmp2.y = m->info.y;
			tmp1.w = tmp2.w = m->info.w;
			tmp1.h = tmp2.h = m->info.h;
			tmp1.h = p + 8;
			tmp2.y = p + 16 + 8;
			//page up
			if(mouseinrange(&tmp1,data) > 0) {
				if(m->click)
					m->position = m->click(m->user,0,20);
				return(1);
			}
			//page down
			if(mouseinrange(&tmp2,data) > 0) {
				if(m->click)
					m->position = m->click(m->user,1,20);
				if(m->position < 0)
					m->position = 0;
				return(1);
			}
			if(mouseinrange(&m->info,data) > 0)
				return(1);
			break;
		case E_MOUSEUP:
			break;
	}
	return(0);
}

void scrollbar_create(scrollbar_t *m,int x,int y,int size,int o,int (*click)(void *,int,int))
{
	memset(m,0,sizeof(scrollbar_t));

	m->info.type = G_SCROLLBAR;
	m->info.x = x;
	m->info.y = y;
	m->info.w = (o == 0) ? 8 : size;
	m->info.h = (o == 0) ? size : 8;
	m->info.draw = (drawfunc_t)scrollbar_draw;
	m->info.event = (eventfunc_t)scrollbar_event;
	m->size = size;
	m->orient = o;
	m->click = click;
}
