#include <stdio.h>
#include <string.h>
#include "gui2/windows/nametable_viewer.h"
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

static void draw_nt(nt_t *m,int dx,int dy,int sx,int sy)
{
	int i,x,y,s;
	int color;
	u8 *dest = gui_draw_getscreen() + dx + gui_draw_getscreenpitch() * dy;
	u8 *nt,t,at,ntnum,pt;

	for(i=0;i<64;i++)
		video_setpalentry(i + 0x80,palette[i].r,palette[i].g,palette[i].b);
	pt = (nes->ppu.ctrl0 & 0x10) >> 2;
//	nametable = (nes->ppu.scroll >> 10) & 3;
//	nametableptr = nes->ppu.readpages[8 | nametable];
	ntnum = 8;
	for(y=0;y<23;y++) {
		if((y + sy) >= 32)
			ntnum |= 2;
		for(x=0;x<30;x++) {
			if((x + sx) >= 32)
				ntnum |= 1;
			s = (x + sx) + (y + sy) * 32;
			t = nes->ppu.readpages[ntnum][s];
			at = nes->ppu.readpages[ntnum][0x3C0 + ((s >> 2) & 7) + (((s >> 7) & 7) << 3)];
			at = (at >> (((s & 2) | (((s >> 5) & 2) << 1)))) & 3;
			at = (at << 2) | 0xC0;
			draw_tile(dest+(x*8)+y*8*264,nes->ppu.readpages[pt|(t>>6)]+(t&0x3F)*16,at);
		}
	}
}

static u8 ntx,nty;

void nt_draw(nt_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Nametables");
	x++;
	y += 12;
	button_draw(&m->donebtn);
	scrollbar_draw(&m->hscrollbar);
	scrollbar_draw(&m->vscrollbar);
	x = m->info.x+1;
	y = m->info.y+10;
	draw_nt(m,x,y,ntx,nty);
}

int nt_event(nt_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			scrollbar_event(&m->hscrollbar,event,data);
			scrollbar_event(&m->vscrollbar,event,data);
			break;
		case E_MOUSEUP:
			if(scrollbar_event(&m->hscrollbar,event,data) > 0)
				return(0);
			if(scrollbar_event(&m->vscrollbar,event,data) > 0)
				return(0);
			if(button_event(&m->donebtn,event,data) > 0)
				m->isshowing = 0;
			break;
	}
	return(0);
}

static int click_hscrollbar(void *u,int direction,int increment)
{
	nt_t *l = (nt_t*)u;
	int n = (direction ? 1 : -1) * increment;

	if(((int)ntx - n) < 0)
		ntx = 0;
	else if((ntx += (u8)n) > 64)
		ntx = 64;
	return(0);
}

static int click_vscrollbar(void *u,int direction,int increment)
{
	nt_t *l = (nt_t*)u;
	int n = (direction ? 1 : -1) * increment;

	if(((int)nty - n) < 0)
		nty = 0;
	else if((nty += (u8)n) > 64)
		nty = 64;
	return(0);
}

void nt_create(nt_t *m)
{
	int x,y;

	memset(m,0,sizeof(nt_t));

	ntx = nty = 0;
	m->info.type = G_WINDOW;
	x = m->info.x = 3;
	y = m->info.y = 21;
	m->info.w = 250;
	m->info.h = 202;
	m->info.draw = (drawfunc_t)nt_draw;
	m->info.event = (eventfunc_t)nt_event;

	button_create(&m->donebtn,"X",x+m->info.w-9,y,0);

	scrollbar_create(&m->hscrollbar,x+1,y+m->info.h-8,m->info.w-9,1,click_hscrollbar);
	scrollbar_create(&m->vscrollbar,x+m->info.w-8,y+10,m->info.h-19,0,click_vscrollbar);
	m->hscrollbar.user = m->vscrollbar.user = m;
	m->hscrollbar.max = m->vscrollbar.max = 64;
}

