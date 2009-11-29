#include <stdio.h>
#include <string.h>
#include "gui2/elements/list.h"
#include "system/config.h"

/*

gui2 list object

-------------------

simple list control

*/

void list_draw(list_t *m)
{
	int x,y,i,j,lines;

	x = m->info.x;
	y = m->info.y;
	gui_draw_borderpressed(GUI_EDITBACKGROUND,x,y,m->info.w,m->info.h);
	x += 2;
	y += 2;
	lines = (m->info.h - 0) / 6;
	j = m->viewstart;
	for(i=0;(i<lines)&&(j<m->numitems);i++,j++) {
		if(j == m->selected)
			gui_draw_text2_highlight(GUI_TEXT,GUI_HIGHLIGHTTEXTBG,x,y+i*6,(m->info.w/6)-1,m->items[j]);
		else
			gui_draw_text2(GUI_TEXT,x,y+i*6,(m->info.w/6)-1,m->items[j]);
	}
	scrollbar_draw(&m->scrollbar);
}

int list_event(list_t *m,int event,int data)
{
	char *str;
	int y;
	mousemove_u mm;

	if(scrollbar_event(&m->scrollbar,event,data) > 0)
		return(1);
	switch(event) {
		//key pressed
//		case E_KEYDOWN:
//			return(1);
		case E_MOUSEDOWN2:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			mm.data = data;
			y = mm.info.y - 1 - m->info.y;
			m->selected = y / 6 + m->viewstart;
			if(m->selected >= m->numitems)
				m->selected = m->numitems - 1;
			if(event == E_MOUSEDOWN) {
				if(m->click)
					m->click(m->user,m->items[m->selected]);
			}
			else {
				if(m->click2)
					m->click2(m->user,m->items[m->selected]);
			}
			return(1);
		case E_REFRESH:
			if(m->refresh(m,1) == 0) {	//start refresh, dont continue if error
				if(m->click)
					m->click(m->user,0);
				m->numitems = 0;
				m->viewstart = 0;
				m->scrollbar.max = 0;
				//if we need the ..
				if(data)
					strcpy(m->items[m->numitems++],"..");
				//copy all items to list
				for(m->selected=-1;(str=m->refresh(m,0)) != 0;)
					strcpy(m->items[m->numitems++],str);
				m->scrollbar.max = m->numitems;
			}
			if(m->refresh(m,1+2) == 0) {	//start refresh, dont continue if error
				//copy all items to list
				for(m->selected=-1;(str=m->refresh(m,0+2)) != 0;)
					sprintf(m->items[m->numitems++],"[%s]",str);
				m->scrollbar.max = m->numitems;
			}
			return(1);
	}
	return(0);
}

static int click_scrollbar(void *u,int direction,int increment)
{
	list_t *l = (list_t*)u;
	int n;

	//up
	if(direction == 0) {
		l->viewstart -= increment;
		if(l->viewstart < 0)
			l->viewstart = 0;
	}
	//down
	else {
		l->viewstart += increment;
		n = l->numitems - ((l->info.h) / 6);
		if(l->viewstart >= n)
			l->viewstart = n;
		else if(n < 0)
			l->viewstart = 0;
	}
	return(l->viewstart);
}

void list_create(list_t *m,int x,int y,int w,int h,char *(*refresh)(list_t*,int))
{
	memset(m,0,sizeof(list_t));

	m->info.type = G_LIST;
	m->info.x = x;
	m->info.y = y;
	m->info.w = w;
	m->info.h = h;
	m->info.draw = (drawfunc_t)list_draw;
	m->info.event = (eventfunc_t)list_event;

	m->refresh = refresh;

	m->selected = -1;

	strcpy(m->path,config.path_rom);

	scrollbar_create(&m->scrollbar,x + w - 8,y,h,0,click_scrollbar);
	m->scrollbar.user = m;
}
