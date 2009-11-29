#include <stdio.h>
#include <string.h>
#include "gui2/windows/patterntable_viewer.h"
#include "nes/ppu/palette.h"

/*
gui2 palette object
-------------------

*/

static void draw_tileline(u8 *dest,u8 chrlo,u8 chrhi,u8 attrib)
{
	u8 b0 = ((chrlo >> 1) & 0x55) | (chrhi & 0xAA);
	u8 b1 = (chrlo & 0x55) | ((chrhi << 1) & 0xAA);

	dest[0] = ((b0 >> 6) & 3) | attrib;
	dest[1] = ((b1 >> 6) & 3) | attrib;
	dest[2] = ((b0 >> 4) & 3) | attrib;
	dest[3] = ((b1 >> 4) & 3) | attrib;
	dest[4] = ((b0 >> 2) & 3) | attrib;
	dest[5] = ((b1 >> 2) & 3) | attrib;
	dest[6] = (b0 & 3) | attrib;
	dest[7] = (b1 & 3) | attrib;
}

static void draw_tile(u8 *dest,u8* chr,u8 attrib)
{
	int i;

	for(i=0;i<8;i++)
		draw_tileline(dest+i*264,chr[i],chr[i+8],attrib);
}

static void draw_pt(pt_t *m,int dx,int dy)
{
	int i,x,y,s;
	int color;
	u8 t,*dest = gui_draw_getscreen() + 40 * gui_draw_getscreenpitch() + 4;

	for(i=0;i<64;i++)
		video_setpalentry(i + 0x80,palette[i].r,palette[i].g,palette[i].b);

	for(i=0;i<256;i++) {
		x = i & 15;
		y = i / 16;
		draw_tile(dest+(x*8)+y*8*264,nes->ppu.readpages[(i >> 6) & 3] + (i&0x3F)*16,0xC0);
		x += 16;
		draw_tile(dest+(x*8)+y*8*264,nes->ppu.readpages[((i >> 6) & 3) | 4] + (i&0x3F)*16,0xC0);
	}
}

void pt_draw(pt_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Pattern Tables");
	x++;
	y += 12;
	button_draw(&m->donebtn);
	x += 5;
	y += 30;
	draw_pt(m,x,y);
}

int pt_event(pt_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		default:
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

void pt_create(pt_t *m)
{
	int x,y;

	memset(m,0,sizeof(pt_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 3;
	y = m->info.y = 21;
	m->info.w = 250;
	m->info.h = 210;
	m->info.draw = (drawfunc_t)pt_draw;
	m->info.event = (eventfunc_t)pt_event;

	button_create(&m->donebtn,"X",x+m->info.w-9,y,0);
}


