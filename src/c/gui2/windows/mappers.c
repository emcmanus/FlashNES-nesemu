#include <stdio.h>
#include <string.h>
#include "gui2/windows/mappers.h"
#include "nesemu.h"
#include "mappers/mapper.h"

/*

gui2 mappers object

-------------------


contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void mapper105_draw(mappers_t *m)
{
	text_draw(&m->m105_label);
	checkbox_draw(&m->m105_check[0]);
	checkbox_draw(&m->m105_check[1]);
	checkbox_draw(&m->m105_check[2]);
	checkbox_draw(&m->m105_check[3]);
}

void mapper105_event(mappers_t *m,int event,int data)
{
	checkbox_event(&m->m105_check[0],event,data);
	checkbox_event(&m->m105_check[1],event,data);
	checkbox_event(&m->m105_check[2],event,data);
	checkbox_event(&m->m105_check[3],event,data);
}

void mappers_draw(mappers_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	button_draw(&m->donebtn);
	if(nes->rom) {
		switch(nes->rom->mapper) {
			//nes-event
			case 105:
				gui_draw_text(GUI_TEXT,x+2,y+2,"NES-EVENT Configuration");
				mapper105_draw(m);
				break;
			//unif
			case -1:
			default:
				gui_draw_text(GUI_TEXT,x+2,y+2,"Mapper Configuration");
				text_draw(&m->label);
				break;
		}
	}
}

int mappers_event(mappers_t *m,int event,int data)
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
		case E_REFRESH:
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0) {
				m->isshowing = 0;
				config_save();
			}
			break;
	}
	if(nes->rom) {
		switch(nes->rom->mapper) {
			//nes-event
			case 105:
				mapper105_event(m,event,data);
				break;
		}
	}
	return(0);
}

extern mapper_ines_t mapper105;

static u8 data[16];

void click_mapper105_check0(void *u,int s)
{
	mapper105.state(STATE_SAVECFG,data);
	data[0] = (data[0] & ~1) | (s << 0);
	mapper105.state(STATE_LOADCFG,data);
}

void click_mapper105_check1(void *u,int s)
{
	mapper105.state(STATE_SAVECFG,data);
	data[0] = (data[0] & ~2) | (s << 1);
	mapper105.state(STATE_LOADCFG,data);
}

void click_mapper105_check2(void *u,int s)
{
	mapper105.state(STATE_SAVECFG,data);
	data[0] = (data[0] & ~4) | (s << 2);
	mapper105.state(STATE_LOADCFG,data);
}

void click_mapper105_check3(void *u,int s)
{
	mapper105.state(STATE_SAVECFG,data);
	data[0] = (data[0] & ~8) | (s << 3);
	mapper105.state(STATE_LOADCFG,data);
}

void mappers_create(mappers_t *m)
{
	int x,y;
	u8 dip = 0;

	memset(m,0,sizeof(mappers_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 20;
	y = m->info.y = 40;
	m->info.w = 200;
	m->info.h = 100;
	m->info.draw = (drawfunc_t)mappers_draw;
	m->info.event = (eventfunc_t)mappers_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6),124,0);

	x += 3;
	y += 2 + 11;

	text_create(&m->label,x,y,"Nothing to configure.");

	//mapper 105 controls
	text_create(&m->m105_label,x,y,"Time Limit");
	checkbox_create(&m->m105_check[0],x,y+10*1,"DIP1",(dip >> 0) & 1,click_mapper105_check0);
	checkbox_create(&m->m105_check[1],x,y+10*2,"DIP2",(dip >> 1) & 1,click_mapper105_check1);
	checkbox_create(&m->m105_check[2],x,y+10*3,"DIP3",(dip >> 2) & 1,click_mapper105_check2);
	checkbox_create(&m->m105_check[3],x,y+10*4,"DIP4",(dip >> 3) & 1,click_mapper105_check3);
}
