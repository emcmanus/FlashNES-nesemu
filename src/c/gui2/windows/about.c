#include <stdio.h>
#include <string.h>
#include "gui2/windows/about.h"

/*

gui2 about object

-------------------

contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void about_draw(about_t *m)
{
	int i,x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"About");
	button_draw(&m->donebtn);
	for(i=0;i<20;i++)
		text_draw(&m->text[i]);
}

int about_event(about_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0)
				m->isshowing = 0;
			break;
	}
	return(0);
}

static char *lines[] = {
	"NESEMU v"VERSION,
	"by James Holodnak",
	"Copyright 2006-2009",
	"",
	"Information gathered from:",
	"  nesdevwiki.org",
	"  nesdev.parodius.com",
	"  nesdev message board",
	" ",
	"Mapper information gathered from:",
	"  Disch's mapper doc zip",
	"  Nestopia v1.40 Sources",
	"  Mapper DLL v1.3 Sources",
	"  FCEUX v2.0.2 Sources",
	" ",
	"Font borrowed from ZSNES",
	0
};

void about_create(about_t *m)
{
	int i,x,y;

	memset(m,0,sizeof(about_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 40;
	y = m->info.y = 20;
	m->info.w = 201;
	m->info.h = 140;
	m->info.draw = (drawfunc_t)about_draw;
	m->info.event = (eventfunc_t)about_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6),y+m->info.h-15,0);

	memset(m->text,0,sizeof(text_t) * 12);

	for(i=0;lines[i];i++)
		text_create(&m->text[i],x+3,y+12+8*i,lines[i]);

}
