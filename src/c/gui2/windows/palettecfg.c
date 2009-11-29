#include <stdio.h>
#include <string.h>
#include "gui2/windows/palettecfg.h"
#include "nes/ppu/palette.h"

/*
gui2 palette object
-------------------

*/

static void draw_palette(palette_t *m,int dx,int dy)
{
	int i,x,y;
	int color;

	for(i=0;i<64;i++)
		video_setpalentry(i + 0x80,palette[i].r,palette[i].g,palette[i].b);
	for(y=0;y<4;y++) {
		for(x=0;x<16;x++) {
			color = x + (y * 16) + 0xC0;
			gui_draw_fillbox(color,dx+x*8,dy+y*8,8,8);
		}
	}
}

void palette_draw(palette_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Palette");
	x++;
	y += 12;
	button_draw(&m->donebtn);
	text_draw(&m->huetext);
	text_draw(&m->sattext);
	edit_draw(&m->hueedit);
	edit_draw(&m->satedit);
	x += 5;
	y += 30;
	draw_palette(m,x,y);
}

int palette_event(palette_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			text_event(&m->huetext,event,data);
			text_event(&m->sattext,event,data);
			edit_event(&m->hueedit,event,data);
			edit_event(&m->satedit,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0)
				m->isshowing = 0;
			break;
	}
	return(0);
}

static void change_hue(char *s)
{
	config.hue = (u32)atoi(s);
	palette_generate(config.hue,config.sat);
}

static void change_sat(char *s)
{
	config.sat = (u32)atoi(s);
	palette_generate(config.hue,config.sat);
}

void palette_create(palette_t *m)
{
	int x,y;

	memset(m,0,sizeof(palette_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 35;
	y = m->info.y = 60;
	m->info.w = 140;
	m->info.h = 95;
	m->info.draw = (drawfunc_t)palette_draw;
	m->info.event = (eventfunc_t)palette_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6),y+m->info.h-15,0);

	text_create(&m->huetext,x+3,y+12+10*0+3,"Hue:");
	text_create(&m->sattext,x+3,y+12+10*1+3,"Saturation:");

	edit_create(&m->hueedit,x+3+70,y+12+10*0,6*4+4,0,change_hue);
	edit_create(&m->satedit,x+3+70,y+12+10*1,6*4+4,0,change_sat);

	sprintf(m->hueedit.text,"%d",config.hue);
	sprintf(m->satedit.text,"%d",config.sat);

	palette_generate(config.hue,config.sat);
}
