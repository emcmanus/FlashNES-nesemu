#include <stdio.h>
#include <string.h>
#include "gui2/windows/tracer.h"
#include "nesemu.h"

/*

gui2 tracer object

-------------------

contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void tracer_draw(tracer_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Tracer");
	button_draw(&m->donebtn);
	button_draw(&m->stepbtn);
	button_draw(&m->steplinebtn);
	m->disasm.scrollbar.max = 0x10000;
	list_draw(&m->disasm);
	for(x=0;x<32;x++)
		text_draw(&m->labels[x]);
}

int tracer_event(tracer_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			button_event(&m->stepbtn,event,data);
			button_event(&m->steplinebtn,event,data);
			list_event(&m->disasm,event,data);
			break;
		case E_REFRESH:
			list_event(&m->disasm,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0) {
				m->isshowing = 0;
				config_save();
			}
			button_event(&m->stepbtn,event,data);
			button_event(&m->steplinebtn,event,data);
			break;
	}
	return(0);
}

extern int ppucycles;
extern u32 ppuframes;

static char *refresh_disasm(list_t *l,int m)
{
	static int lines;
	static u32 pc,oldpc;
	static char ret[256],buf[256];
	tracer_t *t = (tracer_t*)l->user;

	//begin refresh
	if(m == 1) {
		lines = 0;
		sprintf(t->labels[0].text,"CPU Registers");
		sprintf(t->labels[1].text," A $%02X   S $%02X",nes->cpu.a,nes->cpu.s);
		sprintf(t->labels[2].text," X $%02X   F $%02X",nes->cpu.x,nes->cpu.f);
		sprintf(t->labels[3].text," Y $%02X",nes->cpu.y);
		sprintf(t->labels[4].text," PC $%04X",nes->cpu.pc);
		sprintf(t->labels[5].text,"");
		sprintf(t->labels[6].text,"PPU Registers");
		sprintf(t->labels[7].text," Control 0   $%02X",nes->ppu.ctrl0);
		sprintf(t->labels[8].text," Control 1   $%02X",nes->ppu.ctrl0);
		sprintf(t->labels[9].text," Status      $%02X",nes->ppu.status);
		sprintf(t->labels[10].text," Scroll X    $%02X",nes->ppu.scrollx);
		sprintf(t->labels[11].text," Scroll      $%04X",nes->ppu.scroll);
		sprintf(t->labels[12].text," Scroll Temp $%04X",nes->ppu.temp);
		sprintf(t->labels[13].text," SPRRAM Addr $%02X",nes->ppu.spraddr);
		sprintf(t->labels[14].text," Read Buffer $%02X",nes->ppu.buf);
		sprintf(t->labels[15].text," Read Latch  $%02X",nes->ppu.latch);
		sprintf(t->labels[16].text," Cycles   %d",ppucycles);
		sprintf(t->labels[17].text," Frames   %ld",ppuframes);
		pc = ((tracer_t*)l->user)->pc;
		//success
		return(0);
	}
	//get next item
	else if(m == 0) {
		if(lines == 29)
			return(0);
		oldpc = pc;
		pc = dead6502_disassembleopcode(buf,pc);
		sprintf(ret,"$%04X: %s",oldpc,buf);
		if(oldpc == nes->cpu.pc)
			l->selected = lines;
		lines++;
		return(ret);
	}
	return(0);
}

static int click_disasm_scrollbar(void *u,int inc,int num)
{
	list_t *l = (list_t*)u;
	int pc = ((tracer_t*)l->user)->pc;

	if(inc)
		pc += num * 4;
	else
		pc -= num * 4;
	if(pc < 0)
		pc = 0;
	if(pc >= 0xFFE8)
		pc = 0xFFE8;
	((tracer_t*)l->user)->pc = pc;
	list_event(l,E_REFRESH,0);
	l->scrollbar.max = 0xFFE8;
	return(pc);
}

static void click_step(void *u)
{
	tracer_t *m = (tracer_t*)u;

	ppu_event_step();
	m->pc = nes->cpu.pc - 20;
	list_event(&m->disasm,E_REFRESH,0);
}

static void click_stepline(void *u)
{
	tracer_t *m = (tracer_t*)u;
	int i;

	for(i=0;i<113;i++)
		ppu_event_step();
	m->pc = nes->cpu.pc - 20;
	list_event(&m->disasm,E_REFRESH,0);
}

void tracer_create(tracer_t *m)
{
	int x,y,i;
	u8 dip = 0;

	memset(m,0,sizeof(tracer_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 6;
	y = m->info.y = 20;
	m->info.w = 242;
	m->info.h = 192;
	m->info.draw = (drawfunc_t)tracer_draw;
	m->info.event = (eventfunc_t)tracer_event;

	button_create(&m->donebtn,"X",x+m->info.w-10,y,0);

	x += 3;
	y += 2 + 11;

	list_create(&m->disasm,x,y,124,176,refresh_disasm);
	m->disasm.user = m;

	m->disasm.scrollbar.click = click_disasm_scrollbar;

	x += 128;

	for(i=0;i<32;i++)
		text_create(&m->labels[i],x,y+i*8,"");

	button_create(&m->stepbtn,"Step",x,y+150,click_step);
	button_create(&m->steplinebtn,"Step Line",x,y+150+12,click_stepline);

	m->stepbtn.user = m;
	m->steplinebtn.user = m;

	m->pc = 0x8000;
}
