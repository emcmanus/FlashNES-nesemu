#include <stdio.h>
#include <string.h>
#include "gui2/windows/memory_viewer.h"
#include "nesemu.h"

/*

gui2 memory_viewer object

-------------------


contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void memory_viewer_draw(memory_viewer_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Memory Viewer");
	button_draw(&m->donebtn);
	list_draw(&m->list);
	radiogroup_draw(&m->source);
}

int memory_viewer_event(memory_viewer_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			list_event(&m->list,event,data);
			radiogroup_event(&m->source,event,data);
			break;
		case E_REFRESH:
			list_event(&m->list,E_REFRESH,0);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0) {
				m->isshowing = 0;
				config_save();
			}
			break;
	}
	return(0);
}

static char *refresh_mem(list_t *l,int m)
{
	static int lines;
	static char ret[256];
	int i;

	//begin refresh
	if(m == 1) {
		lines = 0;
		//success
		return(0);
	}
	//get next item
	else if(m == 0) {
		memory_viewer_t *mv = ((memory_viewer_t*)l->user);

		if(lines == 20)
			return(0);
		switch(mv->source.selected) {
			default:
			case 0:
				sprintf(ret,"$%04X: $%02X $%02X $%02X $%02X-$%02X $%02X $%02X $%02X",
					mv->addr+lines*8,
					dead6502_read(mv->addr+0+lines*8),dead6502_read(mv->addr+1+lines*8),
					dead6502_read(mv->addr+2+lines*8),dead6502_read(mv->addr+3+lines*8),
					dead6502_read(mv->addr+4+lines*8),dead6502_read(mv->addr+5+lines*8),
					dead6502_read(mv->addr+6+lines*8),dead6502_read(mv->addr+7+lines*8));
				break;
			case 1:
				sprintf(ret,"$%04X: $%02X $%02X $%02X $%02X-$%02X $%02X $%02X $%02X",
					mv->addr+lines*8,
					ppumem_read(mv->addr+0+lines*8),ppumem_read(mv->addr+1+lines*8),
					ppumem_read(mv->addr+2+lines*8),ppumem_read(mv->addr+3+lines*8),
					ppumem_read(mv->addr+4+lines*8),ppumem_read(mv->addr+5+lines*8),
					ppumem_read(mv->addr+6+lines*8),ppumem_read(mv->addr+7+lines*8));
				break;
			case 2:
				i = mv->addr+lines*8;
				if(i + 8 <= 0x100)
				sprintf(ret,"$%04X: $%02X $%02X $%02X $%02X-$%02X $%02X $%02X $%02X",
					mv->addr+lines*8,
					nes->sprmem[i+0],
					nes->sprmem[i+1],
					nes->sprmem[i+2],
					nes->sprmem[i+3],
					nes->sprmem[i+4],
					nes->sprmem[i+5],
					nes->sprmem[i+6],
					nes->sprmem[i+7]);
				break;
		}
		lines++;
		return(ret);
	}
	return(0);
}

static void changed_source(void *u,int s)
{
	memory_viewer_t *m = (memory_viewer_t*)u;

	m->addr = 0;
	list_event(&m->list,E_REFRESH,0);
}

static char *source_caps[] = {
	"CPU",
	"PPU",
	"SPRRAM",
	0
};

static int click_scrollbar(void *u,int inc,int num)
{
	list_t *l = (list_t*)u;
	int addr = ((memory_viewer_t*)l->user)->addr;
	int max;

	switch(((memory_viewer_t*)l->user)->source.selected) {
		default:
		case 0:	max = 0x10000 - 0xA0;	break;
		case 1:	max = 0x4000 - 0xA0;		break;
		case 2:	max = 0x100 - 0xA0;		break;
	}
	if(inc)	addr += num * 2 * 8;
	else		addr -= num * 2 * 8;
	if(addr < 0)	addr = 0;
	if(addr >= max)addr = max;
	((memory_viewer_t*)l->user)->addr = addr;
	list_event(l,E_REFRESH,0);
	l->scrollbar.max = max;
	return(addr);
}

void memory_viewer_create(memory_viewer_t *m)
{
	int x,y;
	u8 dip = 0;

	memset(m,0,sizeof(memory_viewer_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 4;
	y = m->info.y = 22;
	m->info.w = 247;
	m->info.h = 171;
	m->info.draw = (drawfunc_t)memory_viewer_draw;
	m->info.event = (eventfunc_t)memory_viewer_event;

	button_create(&m->donebtn,"X",x+m->info.w-10,y,0);

	x += 3;
	y += 2 + 11;

	list_create(&m->list,x,y,240,122,refresh_mem);
	m->list.user = m;
	m->list.scrollbar.click = click_scrollbar;
	m->addr = 0;

	radiogroup_create(&m->source,x,y+125,"Memory Source",0,source_caps,changed_source);
	m->source.user = m;
}
