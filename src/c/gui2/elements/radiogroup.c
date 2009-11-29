#include <stdio.h>
#include <string.h>
#include "gui2/elements/radiogroup.h"

/*

gui2 radiogroup object

-------------------

simple radiogroup control

*/

void radiogroup_draw(radiogroup_t *m)
{
	int x,y,i;

	x = m->info.x;
	y = m->info.y;
	text_draw(&m->label);
	for(i=0;i<16;i++) {
		if(m->radios[i].click)
			radio_draw(&m->radios[i]);
	}
}

int radiogroup_event(radiogroup_t *m,int event,int data)
{
	int i;

	switch(event) {
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			for(i=0;i<16;i++) {
				if(m->radios[i].click) {
					radio_event(&m->radios[i],event,data);
				}
			}
			if(m->changed)
				m->changed(m->user,m->selected);
			return(1);
		case E_REFRESH:
			for(i=0;i<16;i++) {
				if(m->radios[i].click)
					m->radios[i].state = 0;
			}
			m->radios[data & 0xF].state = 1;
			return(1);
	}
	return(0);
}

#define click_radio(n) \
	static void click_radio##n(void *u,int state) \
{ \
	radiogroup_t *r = (radiogroup_t*)u; \
	int i; \
\
	r->selected = n; \
	for(i=0;i<16;i++) \
		r->radios[i].state = 0; \
	r->radios[n].state = 1;\
}

click_radio(0);click_radio(1);click_radio(2);click_radio(3);
click_radio(4);click_radio(5);click_radio(6);click_radio(7);
click_radio(8);click_radio(9);click_radio(10);click_radio(11);
click_radio(12);click_radio(13);click_radio(14);click_radio(15);

static void (*clicks[16])(void*,int) = {
	click_radio0,click_radio1,click_radio2,click_radio3,
	click_radio4,click_radio5,click_radio6,click_radio7,
	click_radio8,click_radio9,click_radio10,click_radio11,
	click_radio12,click_radio13,click_radio14,click_radio15
};

void radiogroup_create(radiogroup_t *m,int x,int y,char *label,int selected,char *captions[],void (*changed)(void*,int))
{
	int i,n;

	memset(m,0,sizeof(radiogroup_t));

	for(n=0;captions[n];n++);
	m->info.type = G_TEXT;
	m->info.x = x;
	m->info.y = y;
	m->info.w = strlen(label) * 8;
	m->info.h = (1 + n) * 8;
	m->info.draw = (drawfunc_t)radiogroup_draw;
	m->info.event = (eventfunc_t)radiogroup_event;
	m->selected = selected;
	m->changed = changed;

	text_create(&m->label,x,y,label);
	for(i=0;captions[i];i++) {
		radio_create(&m->radios[i],x+3,y+8+i*8,captions[i],0,clicks[i]);
		m->radios[i].user = m;
		if(i == selected)
			m->radios[i].state = 1;
	}
}
